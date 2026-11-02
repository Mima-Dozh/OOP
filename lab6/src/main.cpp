#include "npc/factory.hpp"
#include "events/observer.hpp"
#include "world/world.hpp"

#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>
int main() {
    try {
        game::world::World world;
        game::events::Events events;
        events.subscribe(std::make_shared<game::events::ConsoleObserver>());
        events.subscribe(std::make_shared<game::events::FileObserver>());
        std::cout << "Commands: add orc|squirrel|druid NAME X Y; list; save FILE; load FILE; fight "
                     "RANGE; quit\n";
        for (std::string line; std::getline(std::cin, line);) {
            auto input = std::istringstream(line);
            std::string command;
            input >> command;
            try {
                if (command == "quit")
                    break;
                if (command.empty())
                    continue;
                if (command == "add")
                    world.add(game::npc::Factory::read(input));
                else if (command == "list")
                    world.print(std::cout);
                else if (command == "fight") {
                    double range;
                    if (!(input >> range))
                        throw std::invalid_argument("missing range");
                    world.fight(range, events);
                } else if (command == "save" || command == "load") {
                    std::string path;
                    if (!(input >> std::quoted(path)))
                        throw std::invalid_argument("missing path");
                    if (command == "save") {
                        auto file = std::ofstream(path);
                        if (!file)
                            throw std::runtime_error("cannot open file");
                        world.save(file);
                    } else {
                        auto file = std::ifstream(path);
                        if (!file)
                            throw std::runtime_error("cannot open file");
                        world.load(file);
                    }
                } else
                    throw std::invalid_argument("unknown command");
            } catch (const std::exception &error) {
                std::cout << "Error: " << error.what() << '\n';
            }
        }
    } catch (const std::exception &error) {
        std::cerr << error.what() << '\n';
        return 1;
    }
}
