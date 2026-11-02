#include "npc/factory.hpp"
#include "events/observer.hpp"
#include "combat/visitor.hpp"
#include "world/world.hpp"
#include <algorithm>
#include <array>
#include <sstream>
#include <stdexcept>
#include <gtest/gtest.h>
struct Recorder : game::events::Observer {
    std::vector<game::events::KillEvent> events;
    void on_kill(const game::events::KillEvent &kill_event) override { events.push_back(kill_event); }
};
TEST(Factory, TypesAndBounds) {
    for (auto kind : {game::npc::Kind::Orc, game::npc::Kind::Squirrel, game::npc::Kind::Druid}) {
        auto npc = game::npc::Factory::create(kind, "Name", 0, 500);
        EXPECT_EQ(npc->kind(), kind);
    }
    EXPECT_THROW(game::npc::Factory::create(game::npc::Kind::Orc, "", 0, 0), std::invalid_argument);
    EXPECT_THROW(game::npc::Factory::create(game::npc::Kind::Orc, "x", -1, 0), std::invalid_argument);
    EXPECT_THROW(game::npc::parse_kind("dragon"), std::invalid_argument);
}
TEST(Visitor, AllNinePairs) {
    std::vector<std::shared_ptr<game::npc::NPC>> participants;
    for (auto kind : {game::npc::Kind::Orc, game::npc::Kind::Squirrel, game::npc::Kind::Druid})
        participants.push_back(game::npc::Factory::create(kind, "Name", 0, 0));
    for (int attacker_index = 0; attacker_index < 3; ++attacker_index)
        for (int target_index = 0; target_index < 3; ++target_index)
            EXPECT_EQ(game::combat::can_kill(*participants[attacker_index], *participants[target_index]), (attacker_index == 0 && target_index == 2) || (attacker_index == 2 && target_index == 1));
}
TEST(World, UniqueNames) {
    game::world::World world;
    world.add(game::npc::Factory::create(game::npc::Kind::Orc, "same", 0, 0));
    EXPECT_THROW(world.add(game::npc::Factory::create(game::npc::Kind::Squirrel, "same", 1, 1)),
                 std::invalid_argument);
    EXPECT_EQ(world.npcs().size(), 1);
}
TEST(World, SaveLoadAndTransactionalFailure) {
    game::world::World world;
    world.add(game::npc::Factory::create(game::npc::Kind::Orc, "A quoted \" name", 0.125, 500));
    std::stringstream stream;
    world.save(stream);
    game::world::World loaded;
    loaded.load(stream);
    EXPECT_EQ(loaded.npcs()[0]->name(), world.npcs()[0]->name());
    EXPECT_EQ(loaded.npcs()[0]->position_x(), 0.125);
    std::istringstream bad("BF3 1\n2\norc A 0 0\nsquirrel A 1 1\n");
    EXPECT_THROW(loaded.load(bad), std::invalid_argument);
    EXPECT_EQ(loaded.npcs().size(), 1);
}
TEST(World, FightRangeAndObservers) {
    game::world::World world;
    auto orc = game::npc::Factory::create(game::npc::Kind::Orc, "o", 0, 0),
         druid = game::npc::Factory::create(game::npc::Kind::Druid, "d", 3, 4);
    world.add(orc);
    world.add(druid);
    game::events::Events notifications;
    auto recorder = std::make_shared<Recorder>();
    notifications.subscribe(recorder);
    world.fight(4.99, notifications);
    EXPECT_EQ(world.npcs().size(), 2);
    world.fight(5, notifications);
    EXPECT_EQ(world.npcs().size(), 1);
    ASSERT_EQ(recorder->events.size(), 1);
    EXPECT_EQ(recorder->events[0].victim, "d");
    EXPECT_FALSE(druid->alive());
    druid->move(20, 20);
    EXPECT_EQ(druid->position_x(), 3);
}
TEST(World, DruidKillsSquirrelAndDeadCannotFight) {
    game::world::World world;
    world.add(game::npc::Factory::create(game::npc::Kind::Druid, "d", 0, 0));
    world.add(game::npc::Factory::create(game::npc::Kind::Squirrel, "s", 0, 0));
    game::events::Events notifications;
    world.fight(0, notifications);
    EXPECT_EQ(world.npcs().size(), 1);
    EXPECT_EQ(world.npcs()[0]->kind(), game::npc::Kind::Druid);
    EXPECT_THROW(world.fight(-1, notifications), std::invalid_argument);
}

TEST(World, AllParticipantsAttackBeforeDeathsRegardlessOfOrder) {
    std::array<int, 4> order{0, 1, 2, 3};
    do {
        game::world::World world;
        auto orc = game::npc::Factory::create(game::npc::Kind::Orc, "orc", 0, 0);
        auto druid = game::npc::Factory::create(game::npc::Kind::Druid, "druid", 0, 0);
        auto squirrel1 = game::npc::Factory::create(game::npc::Kind::Squirrel, "s1", 0, 0);
        auto squirrel2 = game::npc::Factory::create(game::npc::Kind::Squirrel, "s2", 0, 0);
        std::array participants{orc, druid, squirrel1, squirrel2};
        for (int index : order)
            world.add(participants[index]);
        game::events::Events events;
        auto recorder = std::make_shared<Recorder>();
        events.subscribe(recorder);

        world.fight(0, events);

        ASSERT_EQ(world.npcs().size(), 1);
        EXPECT_EQ(world.npcs()[0], orc);
        EXPECT_FALSE(druid->alive());
        EXPECT_FALSE(squirrel1->alive());
        EXPECT_FALSE(squirrel2->alive());
        EXPECT_EQ(recorder->events.size(), 3);
        world.fight(0, events);
        EXPECT_EQ(recorder->events.size(), 3);
    } while (std::next_permutation(order.begin(), order.end()));
}

TEST(World, EveryAttackerCanAttackTheSameTarget) {
    game::world::World world;
    world.add(game::npc::Factory::create(game::npc::Kind::Orc, "o1", 0, 0));
    world.add(game::npc::Factory::create(game::npc::Kind::Orc, "o2", 0, 0));
    world.add(game::npc::Factory::create(game::npc::Kind::Druid, "d", 0, 0));
    game::events::Events events;
    auto recorder = std::make_shared<Recorder>();
    events.subscribe(recorder);

    world.fight(0, events);

    EXPECT_EQ(world.npcs().size(), 2);
    ASSERT_EQ(recorder->events.size(), 2);
    EXPECT_EQ(recorder->events[0].attacker, "o1");
    EXPECT_EQ(recorder->events[1].attacker, "o2");
    EXPECT_EQ(recorder->events[0].victim, "d");
    EXPECT_EQ(recorder->events[1].victim, "d");
}

namespace {
class NPCWithoutCombatRules final : public game::npc::NPC {
  public:
    using NPC::NPC;
    game::npc::Kind kind() const noexcept override { return game::npc::Kind::Orc; }
};
} // namespace

TEST(Visitor, MissingCombatRulesThrow) {
    NPCWithoutCombatRules attacker("unimplemented", 0, 0);
    const game::npc::NPC &base = attacker;
    EXPECT_THROW(base.can_kill(game::npc::Kind::Druid), std::logic_error);
    auto target = game::npc::Factory::create(game::npc::Kind::Druid, "target", 0, 0);
    EXPECT_THROW(game::combat::can_kill(attacker, *target), std::logic_error);
}
