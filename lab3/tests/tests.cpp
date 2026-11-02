#include "figure.hpp"
#include <cmath>
#include <gtest/gtest.h>
#include <sstream>
#include <type_traits>
#include <utility>
#include <stdexcept>
#include <limits>

TEST(FigureArray, CopyOwnsIndependentStorage) {
    Triangle triangle;
    Square square;
    FigureArray original;
    original.push_back(&triangle);
    FigureArray copy(original);
    copy[0] = &square;
    EXPECT_EQ(original[0], &triangle);

    FigureArray assigned;
    assigned.push_back(&triangle);
    assigned = copy;
    copy.clear();
    EXPECT_EQ(assigned[0], &square);
    FigureArray &alias = assigned;
    assigned = alias;
    EXPECT_EQ(assigned[0], &square);
}

TEST(FigureArray, MoveClearAndGrowthPreserveOwnership) {
    Triangle triangle;
    FigureArray original;
    original.push_back(&triangle);
    for (int i = 1; i < 16; ++i)
        original.push_back(&triangle);
    EXPECT_EQ(original.size(), 16U);
    EXPECT_EQ(original[0], &triangle);
    FigureArray moved(std::move(original));
    EXPECT_TRUE(original.empty());
    original.push_back(&triangle);

    FigureArray assigned;
    assigned.push_back(nullptr);
    assigned = std::move(moved);
    EXPECT_TRUE(moved.empty());
    EXPECT_EQ(assigned[0], &triangle);
    FigureArray &alias = assigned;
    assigned = std::move(alias);
    EXPECT_EQ(assigned[0], &triangle);
    assigned.clear();
    assigned.clear();
    EXPECT_TRUE(assigned.empty());
    assigned.push_back(&triangle);
    EXPECT_EQ(assigned[0], &triangle);
}

TEST(Figures, DefaultAreaReportsNotImplemented) {
    class UnimplementedFigure : public Figure {
      public:
        UnimplementedFigure() : Figure({{0, 0}, {1, 0}, {1, 1}, {0, 1}}, FigureKind::Square) {}
    };
    const UnimplementedFigure figure;
    try {
        figure.area();
        FAIL() << "Expected an error for an unimplemented area";
    } catch (const std::logic_error &error) {
        EXPECT_STREQ(error.what(), "Not implemented");
    }
}

TEST(Figures, DefaultValidationReportsNotImplemented) {
    class UnimplementedFigure : public Figure {
      public:
        UnimplementedFigure() : Figure({}, FigureKind::Triangle) {}
        using Figure::validate;
    };
    const UnimplementedFigure figure;
    try {
        figure.validate({{0, 0}, {4, 0}, {0, 3}});
        FAIL() << "Expected an error for unimplemented validation";
    } catch (const std::logic_error &error) {
        EXPECT_STREQ(error.what(), "Not implemented");
    }
}

class FigureUtilsTest : public ::testing::Test {
  protected:
    FigureArray figures;
    void TearDown() override {
        for (std::size_t i = 0; i < figures.size(); ++i)
            delete figures[i];
    }
};

TEST_F(FigureUtilsTest, TotalAreaOfEmptyAndMixedFigures) {
    EXPECT_DOUBLE_EQ(total_area(figures), 0);
    figures.push_back(new Triangle);
    figures.push_back(new Square);
    figures.push_back(new Rectangle);
    EXPECT_NEAR(total_area(figures), std::sqrt(3.) + 3, 1e-9);
}

TEST_F(FigureUtilsTest, ErasePreservesOrderAndChecksIndex) {
    figures.push_back(new Triangle);
    figures.push_back(new Square);
    figures.push_back(new Rectangle);
    const auto *first = figures[0];
    const auto *last = figures[2];
    EXPECT_THROW(erase_figure(figures, figures.size()), std::out_of_range);
    ASSERT_EQ(figures.size(), 3U);
    erase_figure(figures, 1);
    ASSERT_EQ(figures.size(), 2U);
    EXPECT_EQ(figures[0], first);
    EXPECT_EQ(figures[1], last);
    EXPECT_NEAR(total_area(figures), std::sqrt(3.) + 2, 1e-9);
    erase_figure(figures, 1);
    erase_figure(figures, 0);
    EXPECT_TRUE(figures.empty());
    EXPECT_DOUBLE_EQ(total_area(figures), 0);
    EXPECT_THROW(erase_figure(figures, 0), std::out_of_range);
}

