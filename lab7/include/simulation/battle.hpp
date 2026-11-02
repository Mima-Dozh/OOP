#pragma once

#include "npc/npc.hpp"
#include "events/observer.hpp"

#include <cstddef>
#include <memory>
#include <random>
#include <vector>

namespace game::simulation {
struct Battle {
    std::size_t attacker, target;
};

double kill_range(npc::Kind kind);
bool winning_roll(int attack, int defense);
// Caller holds the world lock. All attacks are resolved before applying deaths.
std::vector<events::KillEvent> resolve_battles(
    const std::vector<std::shared_ptr<npc::NPC>> &npcs,
    const std::vector<Battle> &tasks, std::mt19937_64 &random);
} // namespace game::simulation
