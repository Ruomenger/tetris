#pragma once

#include <chrono>
#include <expected>
#include <optional>
#include <random>
#include <vector>

#include "board.hpp"
#include "scoring.hpp"
#include "tetromino.hpp"
#include "types.hpp"

namespace tetris {

class Game
{
public:
    Game();

    [[nodiscard]] constexpr GameState state() const noexcept { return state_; }
    [[nodiscard]] constexpr uint32_t score() const noexcept { return scoring_.score(); }
    [[nodiscard]] constexpr uint32_t level() const noexcept { return scoring_.level(); }
    [[nodiscard]] constexpr uint32_t lines_cleared() const noexcept
    {
        return scoring_.total_lines();
    }

    [[nodiscard]] std::expected<void, GameError> process(GameAction action);
    [[nodiscard]] std::expected<void, GameError> tick();

    [[nodiscard]] const Board& board() const noexcept { return board_; }
    [[nodiscard]] const std::optional<Tetromino>& current_piece() const noexcept
    {
        return current_piece_;
    }
    [[nodiscard]] std::optional<TetrominoType> next_piece() const noexcept { return next_type_; }
    [[nodiscard]] std::vector<Position> ghost_position() const noexcept;

    [[nodiscard]] std::chrono::milliseconds drop_interval() const noexcept;

    // 公开 for testing
    void set_rng_seed(uint32_t seed) noexcept;
    void set_piece_sequence(std::vector<TetrominoType> seq) noexcept;

private:
    void spawn_piece();
    void lock_piece();
    TetrominoType random_type();
    void start_game() noexcept;

    bool try_move(Direction dir) noexcept;
    bool try_rotate(bool clockwise) noexcept;
    void hard_drop();

    GameState state_{ GameState::Ready };
    Board board_;
    Scoring scoring_;
    std::optional<Tetromino> current_piece_;
    std::optional<TetrominoType> next_type_;
    std::mt19937 rng_{ std::random_device{}() };

    // 测试用：固定出块序列
    std::optional<std::vector<TetrominoType>> fixed_sequence_;
    size_t sequence_index_{ 0 };
};

}  // namespace tetris
