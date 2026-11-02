#pragma once

#include "npc/npc.hpp"

#include <iosfwd>
#include <memory>
#include <string>

namespace game::npc {
class Factory {
  public:
    static std::shared_ptr<NPC> create(Kind kind, const std::string &name, double position_x, double position_y);
    static std::shared_ptr<NPC> read(std::istream &in);
};
} // namespace game::npc
