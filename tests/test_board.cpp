#include <gtest/gtest.h>

#include "board.hpp"

using namespace tetris;

class BoardTest : public ::testing::Test {
protected:
    Board board;
};

TEST_F(BoardTest, EmptyBoard) {
    for (int8_t row = 0; row < kBoardTotalHeight; ++row) {
        for (int8_t col = 0; col < kBoardWidth; ++col) {
            EXPECT_FALSE(board.is_occupied({row, col}));
            EXPECT_EQ(board.cell_at({row, col}), std::nullopt);
        }
    }
}

TEST_F(BoardTest, InBounds) {
    EXPECT_TRUE(board.is_in_bounds({0, 0}));
    EXPECT_TRUE(board.is_in_bounds({kBoardTotalHeight - 1, kBoardWidth - 1}));
    EXPECT_FALSE(board.is_in_bounds({-1, 0}));
    EXPECT_FALSE(board.is_in_bounds({0, -1}));
    EXPECT_FALSE(board.is_in_bounds({0, kBoardWidth}));
    EXPECT_FALSE(board.is_in_bounds({kBoardTotalHeight, 0}));
}

TEST_F(BoardTest, CanPlace) {
    std::vector<Position> cells = {{0, 0}, {0, 1}, {1, 0}, {1, 1}};
    EXPECT_TRUE(board.can_place(cells));
}

TEST_F(BoardTest, CannotPlaceOutOfBounds) {
    std::vector<Position> cells = {{-1, 0}};
    EXPECT_FALSE(board.can_place(cells));
}

TEST_F(BoardTest, LockCells) {
    std::vector<Position> cells = {{0, 0}, {0, 1}};
    board.lock(cells, TetrominoType::I);

    EXPECT_TRUE(board.is_occupied({0, 0}));
    EXPECT_TRUE(board.is_occupied({0, 1}));
    EXPECT_FALSE(board.is_occupied({1, 0}));
    EXPECT_EQ(board.cell_at({0, 0}), TetrominoType::I);
}

TEST_F(BoardTest, CannotPlaceOnOccupied) {
    std::vector<Position> cells = {{0, 0}};
    board.lock(cells, TetrominoType::I);
    EXPECT_FALSE(board.can_place(cells));
}

TEST_F(BoardTest, ClearSingleLine) {
    // fill the bottom row
    for (int8_t col = 0; col < kBoardWidth; ++col) {
        board.lock({{kBoardTotalHeight - 1, col}}, TetrominoType::I);
    }
    auto result = board.clear_lines();
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), 1);

    // row should now be empty
    for (int8_t col = 0; col < kBoardWidth; ++col) {
        EXPECT_FALSE(board.is_occupied({kBoardTotalHeight - 1, col}));
    }
}

TEST_F(BoardTest, ClearMultipleLines) {
    // fill bottom 2 rows
    for (int8_t row_offset = 0; row_offset < 2; ++row_offset) {
        for (int8_t col = 0; col < kBoardWidth; ++col) {
            board.lock({{kBoardTotalHeight - 1 - row_offset, col}},
                       TetrominoType::I);
        }
    }
    auto result = board.clear_lines();
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), 2);
}

TEST_F(BoardTest, Reset) {
    std::vector<Position> cells = {{0, 0}};
    board.lock(cells, TetrominoType::I);
    board.reset();
    EXPECT_FALSE(board.is_occupied({0, 0}));
}
