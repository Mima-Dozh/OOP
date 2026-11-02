#pragma once

#include "npc/kind.hpp"

#include <iosfwd>
#include <string>

namespace game::combat {
class Visitor;
}

namespace game::npc {
class NPC {
    std::string name_;
    double position_x_, position_y_;
    bool alive_ = true;

  public:
    NPC(std::string name, double position_x, double position_y);
    virtual ~NPC();
    virtual bool can_kill(Kind target) const;
    virtual Kind kind() const noexcept = 0;
    virtual bool accept(const combat::Visitor &visitor) const;
    const std::string &name() const noexcept;
    double position_x() const noexcept;
    double position_y() const noexcept;
    bool alive() const noexcept;
    void kill() noexcept;
    void move(double position_x, double position_y, double width = 500, double height = 500);
    friend std::ostream &operator<<(std::ostream &out, const NPC &npc);
};
} // namespace game::npc
