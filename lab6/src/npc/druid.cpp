#include "npc/druid.hpp"

namespace game::npc {
bool Druid::can_kill(Kind target) const noexcept { return target == Kind::Squirrel; }

Kind Druid::kind() const noexcept { return Kind::Druid; }
} // namespace game::npc
