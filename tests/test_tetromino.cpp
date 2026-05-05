#include <gtest/gtest.h>

#include "tetromino.hpp"

using namespace tetris;

class TetrominoTest : public ::testing::Test {
protected:
    void SetUp() override {}
};

TEST_F(TetrominoTest, CreateITetromino) {
    Tetromino t(TetrominoType::I);
    EXPECT_EQ(t.type(), TetrominoType::I);
    EXPECT_EQ(t.rotation(), Rotation::R0);
    EXPECT_FALSE(t.cells().empty());
    EXPECT_EQ(t.cells().size(), 4);
}

TEST_F(TetrominoTest, CreateAllTypes) {
    for (auto type : {TetrominoType::I, TetrominoType::O, TetrominoType::T,
                      TetrominoType::S, TetrominoType::Z, TetrominoType::J,
                      TetrominoType::L}) {
        Tetromino t(type);
        EXPECT_EQ(t.type(), type);
        EXPECT_EQ(t.rotation(), Rotation::R0);
        EXPECT_EQ(t.cells().size(), 4);
    }
}

TEST_F(TetrominoTest, RotateClockwise) {
    Tetromino t(TetrominoType::T);
    auto cells_before = std::vector<Position>(t.cells().begin(), t.cells().end());

    t.rotate_clockwise();
    EXPECT_EQ(t.rotation(), Rotation::R90);

    auto cells_after = std::vector<Position>(t.cells().begin(), t.cells().end());
    EXPECT_NE(cells_before, cells_after);
}

TEST_F(TetrominoTest, RotateFullCircle) {
    Tetromino t(TetrominoType::T);
    auto cells_before = std::vector<Position>(t.cells().begin(), t.cells().end());

    t.rotate_clockwise();
    t.rotate_clockwise();
    t.rotate_clockwise();
    t.rotate_clockwise();
    EXPECT_EQ(t.rotation(), Rotation::R0);

    auto cells_after = std::vector<Position>(t.cells().begin(), t.cells().end());
    EXPECT_EQ(cells_before, cells_after);
}

TEST_F(TetrominoTest, RotateCounterClockwise) {
    Tetromino t(TetrominoType::T);
    t.rotate_counter_clockwise();
    EXPECT_EQ(t.rotation(), Rotation::R270);
}
