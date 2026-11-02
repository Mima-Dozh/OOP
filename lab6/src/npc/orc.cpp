#include "npc/orc.hpp"

namespace game::npc {
bool Orc::can_kill(Kind target) const noexcept { return target == Kind::Druid; }

Kind Orc::kind() const noexcept { return Kind::Orc; }
} // namespace game::npc