static_assert(!std::is_assignable_v<Figure &, const Figure &>);
static_assert(!std::is_assignable_v<Figure &, Figure &&>);
static_assert(std::is_base_of_v<Rectangle, Square>);

TEST(Figures, SquareWorksThroughRectangleReference) {
    Square square;
    Rectangle &rectangle = square;
    std::istringstream invalid("0 0 3 0 3 2 0 2");
    invalid >> rectangle;
    EXPECT_TRUE(invalid.fail());
    EXPECT_DOUBLE_EQ(rectangle.area(), 1);
    std::istringstream valid("0 0 3 0 3 3 0 3");
    valid >> rectangle;
    EXPECT_FALSE(valid.fail());
    EXPECT_DOUBLE_EQ(rectangle.area(), 9);
    std::ostringstream output;
    output << rectangle;
    EXPECT_EQ(output.str(), "Square (0, 0) (3, 0) (3, 3) (0, 3)");
}

TEST(Figures, EqualityRequiresExactCoordinates) {
    const std::vector<Point> points{{0, 0}, {1, 0}, {1, 1}, {0, 1}};
    const Square original(points);
    EXPECT_TRUE(original == Square(points));
    auto changed = points;
    changed[0].x = 1e-12L;
    EXPECT_FALSE(original == Square(changed));
    changed = points;
    changed[0].y = 1e-12L;
    EXPECT_FALSE(original == Square(changed));
}

TEST(Figures, SameVerticesWithDifferentKindsAreNotEqual) {
    const std::vector<Point> points{{0, 0}, {1, 0}, {1, 1}, {0, 1}};
    Square square(points);
    Rectangle rectangle(points);
    EXPECT_DOUBLE_EQ(square.area(), rectangle.area());
    EXPECT_FALSE(square == rectangle);
    EXPECT_FALSE(rectangle == square);
}

TEST(Figures, IncompleteInputPreservesFigure) {
    Rectangle rectangle;
    const Rectangle original(rectangle);
    Figure &figure = rectangle;
    for (const auto *text : {"", "0 0 3 0", "0 0 3 0 invalid 2 0 2"}) {
        std::istringstream input(text);
        EXPECT_EQ(&(input >> figure), &input);
        EXPECT_TRUE(input.fail());
        EXPECT_TRUE(figure == original);
    }
    std::istringstream valid("0 0 3 0 3 2 0 2");
    EXPECT_EQ(&(valid >> figure), &valid);
    EXPECT_FALSE(valid.fail());
    EXPECT_DOUBLE_EQ(figure.area(), 6);
    std::ostringstream output;
    EXPECT_EQ(&(output << figure), &output);
    EXPECT_EQ(output.str(), "Rectangle (0, 0) (3, 0) (3, 2) (0, 2)");
}

TEST(Figures, AreasAndCenters) {
    Triangle t;
    Square s;
    Rectangle r;
    EXPECT_NEAR(t.area(), std::sqrt(3.), 1e-9);
    EXPECT_DOUBLE_EQ(s.area(), 1);
    EXPECT_DOUBLE_EQ(r.area(), 2);
    EXPECT_NEAR(t.center().x, 1, 1e-9);
    EXPECT_NEAR(t.center().y, std::sqrt(3.) / 3, 1e-9);
    EXPECT_EQ(r.center().x, 1);
}

