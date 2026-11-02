#include "combat/visitor.hpp"
#include "npc/npc.hpp"

namespace game::combat {
bool can_kill(const npc::NPC &attacker, const npc::NPC &target) {
    return target.accept(CombatVisitor(attacker));
}

CombatVisitor::CombatVisitor(const npc::NPC &attacker) : attacker_(attacker) {}

bool CombatVisitor::visit(const npc::NPC &target) const {
    return attacker_.can_kill(target.kind());
}

Visitor::~Visitor() = default;
} // namespace game::combat
