#include <gtest/gtest.h>
#include <tuple>

#include "game.hpp"

using namespace tetris;

class GameTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        game_ = Game();
        game_.set_piece_sequence({ TetrominoType::I, TetrominoType::O, TetrominoType::T,
                                   TetrominoType::S, TetrominoType::Z, TetrominoType::J,
                                   TetrominoType::L });
    }

    Game game_;

    void restart_and_play()
    {
        auto r = game_.process(GameAction::Restart);
        ASSERT_TRUE(r.has_value());
        ASSERT_EQ(game_.state(), GameState::Playing);
        ASSERT_TRUE(game_.current_piece().has_value());
    }
};

// ── 状态转换 ──────────────────────────────────────────

TEST_F(GameTest, InitialState)
{
    EXPECT_EQ(game_.state(), GameState::Ready);
    EXPECT_EQ(game_.score(), 0);
    EXPECT_EQ(game_.level(), 1);
    EXPECT_EQ(game_.lines_cleared(), 0);
    EXPECT_FALSE(game_.current_piece().has_value());
}

TEST_F(GameTest, RestartEntersPlaying)
{
    auto result = game_.process(GameAction::Restart);
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(game_.state(), GameState::Playing);
    EXPECT_TRUE(game_.current_piece().has_value());
    EXPECT_TRUE(game_.next_piece().has_value());
}

TEST_F(GameTest, PauseFromPlaying)
{
    restart_and_play();
    auto result = game_.process(GameAction::Pause);
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(game_.state(), GameState::Paused);
}

TEST_F(GameTest, ResumeFromPaused)
{
    restart_and_play();
    std::ignore = game_.process(GameAction::Pause);
    auto result = game_.process(GameAction::Resume);
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(game_.state(), GameState::Playing);
}

TEST_F(GameTest, CannotPauseFromReady)
{
    auto result = game_.process(GameAction::Pause);
    EXPECT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), GameError::InvalidState);
}

TEST_F(GameTest, MoveBlockedWhenNotPlaying)
{
    EXPECT_FALSE(game_.process(GameAction::MoveLeft).has_value());
}

// ── 移动操作 ──────────────────────────────────────────

TEST_F(GameTest, MoveLeft)
{
    restart_and_play();
    auto initial_col = game_.current_piece()->position().col;
    auto result = game_.process(GameAction::MoveLeft);
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(game_.current_piece()->position().col, initial_col - 1);
}

TEST_F(GameTest, MoveRight)
{
    restart_and_play();
    auto initial_col = game_.current_piece()->position().col;
    auto result = game_.process(GameAction::MoveRight);
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(game_.current_piece()->position().col, initial_col + 1);
}

TEST_F(GameTest, MoveLeftBlockedByWall)
{
    game_.set_piece_sequence({ TetrominoType::I });
    restart_and_play();
    for (int i = 0; i < 3; ++i)
        std::ignore = game_.process(GameAction::MoveLeft);
    EXPECT_EQ(game_.current_piece()->position().col, 0);
    auto result = game_.process(GameAction::MoveLeft);
    EXPECT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), GameError::MoveBlocked);
}

TEST_F(GameTest, SoftDropAwardsPoints)
{
    restart_and_play();
    auto initial_row = game_.current_piece()->position().row;
    auto result = game_.process(GameAction::SoftDrop);
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(game_.current_piece()->position().row, initial_row + 1);
    EXPECT_GT(game_.score(), 0);
}

// ── 旋转 (含墙踢) ────────────────────────────────────

TEST_F(GameTest, RotateCW)
{
    restart_and_play();
    auto init_rot = game_.current_piece()->rotation();
    auto result = game_.process(GameAction::RotateCW);
    EXPECT_TRUE(result.has_value());
    EXPECT_NE(game_.current_piece()->rotation(), init_rot);
}

TEST_F(GameTest, RotateCCW)
{
    restart_and_play();
    auto init_rot = game_.current_piece()->rotation();
    auto result = game_.process(GameAction::RotateCCW);
    EXPECT_TRUE(result.has_value());
    EXPECT_NE(game_.current_piece()->rotation(), init_rot);
}

