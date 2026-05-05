#pragma once

#include <span>
#include <vector>

#include "types.hpp"

namespace tetris {

using ShapeMatrix = std::array<std::array<bool, 4>, 4>;
using ShapeSpan   = std::span<const std::array<bool, 4>, 4>;

class Tetromino {
public:
    Tetromino(TetrominoType type) noexcept;

    [[nodiscard]] constexpr TetrominoType type() const noexcept { return type_; }
    [[nodiscard]] constexpr Rotation rotation() const noexcept { return rotation_; }
    [[nodiscard]] ShapeSpan shape() const noexcept;
    [[nodiscard]] std::span<const Position> cells() const noexcept { return cells_; }

    void rotate_clockwise() noexcept;
    void rotate_counter_clockwise() noexcept;

    [[nodiscard]] std::span<const Position> wall_kick_offsets(
        Rotation from, Rotation to) const noexcept;

private:
    void update_cells() noexcept;

    TetrominoType type_;
    Rotation rotation_{Rotation::R0};
    Position pos_{3, 3};                    // 出生位置
    std::vector<Position> cells_;            // 当前占据格
};

}  // namespace tetris
