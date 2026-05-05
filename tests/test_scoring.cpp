#include <gtest/gtest.h>

#include "scoring.hpp"

using namespace tetris;

class ScoringTest : public ::testing::Test {
protected:
    Scoring scoring;
};

TEST_F(ScoringTest, InitialState) {
    EXPECT_EQ(scoring.score(), 0);
    EXPECT_EQ(scoring.level(), 1);
    EXPECT_EQ(scoring.total_lines(), 0);
}

TEST_F(ScoringTest, SingleLineClear) {
    scoring.add_line_clear(1, 1);
    EXPECT_EQ(scoring.score(), 100);
    EXPECT_EQ(scoring.total_lines(), 1);
}

TEST_F(ScoringTest, DoubleLineClear) {
    scoring.add_line_clear(2, 1);
    EXPECT_EQ(scoring.score(), 300);
}

TEST_F(ScoringTest, TripleLineClear) {
    scoring.add_line_clear(3, 1);
    EXPECT_EQ(scoring.score(), 500);
}

TEST_F(ScoringTest, TetrisLineClear) {
    scoring.add_line_clear(4, 1);
    EXPECT_EQ(scoring.score(), 800);
}

TEST_F(ScoringTest, ScoreMultipliedByLevel) {
    scoring.add_line_clear(1, 5);
    EXPECT_EQ(scoring.score(), 500);
}

TEST_F(ScoringTest, SoftDrop) {
    scoring.add_soft_drop(5);
    EXPECT_EQ(scoring.score(), 5);
}

TEST_F(ScoringTest, HardDrop) {
    scoring.add_hard_drop(5);
    EXPECT_EQ(scoring.score(), 10);
}

TEST_F(ScoringTest, LevelProgression) {
    // clear 10 single lines to go from level 1 to level 2
    for (int i = 0; i < 10; ++i) {
        scoring.add_line_clear(1, scoring.level());
    }
    EXPECT_EQ(scoring.level(), 2);
    EXPECT_EQ(scoring.total_lines(), 10);
}

TEST_F(ScoringTest, Reset) {
    scoring.add_line_clear(4, 1);
    scoring.reset();
    EXPECT_EQ(scoring.score(), 0);
    EXPECT_EQ(scoring.level(), 1);
    EXPECT_EQ(scoring.total_lines(), 0);
}
