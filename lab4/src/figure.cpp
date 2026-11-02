#include "figure.hpp"

#include <algorithm>
#include <cmath>
#include <stdexcept>

void validate_vertices(const std::vector<geometry::Point> &points, geometry::FigureKind kind) {
    if (kind != geometry::FigureKind::Triangle && kind != geometry::FigureKind::Square &&
        kind != geometry::FigureKind::Rectangle)
        throw std::invalid_argument("unknown figure kind");
    const auto count = kind == geometry::FigureKind::Triangle ? 3U : 4U;
    if (points.size() != count)
        throw std::invalid_argument("wrong vertex count");
    for (const auto &point : points)
        if (!std::isfinite(point.x) || !std::isfinite(point.y))
            throw std::invalid_argument("nonfinite coordinate");

    constexpr auto tolerance = 1e-9L;
    auto directions = std::vector<geometry::Point>(count);
    auto lengths = std::vector<long double>(count);
    for (auto i = 0U; i < count; ++i) {
        const auto &a = points[i];
        const auto &b = points[(i + 1) % count];
        const auto dx = b.x - a.x, dy = b.y - a.y;
        lengths[i] = std::hypot(dx, dy);
        if (!std::isfinite(lengths[i]) || lengths[i] == 0)
            throw std::invalid_argument("invalid or zero-length side");
        directions[i] = {dx / lengths[i], dy / lengths[i]};
    }
    auto orientation = 0.0L;
    for (auto i = 0U; i < count; ++i) {
        const auto &a = directions[i];
        const auto &b = directions[(i + 1) % count];
        const auto turn = a.x * b.y - a.y * b.x;
        if (std::abs(turn) <= tolerance || (i != 0 && turn * orientation <= 0))
            throw std::invalid_argument("vertices must form a convex figure in boundary order");
        orientation = turn;
    }
}

void validate(const std::vector<geometry::Point> &points, geometry::FigureKind kind) {
    validate_vertices(points, kind);
    if (kind != geometry::FigureKind::Triangle) {
        for (auto i = 0U; i < 4; ++i) {
            const auto &a = points[i];
            const auto &b = points[(i + 1) % 4];
            const auto &c = points[(i + 2) % 4];
            const auto dx1 = a.x - b.x, dy1 = a.y - b.y;
            const auto dx2 = c.x - b.x, dy2 = c.y - b.y;
            const auto first = std::hypot(dx1, dy1), second = std::hypot(dx2, dy2);
            const auto dot = (dx1 / first) * (dx2 / second) + (dy1 / first) * (dy2 / second);
            if (std::abs(dot) > 1e-9L)
                throw std::invalid_argument("rectangle must have right angles");
            if (kind == geometry::FigureKind::Square) {
                const auto scale = std::max(first, second);
                if (std::abs(first / scale - second / scale) > 1e-9L)
                    throw std::invalid_argument("square must have equal sides");
            }
        }
    }
    if (kind == geometry::FigureKind::Triangle) {
        const auto a = std::hypot(points[1].x - points[0].x, points[1].y - points[0].y);
        const auto b = std::hypot(points[2].x - points[1].x, points[2].y - points[1].y);
        const auto c = std::hypot(points[0].x - points[2].x, points[0].y - points[2].y);
        const auto equal = [](auto first, auto second) {
            const auto scale = std::max(first, second);
            return std::abs(first / scale - second / scale) <= 1e-9L;
        };
        if (!equal(a, b) && !equal(b, c) && !equal(a, c))
            throw std::invalid_argument("triangle must be isosceles");
    }
}

double area(const std::vector<geometry::Point> &points, geometry::FigureKind kind) {
    if (points.empty())
        return 0;
    switch (kind) {
    case geometry::FigureKind::Triangle:
        return static_cast<double>(std::abs(geometry::cross(points.at(0), points.at(1),
                                                            points.at(2))) / 2);
    case geometry::FigureKind::Square:
        return static_cast<double>(geometry::distance2(points.at(0), points.at(1)));
    case geometry::FigureKind::Rectangle:
        return static_cast<double>(std::sqrt(geometry::distance2(points.at(0), points.at(1)) *
                                              geometry::distance2(points.at(1), points.at(2))));
    }
    throw std::invalid_argument("unknown figure kind");
}
