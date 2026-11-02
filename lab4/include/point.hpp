#pragma once

#include <concepts>
#include <istream>
#include <ostream>
#include <type_traits>

// Coordinates must support numeric arithmetic; pointers, bool and enums
// are not coordinate types even though some are scalar in the C++ taxonomy.
template <class T>
concept Coordinate = std::is_arithmetic_v<T> && !std::same_as<T, bool>;

template <Coordinate T> struct Point {
    T x{}, y{};
    bool operator==(const Point &) const = default;
    friend std::ostream &operator<<(std::ostream &out, const Point &point) {
        return out << '(' << point.x << ", " << point.y << ')';
    }
    friend std::istream &operator>>(std::istream &in, Point &point) {
        T x{}, y{};
        if (in >> x >> y)
            point = {x, y};
        return in;
    }
};
