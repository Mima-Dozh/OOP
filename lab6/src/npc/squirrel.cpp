#include "npc/squirrel.hpp"

namespace game::npc {
bool Squirrel::can_kill(Kind) const noexcept { return false; }

Kind Squirrel::kind() const noexcept { return Kind::Squirrel; }
} // namespace game::npc
