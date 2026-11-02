#include "simulation/simulation.hpp"
#include "npc/factory.hpp"
#include "combat/visitor.hpp"
#include "world/world.hpp"

#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <thread>
#include <utility>

namespace game::simulation {
double step_length(npc::Kind kind) {
    switch (kind) {
    case npc::Kind::Orc:
        return 20;
    case npc::Kind::Squirrel:
        return 5;
    case npc::Kind::Druid:
        return 10;
    }
    throw std::invalid_argument("type");
}

double reflect(double value, double limit) {
    value = std::fmod(value, 2 * limit);
    if (value < 0)
        value += 2 * limit;
    return value > limit ? 2 * limit - value : value;
}

bool Simulation::finished() const { return stop_.load() || Clock::now() >= deadline_; }

bool Simulation::stop_requested() const { return stop_.load(); }

bool Simulation::battle_ready() const { return stop_requested() || !queue_.empty(); }

void Simulation::movement_loop() {
    std::mt19937_64 random(seed_ + 1);
    while (!finished()) {
        movement(random);
        pause();
    }
}

void Simulation::combat_loop() {
    std::mt19937_64 random(seed_ + 2);
    std::vector<Battle> task;
    while (!finished())
        if (pop(task))
            combat(task, random);
}

void Simulation::stop() {
    {
        // Synchronize with both predicates before notifying their waiting threads.
        std::scoped_lock lock(queue_mutex_, wait_mutex_);
        stop_.store(true);
    }
    queue_cv_.notify_all();
    wait_cv_.notify_all();
}

void Simulation::guarded(const std::function<void()> &work) {
    try {
        work();
    } catch (...) {
        std::lock_guard lock(error_mutex_);
        if (!error_)
            error_ = std::current_exception();
        stop();
    }
}

void Simulation::pause() {
    std::unique_lock lock(wait_mutex_);
    wait_cv_.wait_until(lock, std::min(deadline_, Clock::now() + tick_),
                        std::bind_front(&Simulation::stop_requested, this));
}

void Simulation::movement(std::mt19937_64 &random) {
    std::vector<Battle> candidates;
    {
        std::lock_guard lock(world_mutex_);
        std::uniform_real_distribution<double> angle(0, 2 * std::acos(-1.));
        for (const auto &npc : npcs_)
            if (npc->alive()) {
                const auto movement_angle = angle(random), length = step_length(npc->kind());
                npc->move(reflect(npc->position_x() + length * std::cos(movement_angle), map_width),
                          reflect(npc->position_y() + length * std::sin(movement_angle), map_height), map_width,
                          map_height);
            }
        for (std::size_t npc_index = 0; npc_index < npcs_.size(); ++npc_index)
            for (std::size_t target_index = 0; target_index < npcs_.size(); ++target_index)
                if (npc_index != target_index && npcs_[npc_index]->alive() && npcs_[target_index]->alive() &&
                    combat::can_kill(*npcs_[npc_index], *npcs_[target_index]) &&
                    world::distance(*npcs_[npc_index], *npcs_[target_index]) <= kill_range(npcs_[npc_index]->kind()))
                    candidates.push_back({npc_index, target_index});
    }
    {
        std::lock_guard lock(queue_mutex_);
        if (queue_.empty() && !candidates.empty())
            queue_.push_back(std::move(candidates));
    }
    queue_cv_.notify_one();
}

bool Simulation::pop(std::vector<Battle> &task) {
    std::unique_lock lock(queue_mutex_);
    queue_cv_.wait_until(lock, deadline_, std::bind_front(&Simulation::battle_ready, this));
    if (finished() || queue_.empty())
        return false;
    task = std::move(queue_.front());
    queue_.pop_front();
    return true;
}

void Simulation::combat(const std::vector<Battle> &tasks, std::mt19937_64 &random) {
    std::vector<events::KillEvent> outcomes;
    {
        std::lock_guard lock(world_mutex_);
        if (finished())
            return;
        outcomes = resolve_battles(npcs_, tasks, random);
    }
    for (const auto &outcome : outcomes)
        events_.notify(outcome);
}

Simulation::Simulation(std::uint64_t seed, std::chrono::milliseconds tick)
    : seed_(seed), tick_(tick) {
    if (tick_.count() <= 0)
        throw std::invalid_argument("tick must be positive");
    std::mt19937_64 random(seed);
    std::uniform_real_distribution<double> random_x(0, map_width), random_y(0, map_height);
    std::uniform_int_distribution<int> type(0, 2);
    for (std::size_t npc_index = 0; npc_index < initial_npc_count; ++npc_index) {
        auto kind = static_cast<npc::Kind>(type(random));
        npcs_.push_back(npc::Factory::create(kind,
                                        std::string(npc::type_name(kind)) + "_" + std::to_string(npc_index),
                                        random_x(random), random_y(random)));
    }
}

void Simulation::subscribe(std::shared_ptr<events::Observer> observer) {
    if (used_)
        throw std::logic_error("subscribe before run");
    events_.subscribe(std::move(observer));
}

std::vector<Snapshot> Simulation::snapshot() const {
    std::shared_lock lock(world_mutex_);
    std::vector<Snapshot> result;
    for (const auto &npc : npcs_)
        result.push_back({npc->kind(), npc->name(), npc->position_x(), npc->position_y(), npc->alive()});
    return result;
}

void Simulation::print_map(std::ostream &out) const {
    const auto data = snapshot();
    std::vector<std::string> map(20, std::string(40, '.'));
    std::size_t alive = 0;
    for (const auto &npc : data)
        if (npc.alive) {
            ++alive;
            auto column = std::min(39, static_cast<int>(npc.position_x / map_width * 40)),
                 row = std::min(19, static_cast<int>(npc.position_y / map_height * 20));
            auto map_symbol = npc.kind == npc::Kind::Orc ? 'O' : npc.kind == npc::Kind::Druid ? 'D' : 'S';
            map[row][column] = map[row][column] == '.' ? map_symbol : '*';
        }
    std::lock_guard lock(events::output_mutex);
    out << "Map 100x100, alive=" << alive << " (O=orc, D=druid, S=squirrel, *=several)\n";
    for (const auto &row : map)
        out << row << '\n';
    out.flush();
}

void Simulation::display_loop(std::ostream &output) {
    auto next = Clock::now();
    while (!finished()) {
        print_map(output);
        next += std::chrono::seconds(1);
        std::unique_lock lock(wait_mutex_);
        wait_cv_.wait_until(lock, std::min(next, deadline_), std::bind_front(&Simulation::stop_requested, this));
    }
}

void Simulation::run(double seconds, std::ostream &output) {
    if (!std::isfinite(seconds) || seconds <= 0 || seconds > 3600)
        throw std::invalid_argument("seconds must be in (0,3600]");
    if (used_)
        throw std::logic_error("simulation can only run once");
    used_ = true;
    deadline_ = Clock::now() + std::chrono::duration_cast<Clock::duration>(
                                   std::chrono::duration<double>(seconds));
    std::thread moving, fighting;
    try {
        moving = std::thread(&Simulation::guarded, this,
                             std::bind_front(&Simulation::movement_loop, this));
        fighting = std::thread(&Simulation::guarded, this,
                               std::bind_front(&Simulation::combat_loop, this));
        display_loop(output);
    } catch (...) {
        stop();
        if (moving.joinable())
            moving.join();
        if (fighting.joinable())
            fighting.join();
        throw;
    }
    stop();
    moving.join();
    fighting.join();
    if (error_)
        std::rethrow_exception(error_);
    const auto result = snapshot();
    output << "Survivors:\n";
    for (const auto &npc : result)
        if (npc.alive)
            output << npc::type_name(npc.kind) << ' ' << npc.name << ' ' << npc.position_x << ' ' << npc.position_y
                   << '\n';
}
} // namespace game::simulation
