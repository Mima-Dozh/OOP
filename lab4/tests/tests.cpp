#include "array_test_types.hpp"
#include "array.hpp"
#include "figure.hpp"

#include <gtest/gtest.h>
#include <sstream>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <vector>

static_assert(Coordinate<int> && Coordinate<double> && !Coordinate<std::string> &&
              !Coordinate<bool>);
static_assert(!std::is_assignable_v<Figure<double> &, const Figure<double> &>);
static_assert(!std::is_assignable_v<Figure<double> &, Figure<double> &&>);
static_assert(!std::is_assignable_v<Rectangle<double> &, const Triangle<double> &>);
static_assert(!std::is_assignable_v<Rectangle<double> &, Triangle<double> &&>);

template <class Shape>
class FigureAssignmentTest : public ::testing::Test {};
using AssignableFigures = ::testing::Types<Triangle<int>, Square<int>, Rectangle<int>,
                                          Triangle<double>, Square<double>, Rectangle<double>>;
TYPED_TEST_SUITE(FigureAssignmentTest, AssignableFigures);

TYPED_TEST(FigureAssignmentTest, CopyAndMovePreserveConcreteFigure) {
    TypeParam original;
    TypeParam copied;
    EXPECT_EQ(&(copied = original), &copied);
    EXPECT_TRUE(copied == original);
    TypeParam moved;
    EXPECT_EQ(&(moved = std::move(copied)), &moved);
    EXPECT_TRUE(moved == original);
    EXPECT_DOUBLE_EQ(static_cast<double>(moved), static_cast<double>(original));
}
TEST(Figures, TypedAreasCenters) {
    Triangle<int> t;
    EXPECT_DOUBLE_EQ(static_cast<double>(t), 2);
    EXPECT_DOUBLE_EQ(t.center().x, 1);
    EXPECT_NEAR(t.center().y, 2. / 3, 1e-12);
    Square<double> s;
    Rectangle<float> r;
    EXPECT_DOUBLE_EQ(static_cast<double>(s), 1);
    EXPECT_DOUBLE_EQ(static_cast<double>(r), 2);
}

TEST(Figures, ReusedGeometryPreservesPrecisionAndValidation) {
    Rectangle<long double> rectangle({{10, 20}, {14, 20}, {14, 23}, {10, 23}});
    EXPECT_DOUBLE_EQ(static_cast<double>(rectangle), 12);
    EXPECT_DOUBLE_EQ(rectangle.center().x, 12);
    EXPECT_DOUBLE_EQ(rectangle.center().y, 21.5);
    EXPECT_THROW(Triangle<int>({{0, 0}, {1, 0}, {2, 0}}), std::invalid_argument);
    EXPECT_THROW(Rectangle<int>({{0, 0}, {2, 0}, {3, 1}, {1, 1}}), std::invalid_argument);
    EXPECT_THROW(Square<int>(std::vector<Point<int>>{}), std::invalid_argument);
    const Triangle<int> isosceles({{0, 0}, {4, 0}, {2, 3}});
    EXPECT_DOUBLE_EQ(static_cast<double>(isosceles), 6);
}
TEST(Figures, DeepCopy) {
    Square<int> a;
    Square<int> b = a;
    std::istringstream input("0 0 3 0 3 3 0 3");
    input >> b;
    EXPECT_DOUBLE_EQ(static_cast<double>(a), 1);
    EXPECT_DOUBLE_EQ(static_cast<double>(b), 9);
    a = b;
    EXPECT_TRUE(a == b);
    Square<int> c = std::move(b);
    EXPECT_TRUE(a == c);
    auto cloned = a.clone();
    EXPECT_TRUE(*cloned == a);
}
TEST(Figures, InputAndValidation) {
    Triangle<int> t;
    std::istringstream bad("0 0 4 0 1 2");
    bad >> t;
    EXPECT_TRUE(bad.fail());
    EXPECT_DOUBLE_EQ(static_cast<double>(t), 2);
    EXPECT_THROW(Square<int>({{0, 0}, {2, 0}, {2, 1}, {0, 1}}), std::invalid_argument);
    EXPECT_THROW(Rectangle<int>({{0, 0}, {1, 1}, {0, 1}, {1, 0}}), std::invalid_argument);
    Square<int> rotated({{0, 1}, {1, 2}, {2, 1}, {1, 0}});
    EXPECT_DOUBLE_EQ(static_cast<double>(rotated), 2);
}
TEST(FigureStorage, ConcreteAndPolymorphic) {
    FigureArray<Square<int>> values;
    for (int i = 0; i < 10; ++i)
        values.push_back(Square<int>{});
    EXPECT_EQ(values.size(), 10);
    values.erase(4);
    EXPECT_EQ(values.size(), 9);
    FigureArray<std::shared_ptr<Figure<int>>> figures;
    figures.push_back(std::make_shared<Triangle<int>>());
    figures.push_back(std::make_shared<Rectangle<int>>());
    EXPECT_DOUBLE_EQ(static_cast<double>(*figures[0]) + static_cast<double>(*figures[1]), 4);
}
TEST(FigureStorage, ValueCopyMoveAndBounds) {
    FigureArray<Square<int>> a;
    a.push_back(Square<int>{});
    auto b = a;
    std::istringstream input("0 0 2 0 2 2 0 2");
    input >> b[0];
    EXPECT_DOUBLE_EQ(static_cast<double>(a[0]), 1);
    EXPECT_DOUBLE_EQ(static_cast<double>(b[0]), 4);
    FigureArray<Square<int>> c = std::move(b);
    EXPECT_EQ(c.size(), 1);
    EXPECT_THROW(c[3], std::out_of_range);
    c.erase(0);
    EXPECT_TRUE(c.empty());
}



