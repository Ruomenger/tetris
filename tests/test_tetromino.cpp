#include <gtest/gtest.h>

#include "tetromino.hpp"

using namespace tetris;

class TetrominoTest : public ::testing::Test
{
protected:
    void SetUp() override {}
};

TEST_F(TetrominoTest, CreateITetromino)
{
    Tetromino t(TetrominoType::I);
    EXPECT_EQ(t.type(), TetrominoType::I);
    EXPECT_EQ(t.rotation(), Rotation::R0);
    EXPECT_EQ(t.position().row, 0);
    EXPECT_EQ(t.position().col, 3);
    EXPECT_FALSE(t.cells().empty());
    EXPECT_EQ(t.cells().size(), 4);
}

TEST_F(TetrominoTest, CreateAllTypes)
{
    for (auto type : { TetrominoType::I, TetrominoType::O, TetrominoType::T, TetrominoType::S,
                       TetrominoType::Z, TetrominoType::J, TetrominoType::L }) {
        Tetromino t(type);
        EXPECT_EQ(t.type(), type);
        EXPECT_EQ(t.rotation(), Rotation::R0);
        EXPECT_EQ(t.cells().size(), 4);
    }
}

TEST_F(TetrominoTest, RotateClockwise)
{
    Tetromino t(TetrominoType::T);
    auto cells_before = std::vector<Position>(t.cells().begin(), t.cells().end());

    t.rotate_clockwise();
    EXPECT_EQ(t.rotation(), Rotation::R90);

    auto cells_after = std::vector<Position>(t.cells().begin(), t.cells().end());
    EXPECT_NE(cells_before, cells_after);
}

TEST_F(TetrominoTest, RotateFullCircleCW)
{
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

TEST_F(TetrominoTest, RotateFullCircleCCW)
{
    Tetromino t(TetrominoType::T);
    auto cells_before = std::vector<Position>(t.cells().begin(), t.cells().end());

    t.rotate_counter_clockwise();
    t.rotate_counter_clockwise();
    t.rotate_counter_clockwise();
    t.rotate_counter_clockwise();
    EXPECT_EQ(t.rotation(), Rotation::R0);

    auto cells_after = std::vector<Position>(t.cells().begin(), t.cells().end());
    EXPECT_EQ(cells_before, cells_after);
}

TEST_F(TetrominoTest, SetPosition)
{
    Tetromino t(TetrominoType::I);
    t.set_position({ 5, 2 });
    EXPECT_EQ(t.position().row, 5);
    EXPECT_EQ(t.position().col, 2);
    EXPECT_EQ(t.cells().size(), 4);
}

TEST_F(TetrominoTest, MovedCells)
{
    Tetromino t(TetrominoType::I);
    t.set_position({ 10, 3 });

    auto moved = t.moved_cells(Direction::Left);
    EXPECT_EQ(moved.size(), 4);
    // I piece at R0 occupies cols 0-3 in shape → pos {10,3} → cols {3,4,5,6}
    // after move left: cols {2,3,4,5}
    int min_col = 100;
    for (auto& p : moved)
        min_col = std::min(min_col, static_cast<int>(p.col));
    EXPECT_EQ(min_col, 2);

    auto down = t.moved_cells(Direction::Down);
    int min_row = 100;
    for (auto& p : down)
        min_row = std::min(min_row, static_cast<int>(p.row));
    EXPECT_EQ(min_row, 12);  // I at {10,3}: cells at row 11; down → row 12

    auto right = t.moved_cells(Direction::Right);
    int max_col = 0;
    for (auto& p : right)
        max_col = std::max(max_col, static_cast<int>(p.col));
    EXPECT_EQ(max_col, 7);  // I at {10,3}: cells at cols 3-6; right → cols 4-7
}

TEST_F(TetrominoTest, RotatedCells)
{
    Tetromino t(TetrominoType::I);
    t.set_position({ 10, 3 });

    auto cw = t.rotated_cells(true);
    EXPECT_EQ(cw.size(), 4);

    auto ccw = t.rotated_cells(false);
    EXPECT_EQ(ccw.size(), 4);

    // I 在 R0 和 R180 的 cells 应该相同（水平→垂直→水平）
    Tetromino t2(TetrominoType::I);
    t2.set_position({ 10, 3 });
    auto r0_cells = std::vector<Position>(t2.cells().begin(), t2.cells().end());
    t2.rotate_clockwise();
    t2.rotate_clockwise();
    auto r180_cells = std::vector<Position>(t2.cells().begin(), t2.cells().end());
    EXPECT_EQ(r0_cells.size(), r180_cells.size());
    // R0 是水平, R180 也是水平，但相对于原点位置略有不同
}

TEST_F(TetrominoTest, WallKickOffsetsHaveFiveTests)
{
    Tetromino t(TetrominoType::T);
    auto kicks = t.wall_kick_offsets(Rotation::R0, Rotation::R90);
    EXPECT_EQ(kicks.size(), 5);
    EXPECT_EQ(kicks[0].row, 0);
    EXPECT_EQ(kicks[0].col, 0);
}

TEST_F(TetrominoTest, IWallKickDiffersFromJLSZT)
{
    Tetromino i(TetrominoType::I);
    Tetromino t(TetrominoType::T);

    auto i_kicks = i.wall_kick_offsets(Rotation::R0, Rotation::R90);
    auto t_kicks = t.wall_kick_offsets(Rotation::R0, Rotation::R90);

    // I 的第二个测试偏移与 JLSZT 不同
    bool any_diff = false;
    for (size_t idx = 0; idx < 5; ++idx) {
        if (i_kicks[idx].row != t_kicks[idx].row || i_kicks[idx].col != t_kicks[idx].col) {
            any_diff = true;
            break;
        }
    }
    EXPECT_TRUE(any_diff);
}

TEST_F(TetrominoTest, OWallKicksAllZero)
{
    Tetromino o(TetrominoType::O);
    for (auto from : { Rotation::R0, Rotation::R90, Rotation::R180, Rotation::R270 }) {
        for (auto to : { Rotation::R0, Rotation::R90, Rotation::R180, Rotation::R270 }) {
            auto kicks = o.wall_kick_offsets(from, to);
            for (auto& k : kicks) {
                EXPECT_EQ(k.row, 0);
                EXPECT_EQ(k.col, 0);
            }
        }
    }
}

TEST_F(TetrominoTest, StaticCellsFor)
{
    auto cells = Tetromino::cells_for(TetrominoType::O, Rotation::R0, { 5, 5 });
    EXPECT_EQ(cells.size(), 4);
    // O 方块在 R0: 占据 (1,1), (1,2), (2,1), (2,2) 相对
    // 所以绝对位置: (6,6), (6,7), (7,6), (7,7)
    EXPECT_EQ(cells[0].row, 6);
    EXPECT_EQ(cells[0].col, 6);
}
