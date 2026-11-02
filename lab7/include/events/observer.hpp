#pragma once

#include "npc/kind.hpp"

#include <fstream>
#include <iosfwd>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

namespace game::npc {
class NPC;
}

namespace game::events {
extern std::mutex output_mutex;
struct KillEvent {
    std::string attacker, victim;
    npc::Kind attacker_kind, victim_kind;
};
KillEvent event(const npc::NPC &attacker, const npc::NPC &victim);
std::ostream &operator<<(std::ostream &out, const KillEvent &kill_event);

class Observer {
  public:
    virtual ~Observer();
    virtual void on_kill(const KillEvent &) = 0;
};

class ConsoleObserver final : public Observer {
  public:
    void on_kill(const KillEvent &kill_event) override;
};

class FileObserver final : public Observer {
    std::ofstream file_;

  public:
    explicit FileObserver(const std::string &filename = "log.txt");
    void on_kill(const KillEvent &kill_event) override;
};

class Events {
    std::vector<std::shared_ptr<Observer>> observers_;

  public:
    void subscribe(std::shared_ptr<Observer> observer);
    void notify(const KillEvent &kill_event) const;
};
} // namespace game::events
