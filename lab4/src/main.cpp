#include "array.hpp"
#include "figure.hpp"

#include <iostream>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>

int main() {
    FigureArray<std::shared_ptr<Figure<double>>> figures;
    std::cout << "Commands: add triangle|square|rectangle x1 y1 ...; list; total; erase INDEX; "
                 "demo; quit\n";
    for (std::string line; std::getline(std::cin, line);) {
        std::istringstream in(line);
        std::string command;
        in >> command;
        try {
            if (command == "quit")
                break;
            if (command.empty())
                continue;
            if (command == "add") {
                std::string type;
                in >> type;
                std::shared_ptr<Figure<double>> figure;
                if (type == "triangle")
                    figure = std::make_shared<Triangle<double>>();
                else if (type == "square")
                    figure = std::make_shared<Square<double>>();
                else if (type == "rectangle")
                    figure = std::make_shared<Rectangle<double>>();
                else
                    throw std::invalid_argument("unknown figure");
                if (!(in >> *figure))
                    throw std::invalid_argument("invalid coordinates or figure shape");
                in >> std::ws;
                if (!in.eof())
                    throw std::invalid_argument("extra input");
                figures.push_back(std::move(figure));
                std::cout << "Added\n";
            } else if (command == "list") {
                for (std::size_t i = 0; i < figures.size(); ++i) {
                    const auto &f = *figures[i];
                    const auto c = f.center();
                    std::cout << i << ": " << f << " center=(" << c.x << ", " << c.y
                              << ") area=" << static_cast<double>(f) << '\n';
                }
            } else if (command == "total") {
                auto sum = 0.0;
                for (std::size_t i = 0; i < figures.size(); ++i)
                    sum += static_cast<double>(*figures[i]);
                std::cout << "Total area: " << sum << '\n';
            } else if (command == "erase") {
                std::size_t index;
                if (!(in >> index))
                    throw std::invalid_argument("missing index");
                if (index >= figures.size())
                    throw std::out_of_range("index");
                figures.erase(index);
                std::cout << "Erased\n";
            } else if (command == "demo") {
                FigureArray<Square<int>> values;
                values.push_back(Square<int>{});
                values.push_back(Square<int>{});
                auto copy = values;
                auto moved = std::move(copy);
                moved.erase(0);
                std::cout << "FigureArray<Square<int>>: " << moved[0]
                          << " area=" << static_cast<double>(moved[0]) << '\n';
                figures.push_back(std::make_shared<Triangle<double>>());
                figures.push_back(std::make_shared<Square<double>>());
                figures.push_back(std::make_shared<Rectangle<double>>());
            } else
                throw std::invalid_argument("unknown command");
        } catch (const std::exception &e) {
            std::cout << "Error: " << e.what() << '\n';
        }
    }
}
