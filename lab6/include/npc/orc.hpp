#pragma once

#include "npc/npc.hpp"

namespace game::npc {
class Orc final : public NPC {
  public:
    using NPC::NPC;
    bool can_kill(Kind target) const noexcept override;
    Kind kind() const noexcept override;
};
} // namespace game::npc
