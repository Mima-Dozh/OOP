#include "npc/kind.hpp"
#include <stdexcept>

namespace game::npc {
const char *type_name(Kind kind) {
    switch (kind) {
    case Kind::Orc:
        return "orc";
    case Kind::Squirrel:
        return "squirrel";
    case Kind::Druid:
        return "druid";
    }
    throw std::invalid_argument("invalid NPC kind");
}

Kind parse_kind(const std::string &type_name) {
    if (type_name == "orc")
        return Kind::Orc;
    if (type_name == "squirrel")
        return Kind::Squirrel;
    if (type_name == "druid")
        return Kind::Druid;
    throw std::invalid_argument("unknown NPC type");
}
} // namespace game::npc
