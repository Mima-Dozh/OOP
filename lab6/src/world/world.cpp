#include "world/world.hpp"
#include "npc/factory.hpp"
#include "events/observer.hpp"
#include "combat/visitor.hpp"

#include <cmath>
#include <iomanip>
#include <istream>
#include <ostream>
#include <stdexcept>
#include <utility>

namespace game::world {
double distance(const npc::NPC &first_npc, const npc::NPC &second_npc) {
    return std::hypot(first_npc.position_x() - second_npc.position_x(), first_npc.position_y() - second_npc.position_y());
}

const std::vector<std::shared_ptr<npc::NPC>> &World::npcs() const noexcept { return npcs_; }

void World::add(std::shared_ptr<npc::NPC> npc) {
    if (!npc)
        throw std::invalid_argument("null NPC");
    for (const auto &item : npcs_)
        if (item->name() == npc->name())
            throw std::invalid_argument("duplicate NPC name");
    npcs_.push_back(std::move(npc));
}

void World::print(std::ostream &out) const {
    for (const auto &npc : npcs_)
        out << *npc << '\n';
}

void World::save(std::ostream &out) const {
    out << "BF3 1\n" << npcs_.size() << '\n' << std::setprecision(17);
    for (const auto &npc : npcs_)
        out << *npc << '\n';
    if (!out)
        throw std::runtime_error("save failed");
}

void World::load(std::istream &in) {
    std::string magic;
    int version;
    std::size_t count;
    if (!(in >> magic >> version >> count) || magic != "BF3" || version != 1 || count > 100000)
        throw std::invalid_argument("invalid file header");
    World replacement;
    for (std::size_t npc_index = 0; npc_index < count; ++npc_index)
        replacement.add(npc::Factory::read(in));
    in >> std::ws;
    if (!in.eof())
        throw std::invalid_argument("trailing data in file");
    npcs_.swap(replacement.npcs_);
}

void World::fight(double range, const events::Events &events) {
    if (!std::isfinite(range) || range < 0)
        throw std::invalid_argument("invalid battle distance");
    std::vector<bool> defeated(npcs_.size(), false);
    std::vector<events::KillEvent> outcomes;
    for (std::size_t npc_index = 0; npc_index < npcs_.size(); ++npc_index)
        for (std::size_t other_index = npc_index + 1; other_index < npcs_.size(); ++other_index) {
            auto &first_npc = *npcs_[npc_index];
            auto &second_npc = *npcs_[other_index];
            if (!first_npc.alive() || !second_npc.alive() || distance(first_npc, second_npc) > range)
                continue;
            if (combat::can_kill(first_npc, second_npc)) {
                defeated[other_index] = true;
                outcomes.push_back(events::event(first_npc, second_npc));
            }
            if (combat::can_kill(second_npc, first_npc)) {
                defeated[npc_index] = true;
                outcomes.push_back(events::event(second_npc, first_npc));
            }
        }
    // Apply deaths only after every participant has attacked every possible target.
    for (std::size_t npc_index = 0; npc_index < npcs_.size(); ++npc_index)
        if (defeated[npc_index])
            npcs_[npc_index]->kill();
    std::erase_if(npcs_, [](const auto &npc) { return !npc->alive(); });
    for (const auto &outcome : outcomes)
        events.notify(outcome);
}
} // namespace game::world
