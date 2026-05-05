#include "game.hpp"

#include <algorithm>
#include <ranges>

namespace tetris {

Game::Game() : state_(GameState::Ready) {}

std::expected<void, GameError> Game::process(GameAction action) {
    if (state_ != GameState::Playing && action != GameAction::Restart
        && action != GameAction::Pause) {
        return std::unexpected(GameError::InvalidState);
    }

    switch (action) {
    case GameAction::Restart:
        board_.reset();
        scoring_.reset();
        current_piece_.reset();
        next_type_.reset();
        state_ = GameState::Playing;
        spawn_piece();
        break;

    case GameAction::Pause:
        if (state_ == GameState::Playing) state_ = GameState::Paused;
        break;

    case GameAction::Resume:
        if (state_ == GameState::Paused) state_ = GameState::Playing;
        break;

    case GameAction::MoveLeft:
    case GameAction::MoveRight:
    case GameAction::SoftDrop:
    case GameAction::HardDrop:
    case GameAction::RotateCW:
    case GameAction::RotateCCW:
    case GameAction::Hold:
        if (state_ != GameState::Playing)
            return std::unexpected(GameError::InvalidState);
        // TODO: 实现移动/旋转逻辑
        break;
    }

    return {};
}

std::expected<void, GameError> Game::tick() {
    if (state_ != GameState::Playing) return {};
    // TODO: 实现自动下落逻辑
    return {};
}

std::vector<Position> Game::ghost_position() const noexcept {
    return {};
}

std::chrono::milliseconds Game::drop_interval() const noexcept {
    // 公式: max(50ms, 800ms - (level-1)*60ms)
    const int32_t ms = std::max(50, 800 - (static_cast<int32_t>(scoring_.level()) - 1) * 60);
    return std::chrono::milliseconds(ms);
}

void Game::spawn_piece() noexcept {
    if (!next_type_) next_type_ = random_type();
    current_piece_ = Tetromino(*next_type_);
    next_type_ = random_type();
}

void Game::lock_piece() noexcept {
    if (!current_piece_) return;
    board_.lock(current_piece_->cells(), current_piece_->type());

    auto result = board_.clear_lines();
    // TODO: 处理 result
    (void)result;

    spawn_piece();
}

TetrominoType Game::random_type() noexcept {
    static std::uniform_int_distribution<int> dist(0, 6);
    return static_cast<TetrominoType>(dist(rng_));
}

}  // namespace tetris
