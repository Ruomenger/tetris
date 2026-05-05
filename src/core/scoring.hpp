#pragma once

#include <chrono>
#include <cstdint>

namespace tetris {

class Scoring {
public:
    Scoring() noexcept = default;

    void add_line_clear(uint8_t count, uint32_t level) noexcept;
    void add_soft_drop(uint32_t cells) noexcept;
    void add_hard_drop(uint32_t cells) noexcept;

    [[nodiscard]] constexpr uint32_t score() const noexcept { return score_; }
    [[nodiscard]] constexpr uint32_t level() const noexcept { return level_; }
    [[nodiscard]] constexpr uint32_t total_lines() const noexcept { return total_lines_; }

    void reset() noexcept;

private:
    static constexpr uint32_t kLinesPerLevel = 10;

    void update_level() noexcept;

    uint32_t score_{ 0 };
    uint32_t level_{ 1 };
    uint32_t total_lines_{ 0 };
    uint32_t lines_this_level_{ 0 };
};

}  // namespace tetris
