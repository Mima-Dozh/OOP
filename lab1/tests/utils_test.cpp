#include "lib/utils.h"

#include <gtest/gtest.h>

TEST(BinaryOnesWindow, ZeroContainsNoOnes) {
    EXPECT_EQ(Count_Of_Ones_In_Binary_Representation_Of_Window(0, 0), 0);
}

TEST(BinaryOnesWindow, CountsSingleValues) {
    EXPECT_EQ(Count_Of_Ones_In_Binary_Representation_Of_Window(1, 1), 1);
    EXPECT_EQ(Count_Of_Ones_In_Binary_Representation_Of_Window(5, 5), 2);
    EXPECT_EQ(Count_Of_Ones_In_Binary_Representation_Of_Window(15, 15), 4);
}

TEST(BinaryOnesWindow, PowerOfTwoContainsOneBit) {
    EXPECT_EQ(Count_Of_Ones_In_Binary_Representation_Of_Window(1024, 1024), 1);
}

TEST(BinaryOnesWindow, IncludesBothEndpoints) {
    // 2..7: 10, 11, 100, 101, 110, 111 -> 1 + 2 + 1 + 2 + 2 + 3.
    EXPECT_EQ(Count_Of_Ones_In_Binary_Representation_Of_Window(2, 7), 11);
}

TEST(BinaryOnesWindow, CountsAcrossPowerOfTwoBoundary) {
    // 1023 has ten set bits; 1024 has one.
    EXPECT_EQ(Count_Of_Ones_In_Binary_Representation_Of_Window(1023, 1024), 11);
}

TEST(BinaryOnesWindow, CountsRangeStartingAtOne) {
    EXPECT_EQ(Count_Of_Ones_In_Binary_Representation_Of_Window(1, 10), 17);
}

TEST(BinaryOnesWindow, CountsCompleteFourBitRange) {
    // In 0..15, each of the four bit positions is set eight times.
    EXPECT_EQ(Count_Of_Ones_In_Binary_Representation_Of_Window(0, 15), 32);
}
