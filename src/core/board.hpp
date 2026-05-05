#pragma once

#include <array>
#include <expected>
#include <optional>
#include <span>

#include "types.hpp"

namespace tetris {

class Board {
public:
    Board() = default;

    [[nodiscard]] bool is_occupied(Position pos) const noexcept;
    [[nodiscard]] static bool is_in_bounds(Position pos) noexcept;
    [[nodiscard]] std::optional<TetrominoType> cell_at(Position pos) const noexcept;

    [[nodiscard]] bool can_place(std::span<const Position> cells) const noexcept;

    void lock(std::span<const Position> cells, TetrominoType type);

    [[nodiscard]] std::expected<uint8_t, BoardError> clear_lines();

    void reset() noexcept;

    [[nodiscard]] const auto& grid() const noexcept { return grid_; }

private:
    std::array<std::array<std::optional<TetrominoType>, kBoardWidth>, kBoardTotalHeight> grid_{};
};

}  // namespace tetris
