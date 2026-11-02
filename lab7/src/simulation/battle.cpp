#include "simulation/battle.hpp"
#include "combat/visitor.hpp"
#include "world/world.hpp"

#include <stdexcept>

namespace game::simulation {
double kill_range(npc::Kind kind) { return kind == npc::Kind::Squirrel ? 5 : 10; }

bool winning_roll(int attack, int defense) {
    if (attack < 1 || attack > 6 || defense < 1 || defense > 6)
        throw std::invalid_argument("dice values must be in [1,6]");
    return attack > defense;
}

std::vector<events::KillEvent> resolve_battles(
    const std::vector<std::shared_ptr<npc::NPC>> &npcs,
    const std::vector<Battle> &tasks, std::mt19937_64 &random) {
    std::vector<events::KillEvent> outcomes;
    std::vector<bool> defeated(npcs.size(), false);
    std::uniform_int_distribution<int> dice(1, 6);
    for (const auto &task : tasks) {
        auto &attacker = *npcs.at(task.attacker);
        auto &target = *npcs.at(task.target);
        if (task.attacker == task.target || !attacker.alive() || !target.alive() ||
            !combat::can_kill(attacker, target) ||
            world::distance(attacker, target) > kill_range(attacker.kind()))
            continue;
        const auto attack = dice(random), defense = dice(random);
        if (winning_roll(attack, defense)) {
            defeated[task.target] = true;
            outcomes.push_back(events::event(attacker, target));
        }
    }
    for (std::size_t npc_index = 0; npc_index < npcs.size(); ++npc_index)
        if (defeated[npc_index])
            npcs[npc_index]->kill();
    return outcomes;
}
} // namespace game::simulation
