#pragma once

#include <array>
#include <span>
#include <vector>

#include "types.hpp"

namespace tetris {

using ShapeMatrix = std::array<std::array<bool, 4>, 4>;
using ShapeSpan = std::span<const std::array<bool, 4>, 4>;

class Tetromino
{
public:
    explicit Tetromino(TetrominoType type) noexcept;

    [[nodiscard]] constexpr TetrominoType type() const noexcept { return type_; }
    [[nodiscard]] constexpr Rotation rotation() const noexcept { return rotation_; }
    [[nodiscard]] constexpr Position position() const noexcept { return pos_; }
    [[nodiscard]] ShapeSpan shape() const noexcept;
    [[nodiscard]] std::span<const Position> cells() const noexcept { return cells_; }

    void set_position(Position pos) noexcept;

    void rotate_clockwise() noexcept;
    void rotate_counter_clockwise() noexcept;

    // SRS 墙踢偏移表: from → to 的 5 个测试偏移
    [[nodiscard]] std::span<const Position> wall_kick_offsets(Rotation from,
                                                              Rotation to) const noexcept;

    // 非变异的查询: 返回移动/旋转后的占据格，用于 try-before-commit
    [[nodiscard]] std::vector<Position> moved_cells(Direction dir) const noexcept;
    [[nodiscard]] std::vector<Position> rotated_cells(bool clockwise) const noexcept;
    [[nodiscard]] std::vector<Position> kicked_cells(Rotation rot, Position kick) const noexcept;

    // 纯形状查询: 给定旋转态和原点，返回占据格
    [[nodiscard]] static std::vector<Position> cells_for(TetrominoType type, Rotation rot,
                                                         Position origin) noexcept;

private:
    void update_cells() noexcept;

    TetrominoType type_;
    Rotation rotation_{ Rotation::R0 };
    Position pos_{ 0, 3 };         // 出生位置 (顶部缓冲区, 水平居中)
    std::vector<Position> cells_;  // 当前占据格
};

}  // namespace tetris
