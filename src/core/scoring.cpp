#include "scoring.hpp"

namespace tetris {

void Scoring::add_line_clear(uint8_t count, uint32_t level) noexcept {
    constexpr uint32_t kScores[] = {0, 100, 300, 500, 800};
    if (count > 0 && count <= 4) {
        score_ += kScores[count] * level;
        total_lines_ += count;
        lines_this_level_ += count;
        update_level();
    }
}

void Scoring::add_soft_drop(uint32_t cells) noexcept {
    score_ += cells * 1;
}

void Scoring::add_hard_drop(uint32_t cells) noexcept {
    score_ += cells * 2;
}

void Scoring::reset() noexcept {
    score_ = 0;
    level_ = 1;
    total_lines_ = 0;
    lines_this_level_ = 0;
}

void Scoring::update_level() noexcept {
    level_ = 1 + total_lines_ / kLinesPerLevel;
}

}  // namespace tetris
