#include "simulation/simulation.hpp"
int main(int argc, char **argv) {
    try {
        auto seconds = 30.0;
        std::uint64_t seed = std::random_device{}();
        for (auto argument_index = 1; argument_index < argc; ++argument_index) {
            const auto arg = std::string(argv[argument_index]);
            if (arg == "--seconds" && argument_index + 1 < argc)
                seconds = std::stod(argv[++argument_index]);
            else if (arg == "--seed" && argument_index + 1 < argc)
                seed = std::stoull(argv[++argument_index]);
            else
                throw std::invalid_argument("Usage: lab7 [--seconds 30] [--seed N]");
        }
        game::simulation::Simulation simulation(seed);
        simulation.subscribe(std::make_shared<game::events::ConsoleObserver>());
        simulation.subscribe(std::make_shared<game::events::FileObserver>());
        simulation.run(seconds);
    } catch (const std::exception &error) {
        std::lock_guard lock(game::events::output_mutex);
        std::cerr << error.what() << '\n';
        return 1;
    }
}
