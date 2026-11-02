#pragma once

#include "geometry.hpp"
#include "point.hpp"

#include <memory>
#include <vector>

void validate_vertices(const std::vector<geometry::Point> &points, geometry::FigureKind kind);
void validate(const std::vector<geometry::Point> &points, geometry::FigureKind kind);
double area(const std::vector<geometry::Point> &points, geometry::FigureKind kind);

template <Coordinate T> class Figure {
  protected:
    std::vector<std::unique_ptr<Point<T>>> points_;
    virtual geometry::FigureKind kind() const = 0;
    std::vector<geometry::Point> coordinates() const;
    void set(std::vector<Point<T>> values, geometry::FigureKind type);
    Figure &operator=(const Figure &other);
    Figure &operator=(Figure &&) noexcept = default;

  public:
    Figure() = default;
    virtual ~Figure() = default;
    Figure(const Figure &other);
    Figure(Figure &&) noexcept = default;
    Point<double> center() const;
    explicit operator double() const;
    bool operator==(const Figure &other) const;
    virtual std::shared_ptr<Figure<T>> clone() const = 0;
    template <Coordinate U>
    friend std::ostream &operator<<(std::ostream &out, const Figure<U> &figure);
    template <Coordinate U>
    friend std::istream &operator>>(std::istream &in, Figure<U> &figure);
};
template <Coordinate T> class Triangle final : public Figure<T> {
    geometry::FigureKind kind() const override;

  public:
    Triangle();
    explicit Triangle(std::vector<Point<T>> p);
    Triangle(const Triangle &) = default;
    Triangle(Triangle &&) noexcept = default;
    Triangle &operator=(const Triangle &) = default;
    Triangle &operator=(Triangle &&) noexcept = default;
    std::shared_ptr<Figure<T>> clone() const override;
};
template <Coordinate T> class Square final : public Figure<T> {
    geometry::FigureKind kind() const override;

  public:
    Square();
    explicit Square(std::vector<Point<T>> p);
    Square(const Square &) = default;
    Square(Square &&) noexcept = default;
    Square &operator=(const Square &) = default;
    Square &operator=(Square &&) noexcept = default;
    std::shared_ptr<Figure<T>> clone() const override;
};
template <Coordinate T> class Rectangle final : public Figure<T> {
    geometry::FigureKind kind() const override;

  public:
    Rectangle();
    explicit Rectangle(std::vector<Point<T>> p);
    Rectangle(const Rectangle &) = default;
    Rectangle(Rectangle &&) noexcept = default;
    Rectangle &operator=(const Rectangle &) = default;
    Rectangle &operator=(Rectangle &&) noexcept = default;
    std::shared_ptr<Figure<T>> clone() const override;
};

#include "figure.tpp"
