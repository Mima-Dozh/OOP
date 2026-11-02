#pragma once

#include "array.hpp"
#include "geometry.hpp"
#include <cstddef>
#include <iosfwd>
#include <vector>


using Point = geometry::Point;
using FigureKind = geometry::FigureKind;

class Figure {
  protected:
    std::vector<Point> points_;
    FigureKind kind_;
    virtual void validate(const std::vector<Point> &points) const;
    void validate_vertices(const std::vector<Point> &points) const;
    Figure(std::vector<Point> p, FigureKind kind);
    Figure(const Figure &) = default;
    Figure(Figure &&) noexcept = default;
    Figure &operator=(const Figure &other);
    Figure &operator=(Figure &&other);

  public:
    virtual ~Figure() = default;
    virtual Point center() const;
    virtual double area() const;
    explicit operator double() const;
    virtual bool equals(const Figure &) const;
    friend bool operator==(const Figure &a, const Figure &b);
    friend std::ostream &operator<<(std::ostream &out, const Figure &f);
    friend std::istream &operator>>(std::istream &in, Figure &f);
};

class Triangle final : public Figure {
  protected:
    void validate(const std::vector<Point> &points) const override;

  public:
    Triangle();
    double area() const override;
    explicit Triangle(std::vector<Point> p);
    Triangle(const Triangle &other);
    Triangle(Triangle &&) noexcept = default;
    Triangle &operator=(const Triangle &other) = default;
    Triangle &operator=(Triangle &&other) = default;
};

class Rectangle : public Figure {
  protected:
    Rectangle(std::vector<Point> p, FigureKind kind);
    void validate(const std::vector<Point> &points) const override;

  public:
    Rectangle();
    double area() const override;
    explicit Rectangle(std::vector<Point> p);
    Rectangle(const Rectangle &other);
    Rectangle(Rectangle &&) noexcept = default;
    Rectangle &operator=(const Rectangle &other) = default;
    Rectangle &operator=(Rectangle &&other) = default;
};

class Square final : public Rectangle {
  protected:
    void validate(const std::vector<Point> &points) const override;

  public:
    Square();
    double area() const override;
    explicit Square(std::vector<Point> p);
    Square(const Square &other);
    Square(Square &&) noexcept = default;
    Square &operator=(const Square &other) = default;
    Square &operator=(Square &&other) = default;
};

double total_area(const FigureArray &figures);
// Deletes the owned figure and removes its pointer from the array.
void erase_figure(FigureArray &figures, std::size_t index);
