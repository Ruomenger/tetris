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

class Game {
public:
    Game();

    [[nodiscard]] constexpr GameState state() const noexcept { return state_; }
    [[nodiscard]] constexpr uint32_t score() const noexcept { return scoring_.score(); }
    [[nodiscard]] constexpr uint32_t level() const noexcept { return scoring_.level(); }
    [[nodiscard]] constexpr uint32_t lines_cleared() const noexcept {
        return scoring_.total_lines();
    }

    [[nodiscard]] std::expected<void, GameError> process(GameAction action);
    [[nodiscard]] std::expected<void, GameError> tick();

    [[nodiscard]] const Board& board() const noexcept { return board_; }
    [[nodiscard]] const std::optional<Tetromino>& current_piece() const noexcept {
        return current_piece_;
    }
    [[nodiscard]] std::optional<TetrominoType> next_piece() const noexcept {
        return next_type_;
    }
    [[nodiscard]] std::vector<Position> ghost_position() const noexcept;

    [[nodiscard]] std::chrono::milliseconds drop_interval() const noexcept;

private:
    void spawn_piece() noexcept;
    void lock_piece() noexcept;
    TetrominoType random_type() noexcept;

    GameState state_{GameState::Ready};
    Board board_;
    Scoring scoring_;
    std::optional<Tetromino> current_piece_;
    std::optional<TetrominoType> next_type_;
    std::mt19937 rng_{std::random_device{}()};
};

}  // namespace tetris