TEST(Figures, TypeSpecificAreasThroughBaseReferences) {
    const Triangle triangle({{0, 0}, {4, 0}, {2, 2 * std::sqrt(3.L)}});
    const Triangle reversed({{2, 2 * std::sqrt(3.L)}, {4, 0}, {0, 0}});
    const Square square({{1, 2}, {4, 2}, {4, 5}, {1, 5}});
    const Rectangle rectangle({{1, 2}, {5, 2}, {5, 4}, {1, 4}});
    const Figure &t = triangle;
    const Figure &s = square;
    const Figure &r = rectangle;
    EXPECT_NEAR(static_cast<double>(t), 4 * std::sqrt(3.), 1e-9);
    EXPECT_NEAR(static_cast<double>(reversed), 4 * std::sqrt(3.), 1e-9);
    EXPECT_DOUBLE_EQ(s.area(), 9);
    EXPECT_DOUBLE_EQ(r.area(), 8);
}
TEST(Figures, RotatedAndReversed) {
    Square a({{0, 1}, {1, 2}, {2, 1}, {1, 0}});
    Square b({{1, 0}, {2, 1}, {1, 2}, {0, 1}});
    EXPECT_TRUE(a == b);
    EXPECT_DOUBLE_EQ(a.area(), 2);
    Rectangle r({{0, 0}, {2, 2}, {1, 3}, {-1, 1}});
    EXPECT_DOUBLE_EQ(r.area(), 4);
}
TEST(Figures, ConstructorsRejectInvalidGeometry) {
    EXPECT_THROW(Triangle({{0, 0}, {2, 0}, {1, 0}}), std::invalid_argument);
    EXPECT_THROW(Square({{0, 0}, {2, 0}, {2, 1}, {0, 1}}), std::invalid_argument);
    EXPECT_THROW(Rectangle({{0, 0}, {1, 1}, {0, 1}, {1, 0}}), std::invalid_argument);
    EXPECT_THROW(Rectangle({{0, 0}, {1, 0}, {2, 0}, {3, 0}}), std::invalid_argument);
}
TEST(Figures, InvalidShapeInputPreservesFigure) {
    Square s;
    std::istringstream input("0 0 2 0 2 1 0 1");
    input >> s;
    EXPECT_TRUE(input.fail());
    EXPECT_DOUBLE_EQ(s.area(), 1);
    std::istringstream valid("0 0 2 0 2 2 0 2");
    valid >> s;
    EXPECT_TRUE(bool(valid));
    EXPECT_DOUBLE_EQ(s.area(), 4);
    std::ostringstream out;
    out << s;
    EXPECT_NE(out.str().find("Square"), std::string::npos);
}
TEST(Figures, CopyMoveAndPolymorphism) {
    Rectangle r;
    Rectangle copy;
    copy = r;
    EXPECT_TRUE(copy == r);
    Rectangle moved;
    moved = std::move(copy);
    EXPECT_TRUE(moved == r);
    Figure &base = moved;
    EXPECT_DOUBLE_EQ(base.area(), 2);
    Square s;
    EXPECT_FALSE(base == s);
}
template <typename T>
class FigureCopyTest : public ::testing::Test {};
using FigureTypes = ::testing::Types<Triangle, Square, Rectangle>;
TYPED_TEST_SUITE(FigureCopyTest, FigureTypes);

TYPED_TEST(FigureCopyTest, ConstructorCreatesIndependentCopy) {
    TypeParam original;
    TypeParam copy(original);
    EXPECT_TRUE(copy == original);
    const auto original_area = original.area();
    std::istringstream input(std::is_same_v<TypeParam, Triangle>
        ? "0 0 4 0 2 3.4641016151377546" : "0 0 4 0 4 4 0 4");
    input >> copy;
    ASSERT_FALSE(input.fail());
    EXPECT_FALSE(copy == original);
    EXPECT_DOUBLE_EQ(original.area(), original_area);
}

TYPED_TEST(FigureCopyTest, AssignmentCreatesIndependentCopy) {
    TypeParam original;
    std::istringstream input(std::is_same_v<TypeParam, Triangle>
        ? "0 0 4 0 2 3.4641016151377546" : "0 0 4 0 4 4 0 4");
    input >> original;
    ASSERT_FALSE(input.fail());
    TypeParam copy;
    EXPECT_EQ(&(copy = original), &copy);
    EXPECT_TRUE(copy == original);
    original = TypeParam();
    EXPECT_FALSE(copy == original);
    const TypeParam expected(copy);
    TypeParam &alias = copy;
    EXPECT_EQ(&(copy = alias), &copy);
    EXPECT_TRUE(copy == expected);
}

