#include "tetromino.hpp"

namespace tetris {

// 经典七种方块的初始形状 (R0)，4×4 矩阵
static constexpr std::array<ShapeMatrix, 7> kInitialShapes = {{
    // I
    {{
        {{0, 0, 0, 0}},
        {{1, 1, 1, 1}},
        {{0, 0, 0, 0}},
        {{0, 0, 0, 0}},
    }},
    // O
    {{
        {{0, 0, 0, 0}},
        {{0, 1, 1, 0}},
        {{0, 1, 1, 0}},
        {{0, 0, 0, 0}},
    }},
    // T
    {{
        {{0, 0, 0, 0}},
        {{0, 1, 0, 0}},
        {{1, 1, 1, 0}},
        {{0, 0, 0, 0}},
    }},
    // S
    {{
        {{0, 0, 0, 0}},
        {{0, 1, 1, 0}},
        {{1, 1, 0, 0}},
        {{0, 0, 0, 0}},
    }},
    // Z
    {{
        {{0, 0, 0, 0}},
        {{1, 1, 0, 0}},
        {{0, 1, 1, 0}},
        {{0, 0, 0, 0}},
    }},
    // J
    {{
        {{0, 0, 0, 0}},
        {{1, 0, 0, 0}},
        {{1, 1, 1, 0}},
        {{0, 0, 0, 0}},
    }},
    // L
    {{
        {{0, 0, 0, 0}},
        {{0, 0, 1, 0}},
        {{1, 1, 1, 0}},
        {{0, 0, 0, 0}},
    }},
}};

namespace {

consteval auto build_shape_table() -> std::array<ShapeMatrix, 28> {
    std::array<ShapeMatrix, 28> table{};
    for (size_t type = 0; type < 7; ++type) {
        auto mat = kInitialShapes[type];
        for (int rot = 0; rot < 4; ++rot) {
            table[type * 4 + static_cast<size_t>(rot)] = mat;
            ShapeMatrix next{};
            for (int y = 0; y < 4; ++y) {
                for (int x = 0; x < 4; ++x) {
                    if (mat[static_cast<size_t>(y)][static_cast<size_t>(x)]) {
                        next[static_cast<size_t>(x)][static_cast<size_t>(3 - y)] = true;
                    }
                }
            }
            mat = next;
        }
    }
    return table;
}

}  // anonymous namespace

static constexpr auto kShapeTable = build_shape_table();

static constexpr size_t shape_index(TetrominoType t, Rotation r) noexcept {
    return static_cast<size_t>(t) * 4 + static_cast<size_t>(r);
}

Tetromino::Tetromino(TetrominoType type) noexcept
    : type_(type), cells_(4) {
    update_cells();
}

ShapeSpan Tetromino::shape() const noexcept {
    return std::span{kShapeTable[shape_index(type_, rotation_)]};
}

void Tetromino::rotate_clockwise() noexcept {
    rotation_ = static_cast<Rotation>(
        (static_cast<int>(rotation_) + 1) % 4);
    update_cells();
}

void Tetromino::rotate_counter_clockwise() noexcept {
    rotation_ = static_cast<Rotation>(
        (static_cast<int>(rotation_) + 3) % 4);
    update_cells();
}

std::span<const Position> Tetromino::wall_kick_offsets(
    Rotation /*from*/, Rotation /*to*/) const noexcept {
    // TODO: SRS 墙踢数据
    static const std::vector<Position> empty{};
    return empty;
}

void Tetromino::update_cells() noexcept {
    cells_.clear();
    const auto& mat = kShapeTable[shape_index(type_, rotation_)];
    for (int8_t y = 0; y < 4; ++y) {
        for (int8_t x = 0; x < 4; ++x) {
            if (mat[static_cast<size_t>(y)][static_cast<size_t>(x)]) {
                cells_.push_back(
                    {static_cast<int8_t>(pos_.row + y),
                     static_cast<int8_t>(pos_.col + x)});
            }
        }
    }
}

}  // namespace tetris
