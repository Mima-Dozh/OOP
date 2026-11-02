#include "npc/npc.hpp"
#include "combat/visitor.hpp"
#include <cmath>
#include <iomanip>
#include <ostream>
#include <stdexcept>
#include <utility>

namespace game::npc {
NPC::NPC(std::string name, double position_x, double position_y) : name_(std::move(name)), position_x_(position_x), position_y_(position_y) {
    if (name_.empty() || name_.find_first_of("\n\r") != std::string::npos)
        throw std::invalid_argument("name must be nonempty, single-line");
    if (!std::isfinite(position_x) || !std::isfinite(position_y) || position_x < 0 || position_x > 500 || position_y < 0 || position_y > 500)
        throw std::invalid_argument("coordinates outside [0,500]");
}

bool NPC::accept(const combat::Visitor &visitor) const { return visitor.visit(*this); }

bool NPC::can_kill(Kind) const {
    throw std::logic_error("NPC::can_kill is not implemented");
}

const std::string &NPC::name() const noexcept { return name_; }

double NPC::position_x() const noexcept { return position_x_; }

double NPC::position_y() const noexcept { return position_y_; }

bool NPC::alive() const noexcept { return alive_; }

void NPC::kill() noexcept { alive_ = false; }

void NPC::move(double position_x, double position_y, double width, double height) {
    if (!alive_)
        return;
    if (!std::isfinite(position_x) || !std::isfinite(position_y) || position_x < 0 || position_x > width || position_y < 0 || position_y > height)
        throw std::invalid_argument("movement outside map");
    position_x_ = position_x;
    position_y_ = position_y;
}

std::ostream &operator<<(std::ostream &out, const NPC &npc) {
    return out << type_name(npc.kind()) << ' ' << std::quoted(npc.name()) << ' ' << npc.position_x_
               << ' ' << npc.position_y_;
}

NPC::~NPC() = default;
} // namespace game::npc