TYPED_TEST(FigureCopyTest, MoveAssignmentAndSelfMovePreserveValue) {
    TypeParam original;
    const TypeParam expected(original);
    TypeParam target;
    EXPECT_EQ(&(target = std::move(original)), &target);
    EXPECT_TRUE(target == expected);
    TypeParam &alias = target;
    EXPECT_EQ(&(target = std::move(alias)), &target);
    EXPECT_TRUE(target == expected);
}

TEST(Figures, RejectsMissingRepeatedAndNonfiniteVertices) {
    EXPECT_THROW(Triangle(std::vector<Point>{}), std::invalid_argument);
    EXPECT_THROW(Square({{0, 0}, {1, 0}, {1, 1}}), std::invalid_argument);
    EXPECT_THROW(Rectangle({{0, 0}, {0, 0}, {1, 1}, {0, 1}}), std::invalid_argument);
    const auto infinity = std::numeric_limits<long double>::infinity();
    const auto nan = std::numeric_limits<long double>::quiet_NaN();
    EXPECT_THROW(Triangle({{0, 0}, {1, 0}, {nan, 1}}), std::invalid_argument);
    EXPECT_THROW(Square({{0, 0}, {1, 0}, {1, infinity}, {0, 1}}), std::invalid_argument);
    EXPECT_THROW(Rectangle({{0, 0}, {2, 0}, {3, 1}, {1, 1}}), std::invalid_argument);
}

TEST(Figures, TriangleMustBeEquilateral) {
    EXPECT_THROW(Triangle({{0, 0}, {4, 0}, {0, 3}}), std::invalid_argument);
    EXPECT_THROW(Triangle({{0, 0}, {4, 0}, {2, 3}}), std::invalid_argument);
    Triangle triangle;
    const Triangle original(triangle);
    std::istringstream input("0 0 4 0 0 3");
    input >> triangle;
    EXPECT_TRUE(input.fail());
    EXPECT_TRUE(triangle == original);
    const Figure &figure = triangle;
    EXPECT_NEAR(static_cast<double>(figure), std::sqrt(3.), 1e-9);
}

TEST(Figures, SquareRejectsRectangleAssignmentWithoutChangingEitherObject) {
    Square square;
    const Square original_square(square);
    Rectangle rectangle({{0, 0}, {4, 0}, {4, 2}, {0, 2}});
    const Rectangle original_rectangle(rectangle);
    Rectangle &square_base = square;

    EXPECT_THROW(square_base = rectangle, std::invalid_argument);
    EXPECT_TRUE(square == original_square);
    EXPECT_TRUE(rectangle == original_rectangle);

    EXPECT_THROW(square_base = std::move(rectangle), std::invalid_argument);
    EXPECT_TRUE(square == original_square);
    EXPECT_TRUE(rectangle == original_rectangle);
}

TEST(Figures, SquareAcceptsSquareVerticesThroughRectangleAssignment) {
    Square square;
    Rectangle &square_base = square;
    const std::vector<Point> vertices{{0, 0}, {3, 0}, {3, 3}, {0, 3}};
    Rectangle rectangle(vertices);
    const Square expected(vertices);

    square_base = rectangle;
    EXPECT_TRUE(square == expected);
    EXPECT_DOUBLE_EQ(square.area(), 9);

    square = Square();
    square_base = std::move(rectangle);
    EXPECT_TRUE(square == expected);
    std::ostringstream output;
    output << square;
    EXPECT_EQ(output.str(), "Square (0, 0) (3, 0) (3, 3) (0, 3)");
}

TEST(Figures, RectangleAssignmentFromSquarePreservesRectangleType) {
    Rectangle rectangle;
    Square square({{0, 0}, {3, 0}, {3, 3}, {0, 3}});
    const Rectangle expected({{0, 0}, {3, 0}, {3, 3}, {0, 3}});

    rectangle = square;
    EXPECT_TRUE(rectangle == expected);
    rectangle = Rectangle();
    rectangle = std::move(square);
    EXPECT_TRUE(rectangle == expected);
    std::ostringstream output;
    output << rectangle;
    EXPECT_EQ(output.str(), "Rectangle (0, 0) (3, 0) (3, 3) (0, 3)");
}
