#include "game.hpp"

#include <algorithm>
#include <ranges>

namespace tetris {

Game::Game() = default;

// ── 公开接口 ────────────────────────────────────────────

std::expected<void, GameError> Game::process(GameAction action) {
    // 全局操作：任何时候都可以
    if (action == GameAction::Restart) {
        start_game();
        return {};
    }
    if (action == GameAction::Pause) {
        if (state_ == GameState::Playing) {
            state_ = GameState::Paused;
            return {};
        }
        return std::unexpected(GameError::InvalidState);
    }
    if (action == GameAction::Resume) {
        if (state_ == GameState::Paused) {
            state_ = GameState::Playing;
            return {};
        }
        return std::unexpected(GameError::InvalidState);
    }

    // 游戏操作：仅在 Playing 状态有效
    if (state_ != GameState::Playing) {
        return std::unexpected(GameError::InvalidState);
    }

    switch (action) {
        case GameAction::MoveLeft:
            if (!try_move(Direction::Left))
                return std::unexpected(GameError::MoveBlocked);
            break;
        case GameAction::MoveRight:
            if (!try_move(Direction::Right))
                return std::unexpected(GameError::MoveBlocked);
            break;
        case GameAction::SoftDrop:
            if (try_move(Direction::Down)) {
                scoring_.add_soft_drop(1);
            } else {
                return std::unexpected(GameError::MoveBlocked);
            }
            break;
        case GameAction::HardDrop:
            hard_drop();
            break;
        case GameAction::RotateCW:
            if (!try_rotate(true))
                return std::unexpected(GameError::MoveBlocked);
            break;
        case GameAction::RotateCCW:
            if (!try_rotate(false))
                return std::unexpected(GameError::MoveBlocked);
            break;
        default:
            break;
    }

    return {};
}

std::expected<void, GameError> Game::tick() {
    if (state_ != GameState::Playing)
        return {};

    if (!current_piece_.has_value())
        return {};

    // 尝试下落一行
    if (try_move(Direction::Down))
        return {};

    // 无法下落 → 锁定
    lock_piece();
    return {};
}

std::vector<Position> Game::ghost_position() const noexcept {
    if (!current_piece_.has_value())
        return {};

    // 从当前位置一直向下移动直到碰撞
    Tetromino ghost(*current_piece_);
    while (true) {
        auto next = ghost.moved_cells(Direction::Down);
        if (board_.can_place(next)) {
            ghost.set_position(
                { static_cast<int8_t>(ghost.position().row + 1), ghost.position().col });
        } else {
            break;
        }
    }
    return std::vector<Position>(ghost.cells().begin(), ghost.cells().end());
}

std::chrono::milliseconds Game::drop_interval() const noexcept {
    const int32_t ms = std::max(50, 800 - ((static_cast<int32_t>(scoring_.level()) - 1) * 60));
    return std::chrono::milliseconds(ms);
}

void Game::set_rng_seed(uint32_t seed) noexcept {
    rng_.seed(seed);
}

void Game::set_piece_sequence(std::vector<TetrominoType> seq) noexcept {
    fixed_sequence_ = std::move(seq);
    sequence_index_ = 0;
}

// ── 私有方法 ────────────────────────────────────────────

void Game::start_game() noexcept {
    board_.reset();
    scoring_.reset();
    current_piece_.reset();
    next_type_.reset();
    state_ = GameState::Playing;
    sequence_index_ = 0;
    spawn_piece();
}

void Game::spawn_piece() {
    if (!next_type_.has_value()) {
        next_type_ = random_type();
    }
    current_piece_ = Tetromino(*next_type_);
    next_type_ = random_type();

    // 新方块出生位置：缓冲区顶部，水平居中
    // 检测 GameOver：新方块无法放置
    if (current_piece_.has_value() && !board_.can_place(current_piece_->cells())) {
        current_piece_.reset();
        state_ = GameState::GameOver;
    }
}

void Game::lock_piece() {
    if (!current_piece_.has_value())
        return;

    const auto& piece = *current_piece_;
    board_.lock(piece.cells(), piece.type());

    auto clear_result = board_.clear_lines();
    if (clear_result.has_value() && clear_result.value() > 0) {
        scoring_.add_line_clear(clear_result.value(), scoring_.level());
    }

    current_piece_.reset();
    spawn_piece();
}

TetrominoType Game::random_type() {
    if (fixed_sequence_.has_value() && !fixed_sequence_->empty()) {
        auto t = (*fixed_sequence_)[sequence_index_ % fixed_sequence_->size()];
        sequence_index_ = (sequence_index_ + 1) % fixed_sequence_->size();
        return t;
    }
    static std::uniform_int_distribution<int> dist(0, 6);
    return static_cast<TetrominoType>(dist(rng_));
}

bool Game::try_move(Direction dir) noexcept {
    if (!current_piece_.has_value())
        return false;
    auto new_cells = current_piece_->moved_cells(dir);
    if (board_.can_place(new_cells)) {
        auto pos = current_piece_->position();
        switch (dir) {
            case Direction::Left:
                pos.col = static_cast<int8_t>(pos.col - 1);
                break;
            case Direction::Right:
                pos.col = static_cast<int8_t>(pos.col + 1);
                break;
            case Direction::Down:
                pos.row = static_cast<int8_t>(pos.row + 1);
                break;
        }
        current_piece_->set_position(pos);
        return true;
    }
    return false;
}

bool Game::try_rotate(bool clockwise) noexcept {
    if (!current_piece_.has_value()) {
        return false;
    }

    auto& piece = *current_piece_;
    const int fi = static_cast<int>(piece.rotation());
    const int ti = clockwise ? (fi + 1) % 4 : (fi + 3) % 4;
    const auto new_rot = static_cast<Rotation>(ti);

    auto kicks = piece.wall_kick_offsets(piece.rotation(), new_rot);

    for (const auto& kick : kicks) {
        auto kicked_cells = piece.kicked_cells(new_rot, kick);
        if (board_.can_place(kicked_cells)) {
            auto pos = piece.position();
            pos.row = static_cast<int8_t>(pos.row + kick.row);
            pos.col = static_cast<int8_t>(pos.col + kick.col);
            current_piece_->set_position(pos);
            if (clockwise) {
                current_piece_->rotate_clockwise();
            } else {
                current_piece_->rotate_counter_clockwise();
            }
            return true;
        }
    }
    return false;
}

void Game::hard_drop() {
    if (!current_piece_.has_value())
        return;

    uint32_t dropped = 0;
    while (try_move(Direction::Down)) {
        ++dropped;
    }
    scoring_.add_hard_drop(dropped);
    lock_piece();
}

}  // namespace tetris