TEST(FigureArray, GrowthMovesExistingElements) {
    FigureArray<MoveCounter> values;
    values.push_back(MoveCounter(10));
    values.push_back(MoveCounter(20));
    MoveCounter::copies = MoveCounter::moves = 0;
    values.reserve(10);
    EXPECT_EQ(MoveCounter::copies, 0);
    EXPECT_EQ(MoveCounter::moves, 2);
    EXPECT_EQ(values[0].value, 10);
    EXPECT_EQ(values[1].value, 20);
    EXPECT_GE(values.capacity(), 10U);
}

TEST(FigureArray, EraseReleasesOwnershipAndPreservesOrder) {
    FigureArray<std::shared_ptr<Figure<int>>> values;
    values.push_back(std::make_shared<Triangle<int>>());
    values.push_back(std::make_shared<Square<int>>());
    values.push_back(std::make_shared<Rectangle<int>>());
    const std::weak_ptr<Figure<int>> removed = values[1];
    const auto last = values[2];
    values.erase(1);
    EXPECT_TRUE(removed.expired());
    ASSERT_EQ(values.size(), 2U);
    EXPECT_EQ(values[1], last);
    EXPECT_THROW(values.erase(2), std::out_of_range);
    values.clear();
    EXPECT_TRUE(values.empty());
}

TEST(FigureArray, CopyAndMoveAssignmentAndMovedFromReuse) {
    FigureArray<Square<int>> original;
    original.push_back(Square<int>{});
    FigureArray<Square<int>> copy;
    copy = original;
    std::istringstream input("0 0 2 0 2 2 0 2");
    input >> copy[0];
    EXPECT_DOUBLE_EQ(static_cast<double>(original[0]), 1);
    FigureArray<Square<int>> moved;
    moved = std::move(copy);
    EXPECT_TRUE(copy.empty());
    EXPECT_DOUBLE_EQ(static_cast<double>(moved[0]), 4);
    copy.push_back(Square<int>{});
    EXPECT_EQ(copy.size(), 1U);
    auto &alias = moved;
    moved = std::move(alias);
    EXPECT_EQ(moved.size(), 1U);
}
