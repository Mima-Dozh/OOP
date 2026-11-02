#include "npc/factory.hpp"
#include "npc/druid.hpp"
#include "npc/orc.hpp"
#include "npc/squirrel.hpp"

#include <iomanip>
#include <istream>
#include <stdexcept>

namespace game::npc {
std::shared_ptr<NPC> Factory::create(Kind kind, const std::string &name, double position_x, double position_y) {
    switch (kind) {
    case Kind::Orc:
        return std::make_shared<Orc>(name, position_x, position_y);
    case Kind::Squirrel:
        return std::make_shared<Squirrel>(name, position_x, position_y);
    case Kind::Druid:
        return std::make_shared<Druid>(name, position_x, position_y);
    }
    throw std::invalid_argument("invalid type");
}

std::shared_ptr<NPC> Factory::read(std::istream &in) {
    std::string type, name;
    double position_x, position_y;
    if (!(in >> type >> std::quoted(name) >> position_x >> position_y))
        throw std::invalid_argument("invalid NPC record");
    return create(parse_kind(type), name, position_x, position_y);
}
} // namespace game::npc
