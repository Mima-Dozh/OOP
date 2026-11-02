#include "figure.hpp"
#include <iostream>
#include <sstream>
#include <string>
#include <stdexcept>


Figure *read_figure(std::istream &in) {
    std::string type;
    in >> type;
    Figure *figure = nullptr;
    if (type == "triangle")
        figure = new Triangle;
    else if (type == "square")
        figure = new Square;
    else if (type == "rectangle")
        figure = new Rectangle;
    else
        throw std::invalid_argument("unknown figure");
    try {
        if (!(in >> *figure))
            throw std::invalid_argument("invalid coordinates or figure shape");
        in >> std::ws;
        if (!in.eof())
            throw std::invalid_argument("extra input");
    } catch (...) {
        delete figure;
        throw;
    }
    return figure;
}
void clear_figures(FigureArray &figures) noexcept {
    for (std::size_t i = 0; i < figures.size(); ++i)
        delete figures[i];
    figures.clear();
}

void demo(FigureArray &figures) {
    for (auto kind : {FigureKind::Triangle, FigureKind::Square,
                      FigureKind::Rectangle}) {
        Figure *figure = nullptr;
        if (kind == FigureKind::Triangle)
            figure = new Triangle;
        else if (kind == FigureKind::Square)
            figure = new Square;
        else
            figure = new Rectangle;
        try {
            figures.push_back(figure);
        } catch (...) {
            delete figure;
            throw;
        }
    }
}

int main() {
    FigureArray figures;
    try {
        std::cout << "Commands: add triangle|square|rectangle x1 y1 ...; list; total; erase INDEX; "
                     "demo; quit\n";
        for (std::string line; std::getline(std::cin, line);) {
            auto in = std::istringstream(line);
            std::string command;
            in >> command;
            try {
                if (command == "quit")
                    break;
                if (command.empty())
                    continue;
                if (command == "add") {
                    auto *figure = read_figure(in);
                    try {
                        figures.push_back(figure);
                    } catch (...) {
                        delete figure;
                        throw;
                    }
                    std::cout << "Added\n";
                } else if (command == "list") {
                    for (std::size_t i = 0; i < figures.size(); ++i) {
                        const auto &f = *figures[i];
                        const auto c = f.center();
                        std::cout << i << ": " << f << " center=(" << c.x << ", " << c.y
                                  << ") area=" << static_cast<double>(f) << '\n';
                    }
                } else if (command == "total") {
                    std::cout << "Total area: " << total_area(figures) << '\n';
                } else if (command == "erase") {
                    std::size_t index;
                    if (!(in >> index))
                        throw std::invalid_argument("missing index");
                    erase_figure(figures, index);
                    std::cout << "Erased\n";
                } else if (command == "demo") {
                    demo(figures);
                } else
                    throw std::invalid_argument("unknown command");
            } catch (const std::exception &e) {
                std::cout << "Error: " << e.what() << '\n';
            }
        }
    } catch (...) {
        clear_figures(figures);
        throw;
    }
    clear_figures(figures);

    return 0;
}
