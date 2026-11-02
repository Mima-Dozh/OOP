#pragma once

namespace game::npc {
class NPC;
}

namespace game::combat {
class Visitor {
  public:
    virtual ~Visitor();
    virtual bool visit(const npc::NPC &target) const = 0;
};

class CombatVisitor final : public Visitor {
    const npc::NPC &attacker_;

  public:
    explicit CombatVisitor(const npc::NPC &attacker);
    bool visit(const npc::NPC &target) const override;
};

bool can_kill(const npc::NPC &attacker, const npc::NPC &target);
} // namespace game::combat
