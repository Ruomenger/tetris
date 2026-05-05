#pragma once

#include <array>
#include <compare>
#include <cstdint>

namespace tetris {

enum class TetrominoType : uint8_t {
    I, O, T, S, Z, J, L
};

enum class Rotation : uint8_t {
    R0, R90, R180, R270
};

enum class Direction : uint8_t {
    Left, Right, Down
};

struct Position {
    int8_t row;
    int8_t col;
    auto operator<=>(const Position&) const = default;
};

enum class GameState {
    Ready,
    Playing,
    Paused,
    GameOver
};

enum class GameAction {
    MoveLeft,
    MoveRight,
    SoftDrop,
    HardDrop,
    RotateCW,
    RotateCCW,
    Hold,
    Pause,
    Resume,
    Restart
};

enum class GameError : uint8_t {
    InvalidState,
    MoveBlocked,
    GameOver
};

enum class BoardError : uint8_t {
    Collision,
    InvalidPosition
};

static constexpr int8_t kBoardWidth  = 10;
static constexpr int8_t kBoardVisibleHeight = 20;
static constexpr int8_t kBoardBufferHeight  = 4;
static constexpr int8_t kBoardTotalHeight = kBoardVisibleHeight + kBoardBufferHeight;

}  // namespace tetris
