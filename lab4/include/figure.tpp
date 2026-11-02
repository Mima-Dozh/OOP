#pragma once

#include "figure.hpp"

#include <istream>
#include <memory>
#include <ostream>
#include <stdexcept>
#include <utility>
#include <vector>

template <Coordinate T>
std::vector<geometry::Point> Figure<T>::coordinates() const {
    std::vector<geometry::Point> result;
    for (const auto &p : points_)
        result.push_back({static_cast<long double>(p->x), static_cast<long double>(p->y)});
    return result;
}

template <Coordinate T>
void Figure<T>::set(std::vector<Point<T>> values, geometry::FigureKind type) {
    std::vector<geometry::Point> check;
    for (auto p : values)
        check.push_back({static_cast<long double>(p.x), static_cast<long double>(p.y)});
    ::validate(check, type);
    std::vector<std::unique_ptr<Point<T>>> temp;
    for (auto p : values)
        temp.push_back(std::make_unique<Point<T>>(p));
    points_ = std::move(temp);
}

template <Coordinate T>
Figure<T>::Figure(const Figure &other) {
    for (const auto &p : other.points_)
        points_.push_back(std::make_unique<Point<T>>(*p));
}

template <Coordinate T>
Figure<T> &Figure<T>::operator=(const Figure &other) {
    if (this != &other) {
        std::vector<std::unique_ptr<Point<T>>> temp;
        for (const auto &p : other.points_)
            temp.push_back(std::make_unique<Point<T>>(*p));
        points_ = std::move(temp);
    }
    return *this;
}

template <Coordinate T>
Point<double> Figure<T>::center() const {
    auto p = geometry::center(coordinates());
    return {static_cast<double>(p.x), static_cast<double>(p.y)};
}

template <Coordinate T>
Figure<T>::operator double() const { return ::area(coordinates(), kind()); }

template <Coordinate T>
bool Figure<T>::operator==(const Figure &other) const {
    return kind() == other.kind() && geometry::equal(coordinates(), other.coordinates());
}

template <Coordinate T>
std::ostream &operator<<(std::ostream &out, const Figure<T> &figure) {
    out << (figure.kind() == geometry::FigureKind::Triangle ? "Triangle" : figure.kind() == geometry::FigureKind::Square ? "Square" : "Rectangle");
    for (const auto &p : figure.points_)
        out << ' ' << *p;
    return out;
}

template <Coordinate T>
std::istream &operator>>(std::istream &in, Figure<T> &figure) {
    std::vector<Point<T>> values(figure.kind() == geometry::FigureKind::Triangle ? 3 : 4);
    for (auto &p : values)
        if (!(in >> p))
            return in;
    try {
        figure.set(std::move(values), figure.kind());
    } catch (const std::invalid_argument &) {
        in.setstate(std::ios::failbit);
    }
    return in;
}

template <Coordinate T>
geometry::FigureKind Triangle<T>::kind() const { return geometry::FigureKind::Triangle; }

template <Coordinate T>
Triangle<T>::Triangle() { this->set({{0, 0}, {2, 0}, {1, 2}}, geometry::FigureKind::Triangle); }

template <Coordinate T>
Triangle<T>::Triangle(std::vector<Point<T>> p) { this->set(std::move(p), geometry::FigureKind::Triangle); }

template <Coordinate T>
std::shared_ptr<Figure<T>> Triangle<T>::clone() const { return std::make_shared<Triangle>(*this); }

template <Coordinate T>
geometry::FigureKind Square<T>::kind() const { return geometry::FigureKind::Square; }

template <Coordinate T>
Square<T>::Square() { this->set({{0, 0}, {1, 0}, {1, 1}, {0, 1}}, geometry::FigureKind::Square); }

template <Coordinate T>
Square<T>::Square(std::vector<Point<T>> p) { this->set(std::move(p), geometry::FigureKind::Square); }

template <Coordinate T>
std::shared_ptr<Figure<T>> Square<T>::clone() const { return std::make_shared<Square>(*this); }

template <Coordinate T>
geometry::FigureKind Rectangle<T>::kind() const { return geometry::FigureKind::Rectangle; }

template <Coordinate T>
Rectangle<T>::Rectangle() { this->set({{0, 0}, {2, 0}, {2, 1}, {0, 1}}, geometry::FigureKind::Rectangle); }

template <Coordinate T>
Rectangle<T>::Rectangle(std::vector<Point<T>> p) { this->set(std::move(p), geometry::FigureKind::Rectangle); }

template <Coordinate T>
std::shared_ptr<Figure<T>> Rectangle<T>::clone() const { return std::make_shared<Rectangle>(*this); }
