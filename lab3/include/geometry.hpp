#pragma once

#include <vector>


namespace geometry {
enum class FigureKind { Triangle, Square, Rectangle };

struct Point {
    long double x, y;
};

long double cross(Point a, Point b, Point c);
long double distance2(Point a, Point b);
Point center(const std::vector<Point> &p);
bool equal(const std::vector<Point> &a, const std::vector<Point> &b);
} // namespace geometry
