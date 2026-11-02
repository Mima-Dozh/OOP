#pragma once

#include <string>

namespace game::npc {
enum class Kind { Orc, Squirrel, Druid };
const char *type_name(Kind kind);
Kind parse_kind(const std::string &type_name);
} // namespace game::npc
