#include <gtest/gtest.h>

#include "types.hpp"

using namespace tetris;

TEST(PositionTest, DefaultComparison) {
    Position a{ 0, 0 };
    Position b{ 0, 0 };
    Position c{ 1, 0 };
    EXPECT_EQ(a, b);
    EXPECT_NE(a, c);
    EXPECT_LT(a, c);
}

TEST(TypesTest, EnumValues) {
    EXPECT_NE(TetrominoType::I, TetrominoType::O);
    EXPECT_NE(Rotation::R0, Rotation::R90);
}

TEST(TypesTest, Constants) {
    EXPECT_EQ(kBoardWidth, 10);
    EXPECT_EQ(kBoardVisibleHeight, 20);
    EXPECT_EQ(kBoardBufferHeight, 4);
    EXPECT_EQ(kBoardTotalHeight, 24);
}
