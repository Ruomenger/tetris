#include "tetromino.hpp"

#include <array>
#include <cstddef>

#include "types.hpp"

namespace tetris {

// ── 经典七种方块的初始形状 (R0)，4×4 矩阵 ─────────────

static constexpr std::array<ShapeMatrix, 7> kInitialShapes = { {
    // I
    { {
        { { false, false, false, false } },
        { { true, true, true, true } },
        { { false, false, false, false } },
        { { false, false, false, false } },
    } },
    // O
    { {
        { { false, false, false, false } },
        { { false, true, true, false } },
        { { false, true, true, false } },
        { { false, false, false, false } },
    } },
    // T
    { {
        { { false, false, false, false } },
        { { false, true, false, false } },
        { { true, true, true, false } },
        { { false, false, false, false } },
    } },
    // S
    { {
        { { false, false, false, false } },
        { { false, true, true, false } },
        { { true, true, false, false } },
        { { false, false, false, false } },
    } },
    // Z
    { {
        { { false, false, false, false } },
        { { true, true, false, false } },
        { { false, true, true, false } },
        { { false, false, false, false } },
    } },
    // J
    { {
        { { false, false, false, false } },
        { { true, false, false, false } },
        { { true, true, true, false } },
        { { false, false, false, false } },
    } },
    // L
    { {
        { { false, false, false, false } },
        { { false, false, true, false } },
        { { true, true, true, false } },
        { { false, false, false, false } },
    } },
} };

// ── consteval 预计算全部 28 种形状 ────────────────────

namespace {

consteval auto build_shape_table() -> std::array<ShapeMatrix, 28>
{
    std::array<ShapeMatrix, 28> table{};
    for (size_t t = 0; t < 7; ++t) {
        auto mat = kInitialShapes[t];
        for (int r = 0; r < 4; ++r) {
            table[(t * 4) + static_cast<size_t>(r)] = mat;
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

namespace {

constexpr size_t shape_idx(TetrominoType t, Rotation r) noexcept
{
    return (static_cast<size_t>(t) * 4) + static_cast<size_t>(r);
}

}  // anonymous namespace

// ── SRS 墙踢偏移表 ─────────────────────────────────────

namespace {

using Kick5 = std::array<Position, 5>;

// J/L/S/Z/T CW 偏移 (0→1, 1→2, 2→3, 3→0)
constexpr std::array<Kick5, 4> kJlsztCW = { {
    { { { 0, 0 }, { -1, 0 }, { -1, 1 }, { 0, -2 }, { -1, -2 } } },  // 0→1
    { { { 0, 0 }, { 1, 0 }, { 1, -1 }, { 0, 2 }, { 1, 2 } } },      // 1→2
    { { { 0, 0 }, { 1, 0 }, { 1, 1 }, { 0, -2 }, { 1, -2 } } },     // 2→3
    { { { 0, 0 }, { -1, 0 }, { -1, -1 }, { 0, 2 }, { -1, 2 } } },   // 3→0
} };

// I CW 偏移
constexpr std::array<Kick5, 4> kICW = { {
    { { { 0, 0 }, { -2, 0 }, { 1, 0 }, { -2, -1 }, { 1, 2 } } },  // 0→1
    { { { 0, 0 }, { -1, 0 }, { 2, 0 }, { -1, 2 }, { 2, -1 } } },  // 1→2
    { { { 0, 0 }, { 2, 0 }, { -1, 0 }, { 2, 1 }, { -1, -2 } } },  // 2→3
    { { { 0, 0 }, { 1, 0 }, { -2, 0 }, { 1, -2 }, { -2, 1 } } },  // 3→0
} };

constexpr Kick5 kZero5{ { { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 } } };

// 取反
constexpr Kick5 negate_kicks(const Kick5& k)
{
    return { { { static_cast<int8_t>(-k[0].row), static_cast<int8_t>(-k[0].col) },
               { static_cast<int8_t>(-k[1].row), static_cast<int8_t>(-k[1].col) },
               { static_cast<int8_t>(-k[2].row), static_cast<int8_t>(-k[2].col) },
               { static_cast<int8_t>(-k[3].row), static_cast<int8_t>(-k[3].col) },
               { static_cast<int8_t>(-k[4].row), static_cast<int8_t>(-k[4].col) } } };
}

// CW 表 [type][from_rot] → 5 个偏移 (仅 CW 方向)
const auto& cw_kick_table(TetrominoType type) noexcept
{
    if (type == TetrominoType::I)
        return kICW;
    if (type == TetrominoType::O) {
        static constexpr std::array<Kick5, 4> oz = { kZero5, kZero5, kZero5, kZero5 };
        return oz;
    }
    return kJlsztCW;
}

}  // anonymous namespace

// ── 辅助：从形状矩阵提取占据格 ─────────────────────────

namespace {

std::vector<Position> extract_cells(const ShapeMatrix& mat, Position origin) noexcept
{
    std::vector<Position> cells;
    cells.reserve(4);
    for (int8_t y = 0; y < 4; ++y) {
        for (int8_t x = 0; x < 4; ++x) {
            if (mat[static_cast<size_t>(y)][static_cast<size_t>(x)]) {
                cells.push_back(
                    { static_cast<int8_t>(origin.row + y), static_cast<int8_t>(origin.col + x) });
            }
        }
    }
    return cells;
}

}  // anonymous namespace

// ── 公共接口 ───────────────────────────────────────────

Tetromino::Tetromino(TetrominoType type) noexcept : type_(type)
{
    update_cells();
}

ShapeSpan Tetromino::shape() const noexcept
{
    return std::span{ kShapeTable[shape_idx(type_, rotation_)] };
}

void Tetromino::set_position(Position pos) noexcept
{
    pos_ = pos;
    update_cells();
}

void Tetromino::rotate_clockwise() noexcept
{
    rotation_ = static_cast<Rotation>((static_cast<int>(rotation_) + 1) % 4);
    update_cells();
}

void Tetromino::rotate_counter_clockwise() noexcept
{
    rotation_ = static_cast<Rotation>((static_cast<int>(rotation_) + 3) % 4);
    update_cells();
}

void Tetromino::update_cells() noexcept
{
    cells_ = extract_cells(kShapeTable[shape_idx(type_, rotation_)], pos_);
}

std::span<const Position> Tetromino::wall_kick_offsets(Rotation from, Rotation to) const noexcept
{
    if (type_ == TetrominoType::O)
        return { kZero5 };

    const int fi = static_cast<int>(from);
    const int ti = static_cast<int>(to);

    if (ti == (fi + 1) % 4) {
        // CW
        return { cw_kick_table(type_)[static_cast<size_t>(fi)] };
    }
    if (ti == (fi + 3) % 4) {
        // CCW = 反向 CW 的负偏移
        // 从 to 到 from 是 CW，其偏移取反即从 from 到 to 的 CCW
        const auto rev = static_cast<size_t>(ti);
        static const auto& tbl = cw_kick_table(type_);
        // 使用 static 缓存负偏移结果
        // 注意：constexpr 函数在不同调用点可能产生不同值
        // 这里简单处理：在运行时计算负偏移
        thread_local static Kick5 cached;
        const auto& cw = tbl[rev];
        for (size_t i = 0; i < 5; ++i) {
            cached[i] = { static_cast<int8_t>(-cw[i].row), static_cast<int8_t>(-cw[i].col) };
        }
        return { cached };
    }
    return { kZero5 };
}

std::vector<Position> Tetromino::moved_cells(Direction dir) const noexcept
{
    int8_t dr = 0;
    int8_t dc = 0;
    switch (dir) {
        case Direction::Left:
            dc = -1;
            break;
        case Direction::Right:
            dc = 1;
            break;
        case Direction::Down:
            dr = 1;
            break;
    }
    const Position new_pos{ static_cast<int8_t>(pos_.row + dr),
                            static_cast<int8_t>(pos_.col + dc) };
    return extract_cells(kShapeTable[shape_idx(type_, rotation_)], new_pos);
}

std::vector<Position> Tetromino::rotated_cells(bool clockwise) const noexcept
{
    const int fi = static_cast<int>(rotation_);
    const int ti = clockwise ? (fi + 1) % 4 : (fi + 3) % 4;
    const auto new_rot = static_cast<Rotation>(ti);
    return extract_cells(kShapeTable[shape_idx(type_, new_rot)], pos_);
}

std::vector<Position> Tetromino::kicked_cells(Rotation rot, Position kick) const noexcept
{
    const Position new_pos{ static_cast<int8_t>(pos_.row + kick.row),
                            static_cast<int8_t>(pos_.col + kick.col) };
    return extract_cells(kShapeTable[shape_idx(type_, rot)], new_pos);
}

std::vector<Position> Tetromino::cells_for(TetrominoType type, Rotation rot,
                                           Position origin) noexcept
{
    return extract_cells(kShapeTable[shape_idx(type, rot)], origin);
}

}  // namespace tetris