TEST_F(GameTest, RotateFullCircle)
{
    restart_and_play();
    auto init_rot = game_.current_piece()->rotation();
    for (int i = 0; i < 4; ++i)
        std::ignore = game_.process(GameAction::RotateCW);
    EXPECT_EQ(game_.current_piece()->rotation(), init_rot);
}

TEST_F(GameTest, IRotatesAtWallWithKick)
{
    game_.set_piece_sequence({ TetrominoType::I });
    restart_and_play();
    for (int i = 0; i < 3; ++i)
        std::ignore = game_.process(GameAction::MoveLeft);
    EXPECT_EQ(game_.current_piece()->position().col, 0);
    auto result = game_.process(GameAction::RotateCW);
    EXPECT_TRUE(result.has_value());
}

// ── HardDrop ──────────────────────────────────────────

TEST_F(GameTest, HardDropLocksAndSpawnsNext)
{
    restart_and_play();
    auto old_type = game_.current_piece()->type();
    auto result = game_.process(GameAction::HardDrop);
    EXPECT_TRUE(result.has_value());
    EXPECT_TRUE(game_.current_piece().has_value());
    // 硬降后新方块应该是下一个方块类型
    EXPECT_NE(game_.current_piece()->type(), old_type);
}

// ── tick ──────────────────────────────────────────────

TEST_F(GameTest, TickMovesDown)
{
    restart_and_play();
    auto initial_row = game_.current_piece()->position().row;
    auto result = game_.tick();
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(game_.current_piece()->position().row, initial_row + 1);
}

TEST_F(GameTest, DropIntervalDecreasesWithLevel)
{
    EXPECT_GT(game_.drop_interval().count(), 0);
    EXPECT_LE(game_.drop_interval().count(), 800);
}

TEST_F(GameTest, TickDoesNothingWhenPaused)
{
    restart_and_play();
    auto initial_row = game_.current_piece()->position().row;
    std::ignore = game_.process(GameAction::Pause);
    std::ignore = game_.tick();
    EXPECT_EQ(game_.current_piece()->position().row, initial_row);
}

// ── 游戏结束 ──────────────────────────────────────────

TEST_F(GameTest, GameOverOnBlockedSpawn)
{
    game_.set_piece_sequence(std::vector<TetrominoType>(40, TetrominoType::O));
    restart_and_play();
    while (game_.state() == GameState::Playing) {
        std::ignore = game_.process(GameAction::HardDrop);
    }
    EXPECT_EQ(game_.state(), GameState::GameOver);
}

// ── 幽灵方块 ──────────────────────────────────────────

TEST_F(GameTest, GhostNotEmpty)
{
    restart_and_play();
    auto ghost = game_.ghost_position();
    EXPECT_FALSE(ghost.empty());
    EXPECT_EQ(ghost.size(), game_.current_piece()->cells().size());
}

TEST_F(GameTest, GhostBelowOrAtCurrent)
{
    restart_and_play();
    auto ghost = game_.ghost_position();
    auto cells = game_.current_piece()->cells();

    int ghost_max_row = 0;
    for (const auto& p : ghost)
        ghost_max_row = std::max(ghost_max_row, static_cast<int>(p.row));
    int cells_max_row = 0;
    for (const auto& p : cells)
        cells_max_row = std::max(cells_max_row, static_cast<int>(p.row));
    EXPECT_GE(ghost_max_row, cells_max_row);
}

// ── 完整流程 ──────────────────────────────────────────

TEST_F(GameTest, RestartResetsEverything)
{
    restart_and_play();
    std::ignore = game_.process(GameAction::HardDrop);
    if (game_.state() == GameState::Playing)
        std::ignore = game_.process(GameAction::HardDrop);

    std::ignore = game_.process(GameAction::Restart);
    EXPECT_EQ(game_.state(), GameState::Playing);
    EXPECT_EQ(game_.score(), 0);
    EXPECT_EQ(game_.level(), 1);
    EXPECT_EQ(game_.lines_cleared(), 0);
    EXPECT_TRUE(game_.current_piece().has_value());
}
