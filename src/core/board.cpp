#include "board.hpp"

#include <algorithm>
#include <ranges>

namespace tetris {

bool Board::is_occupied(Position pos) const noexcept
{
    if (!is_in_bounds(pos))
        return false;
    return grid_[static_cast<size_t>(pos.row)][static_cast<size_t>(pos.col)].has_value();
}

bool Board::is_in_bounds(Position pos) noexcept
{
    return pos.col >= 0 && pos.col < kBoardWidth && pos.row >= 0 && pos.row < kBoardTotalHeight;
}

std::optional<TetrominoType> Board::cell_at(Position pos) const noexcept
{
    if (!is_in_bounds(pos))
        return std::nullopt;
    return grid_[static_cast<size_t>(pos.row)][static_cast<size_t>(pos.col)];
}

bool Board::can_place(std::span<const Position> cells) const noexcept
{
    return std::ranges::all_of(cells,
                               [this](Position p) { return is_in_bounds(p) && !is_occupied(p); });
}

void Board::lock(std::span<const Position> cells, TetrominoType type)
{
    for (auto p : cells) {
        grid_[static_cast<size_t>(p.row)][static_cast<size_t>(p.col)] = type;
    }
}

std::expected<uint8_t, BoardError> Board::clear_lines()
{
    uint8_t cleared = 0;
    for (int8_t row = kBoardTotalHeight - 1; row >= 0; --row) {
        const bool full = std::ranges::all_of(grid_[static_cast<size_t>(row)],
                                              [](const auto& cell) { return cell.has_value(); });

        if (full) {
            // 向下移动所有上面的行
            for (int8_t r = row; r > 0; --r) {
                grid_[static_cast<size_t>(r)] = grid_[static_cast<size_t>(r - 1)];
            }
            // 清空顶行
            grid_[0].fill(std::nullopt);
            ++cleared;
            ++row;  // 重新检查当前行
        }
    }
    return cleared;
}

void Board::reset() noexcept
{
    for (auto& row : grid_) {
        row.fill(std::nullopt);
    }
}

}  // namespace tetris
