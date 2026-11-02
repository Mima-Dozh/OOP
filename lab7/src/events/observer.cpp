#include "events/observer.hpp"
#include "npc/npc.hpp"

#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <utility>

namespace game::events {
std::mutex output_mutex;

KillEvent event(const npc::NPC &attacker, const npc::NPC &victim) {
    return {attacker.name(), victim.name(), attacker.kind(), victim.kind()};
}

std::ostream &operator<<(std::ostream &out, const KillEvent &kill_event) {
    return out << npc::type_name(kill_event.attacker_kind) << ' ' << std::quoted(kill_event.attacker) << " killed "
               << npc::type_name(kill_event.victim_kind) << ' ' << std::quoted(kill_event.victim);
}

void ConsoleObserver::on_kill(const KillEvent &kill_event) {
    std::lock_guard lock(output_mutex);
    std::cout << kill_event << '\n';
}

FileObserver::FileObserver(const std::string &filename) : file_(filename, std::ios::app) {
    if (!file_)
        throw std::runtime_error("cannot open log");
}

void FileObserver::on_kill(const KillEvent &kill_event) {
    file_ << kill_event << '\n';
    file_.flush();
    if (!file_)
        throw std::runtime_error("cannot write log");
}

void Events::subscribe(std::shared_ptr<Observer> observer) {
    if (!observer)
        throw std::invalid_argument("null observer");
    observers_.push_back(std::move(observer));
}

void Events::notify(const KillEvent &kill_event) const {
    for (const auto &observer : observers_)
        observer->on_kill(kill_event);
}

Observer::~Observer() = default;
} // namespace game::events
