#pragma once

#include "npc/npc.hpp"

#include <iosfwd>
#include <memory>
#include <vector>

namespace game::events {
class Events;
}

namespace game::world {
double distance(const npc::NPC &first_npc, const npc::NPC &second_npc);

class World {
    std::vector<std::shared_ptr<npc::NPC>> npcs_;

  public:
    const std::vector<std::shared_ptr<npc::NPC>> &npcs() const noexcept;
    void add(std::shared_ptr<npc::NPC> npc);
    void print(std::ostream &out) const;
    void save(std::ostream &out) const;
    void load(std::istream &in);
    void fight(double range, const events::Events &events);
};
} // namespace game::world
