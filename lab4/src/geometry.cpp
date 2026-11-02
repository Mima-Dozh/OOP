#include "geometry.hpp"

#include <stdexcept>

namespace geometry {
long double cross(Point a, Point b, Point c) {
    return (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x);
}
long double distance2(Point a, Point b) {
    return (a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y);
}
Point center(const std::vector<Point> &p) {
    if (p.empty())
        throw std::logic_error("empty moved-from figure");
    auto c = Point{};
    for (auto v : p) {
        c.x += v.x;
        c.y += v.y;
    }
    c.x /= p.size();
    c.y /= p.size();
    return c;
}
bool equal(const std::vector<Point> &a, const std::vector<Point> &b) {
    if (a.size() != b.size())
        return false;
    if (a.empty())
        return true;
    for (std::size_t shift = 0; shift < a.size(); ++shift)
        for (auto sign : {-1, 1}) {
            auto match = true;
            for (std::size_t i = 0; i < a.size(); ++i) {
                auto j = (shift + a.size() + sign * static_cast<int>(i)) % a.size();
                if (a[i].x != b[j].x || a[i].y != b[j].y) {
                    match = false;
                    break;
                }
            }
            if (match)
                return true;
        }
    return false;
}
} // namespace geometry
