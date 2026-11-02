#include "figure.hpp"
#include <cmath>
#include <array>
#include <algorithm>
#include <stdexcept>
#include <istream>
#include <ostream>
#include <utility>


bool operator==(const Figure &a, const Figure &b) { return a.equals(b); }

std::ostream &operator<<(std::ostream &out, const Figure &f) {
    out << (f.kind_ == FigureKind::Triangle ? "Triangle" : f.kind_ == FigureKind::Square ? "Square" : "Rectangle");
    for (auto p : f.points_)
        out << " (" << p.x << ", " << p.y << ')';
    return out;
}

std::istream &operator>>(std::istream &in, Figure &f) {
    auto temp = std::vector<Point>(f.kind_ == FigureKind::Triangle ? 3 : 4);
    for (auto &p : temp)
        if (!(in >> p.x >> p.y))
            return in;
    try {
        f.validate(temp);
        f.points_ = std::move(temp);
    } catch (const std::invalid_argument &) {
        in.setstate(std::ios::failbit);
    }
    return in;
}

Figure::Figure(std::vector<Point> p, FigureKind kind) : points_(std::move(p)), kind_(kind) {}

Figure &Figure::operator=(const Figure &other) {
    if (this != &other) {
        validate(other.points_);
        points_ = other.points_;
    }
    return *this;
}

Figure &Figure::operator=(Figure &&other) {
    if (this != &other) {
        validate(other.points_);
        points_ = std::move(other.points_);
    }
    return *this;
}

Point Figure::center() const { return geometry::center(points_); }

Figure::operator double() const { return area(); }

double Figure::area() const {
    throw std::logic_error("Not implemented");
}

bool Figure::equals(const Figure &f) const {
    return kind_ == f.kind_ && geometry::equal(points_, f.points_);
}

Triangle::Triangle() : Triangle({{0, 0}, {2, 0}, {1, std::sqrt(3.L)}}) {}

Triangle::Triangle(std::vector<Point> p) : Figure(std::move(p), FigureKind::Triangle) {
    validate(points_);
}

Triangle::Triangle(const Triangle &other) : Figure(other) {}

Square::Square() : Square({{0, 0}, {1, 0}, {1, 1}, {0, 1}}) {}

Square::Square(std::vector<Point> p) : Rectangle(std::move(p), FigureKind::Square) {
    validate(points_);
}

Square::Square(const Square &other) : Rectangle(other) {}

Rectangle::Rectangle() : Rectangle({{0, 0}, {2, 0}, {2, 1}, {0, 1}}) {}

Rectangle::Rectangle(std::vector<Point> p) : Rectangle(std::move(p), FigureKind::Rectangle) {}

Rectangle::Rectangle(std::vector<Point> p, FigureKind kind) : Figure(std::move(p), kind) {
    validate(points_);
}

Rectangle::Rectangle(const Rectangle &other) : Figure(other) {}


double total_area(const FigureArray &figures) {
    auto sum = 0.0;
    for (std::size_t i = 0; i < figures.size(); ++i)
        sum += static_cast<double>(*figures[i]);
    return sum;
}

void erase_figure(FigureArray &figures, std::size_t index) {
    if (index >= figures.size())
        throw std::out_of_range("index");

    delete figures[index];
    figures.erase(index);
}

double Triangle::area() const {
    const auto side_squared = geometry::distance2(points_.at(0), points_.at(1));
    return static_cast<double>(std::sqrt(3.L) * side_squared / 4);
}

double Square::area() const {
    return static_cast<double>(geometry::distance2(points_.at(0), points_.at(1)));
}

double Rectangle::area() const {
    const auto width_squared = geometry::distance2(points_.at(0), points_.at(1));
    const auto height_squared = geometry::distance2(points_.at(1), points_.at(2));
    return static_cast<double>(std::sqrt(width_squared * height_squared));
}

void Figure::validate(const std::vector<Point> &) const {
    throw std::logic_error("Not implemented");
}

void Triangle::validate(const std::vector<Point> &points) const {
    validate_vertices(points);
    const auto side = std::hypot(points[1].x - points[0].x, points[1].y - points[0].y);
    for (std::size_t i = 1; i < 3; ++i) {
        const auto &a = points[i], &b = points[(i + 1) % 3];
        const auto length = std::hypot(b.x - a.x, b.y - a.y);
        if (std::abs(side / std::max(side, length) - length / std::max(side, length)) > 1e-9L)
            throw std::invalid_argument("triangle must have equal sides");
    }
}

void Figure::validate_vertices(const std::vector<Point> &points) const {
    if (kind_ != FigureKind::Triangle && kind_ != FigureKind::Square &&
        kind_ != FigureKind::Rectangle)
        throw std::invalid_argument("unknown figure kind_");
    const auto count = kind_ == FigureKind::Triangle ? 3U : 4U;
    if (points.size() != count)
        throw std::invalid_argument("wrong vertex count");
    for (const auto &point : points)
        if (!std::isfinite(point.x) || !std::isfinite(point.y))
            throw std::invalid_argument("nonfinite coordinate");

    constexpr auto tolerance = 1e-9L;
    std::array<Point, 4> directions{};
    std::array<long double, 4> lengths{};
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

void Square::validate(const std::vector<Point> &points) const {
    Rectangle::validate(points);
    constexpr auto tolerance = 1e-9L;
    for (auto i = 0U; i < 4; ++i) {
        const auto &a = points[i];
        const auto &b = points[(i + 1) % 4];
        const auto &c = points[(i + 2) % 4];
        const auto dx1 = a.x - b.x, dy1 = a.y - b.y;
        const auto dx2 = c.x - b.x, dy2 = c.y - b.y;
        const auto first = std::hypot(dx1, dy1), second = std::hypot(dx2, dy2);
        const auto scale = std::max(first, second);
        if (std::abs(first / scale - second / scale) > tolerance)
            throw std::invalid_argument("square must have equal sides");
    }
}

void Rectangle::validate(const std::vector<Point> &points) const {
    validate_vertices(points);
    constexpr auto tolerance = 1e-9L;
    for (auto i = 0U; i < 4; ++i) {
        const auto &a = points[i];
        const auto &b = points[(i + 1) % 4];
        const auto &c = points[(i + 2) % 4];
        const auto dx1 = a.x - b.x, dy1 = a.y - b.y;
        const auto dx2 = c.x - b.x, dy2 = c.y - b.y;
        const auto first = std::hypot(dx1, dy1), second = std::hypot(dx2, dy2);
        const auto dot = (dx1 / first) * (dx2 / second) + (dy1 / first) * (dy2 / second);
        if (std::abs(dot) > tolerance)
            throw std::invalid_argument("rectangle must have right angles");
    }
}
