#pragma once
#include "events/observer.hpp"
#include "simulation/battle.hpp"

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <cstdint>
#include <deque>
#include <exception>
#include <functional>
#include <iostream>
#include <memory>
#include <mutex>
#include <random>
#include <shared_mutex>
#include <string>
#include <vector>

namespace game::simulation {
inline constexpr double map_width = 100, map_height = 100;
inline constexpr std::size_t initial_npc_count = 50;
double step_length(npc::Kind kind);
double reflect(double value, double limit);
struct Snapshot {
    npc::Kind kind;
    std::string name;
    double position_x, position_y;
    bool alive;
};
class Simulation {
    using Clock = std::chrono::steady_clock;
    std::vector<std::shared_ptr<npc::NPC>> npcs_;
    mutable std::shared_mutex world_mutex_;
    std::mutex queue_mutex_, wait_mutex_, error_mutex_;
    std::condition_variable queue_cv_, wait_cv_;
    std::deque<std::vector<Battle>> queue_;
    std::atomic<bool> stop_{false};
    std::exception_ptr error_;
    events::Events events_;
    std::uint64_t seed_;
    std::chrono::milliseconds tick_;
    bool used_ = false;
    Clock::time_point deadline_;
    bool finished() const;
    bool stop_requested() const;
    // Called with queue_mutex_ locked.
    bool battle_ready() const;
    void movement_loop();
    void combat_loop();
    void stop();
    void guarded(const std::function<void()> &work);
    void pause();
    void display_loop(std::ostream &output);
    void movement(std::mt19937_64 &random);
    bool pop(std::vector<Battle> &task);
    void combat(const std::vector<Battle> &tasks, std::mt19937_64 &random);

  public:
    explicit Simulation(std::uint64_t seed = std::random_device{}(),
                        std::chrono::milliseconds tick = std::chrono::milliseconds(200));
    void subscribe(std::shared_ptr<events::Observer> observer);
    std::vector<Snapshot> snapshot() const;
    void print_map(std::ostream &out) const;
    void run(double seconds = 30, std::ostream &output = std::cout);
};
} // namespace game::simulation
