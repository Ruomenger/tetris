#include <gtest/gtest.h>

#include "game.hpp"

using namespace tetris;

class GameTest : public ::testing::Test {
protected:
    Game game;
};

TEST_F(GameTest, InitialState) {
    EXPECT_EQ(game.state(), GameState::Ready);
    EXPECT_EQ(game.score(), 0);
    EXPECT_EQ(game.level(), 1);
    EXPECT_EQ(game.lines_cleared(), 0);
}

TEST_F(GameTest, Restart) {
    auto result = game.process(GameAction::Restart);
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(game.state(), GameState::Playing);
    EXPECT_TRUE(game.current_piece().has_value());
    EXPECT_TRUE(game.next_piece().has_value());
}

TEST_F(GameTest, PauseFromPlaying) {
    game.process(GameAction::Restart);
    auto result = game.process(GameAction::Pause);
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(game.state(), GameState::Paused);
}

TEST_F(GameTest, ResumeFromPaused) {
    game.process(GameAction::Restart);
    game.process(GameAction::Pause);
    auto result = game.process(GameAction::Resume);
    EXPECT_TRUE(result.has_value());
    EXPECT_EQ(game.state(), GameState::Playing);
}

TEST_F(GameTest, CannotPauseFromReady) {
    auto result = game.process(GameAction::Pause);
    EXPECT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), GameError::InvalidState);
}

TEST_F(GameTest, DropInterval) {
    EXPECT_GT(game.drop_interval().count(), 0);
}
