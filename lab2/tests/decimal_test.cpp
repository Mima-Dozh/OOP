#include "lib/decimal.h"

#include <gtest/gtest.h>
#include <stdexcept>
#include <utility>


TEST(Decimal, ConstructorsAndDigitOrder) {
    EXPECT_EQ(Decimal().to_string(), "0");
    EXPECT_EQ(Decimal({3, 2, 1, 0}).to_string(), "123");
    const Decimal value("000123");
    EXPECT_EQ(value.size(), 3U);
    EXPECT_EQ(value[0], 3);
    EXPECT_EQ(value[2], 1);
    EXPECT_EQ(value.to_string(), "123");
    EXPECT_EQ(Decimal("000").size(), 1U);
}

TEST(Decimal, RejectsInvalidInputAndIndices) {
    EXPECT_THROW(Decimal({}), std::invalid_argument);
    EXPECT_THROW(Decimal(""), std::invalid_argument);
    EXPECT_THROW(Decimal("-1"), std::invalid_argument);
    EXPECT_THROW(Decimal("12a"), std::invalid_argument);
    EXPECT_THROW(Decimal(" 12"), std::invalid_argument);
    EXPECT_THROW(Decimal({1, 10}), std::invalid_argument);
    EXPECT_THROW(Decimal("123")[3], std::out_of_range);
}

TEST(Decimal, ArbitraryPrecisionCarryAndBorrow) {
    const std::string nines(200, '9');
    const Decimal power("1" + std::string(200, '0'));
    EXPECT_EQ(Decimal(nines) + Decimal("1"), power);
    EXPECT_EQ((power - Decimal("1")).to_string(), nines);
    EXPECT_EQ((power - power).to_string(), "0");
    EXPECT_EQ(power + Decimal(), power);
    EXPECT_EQ(power - Decimal(), power);
}

TEST(Decimal, CompoundOperationsAndUnderflow) {
    Decimal value("99");
    EXPECT_EQ(&(value += value), &value);
    EXPECT_EQ(value.to_string(), "198");
    EXPECT_THROW(value -= Decimal("199"), std::underflow_error);
    EXPECT_EQ(value.to_string(), "198");
    EXPECT_THROW(Decimal() - Decimal("1"), std::underflow_error);
    EXPECT_EQ(&(value -= value), &value);
    EXPECT_EQ(value.to_string(), "0");
}

TEST(Decimal, CopyOwnsIndependentStorage) {
    Decimal original("123");
    Decimal copy(original);
    Decimal assigned;
    assigned = original;
    original += Decimal("1");
    EXPECT_EQ(copy.to_string(), "123");
    EXPECT_EQ(assigned.to_string(), "123");
    Decimal& alias = original;
    original = alias;
    EXPECT_EQ(original.to_string(), "124");
}

TEST(Decimal, RvalueCopiesPreserveSourceAndOwnStorage) {
    Decimal original("123");
    Decimal moved(std::move(original));
    EXPECT_EQ(moved.to_string(), "123");
    EXPECT_EQ(original.to_string(), "123");
    original -= Decimal("3");
    EXPECT_EQ(moved.to_string(), "123");
    moved = std::move(original);
    EXPECT_EQ(moved.to_string(), "120");
    EXPECT_EQ(original.to_string(), "120");
    original -= Decimal("1");
    EXPECT_EQ(moved.to_string(), "120");
    EXPECT_EQ(original.to_string(), "119");
    Decimal& alias = moved;
    moved = std::move(alias);
    EXPECT_EQ(moved.to_string(), "120");
}

TEST(Decimal, ArithmeticAndComparisonsAgreeWithNativeIntegers) {
    for (unsigned int a = 0; a < 150; ++a) {
        for (unsigned int b = 0; b < 150; ++b) {
            const Decimal left(std::to_string(a));
            const Decimal right(std::to_string(b));
            EXPECT_EQ((left + right).to_string(), std::to_string(a + b));
            if (a >= b) {
                EXPECT_EQ((left - right).to_string(), std::to_string(a - b));
            }
            EXPECT_EQ(left == right, a == b);
            EXPECT_EQ(left != right, a != b);
            EXPECT_EQ(left < right, a < b);
            EXPECT_EQ(left > right, a > b);
            EXPECT_EQ(left <= right, a <= b);
            EXPECT_EQ(left >= right, a >= b);
        }
    }
    EXPECT_EQ(Decimal("00042"), Decimal("42"));
}
