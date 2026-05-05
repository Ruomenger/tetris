# 架构设计文档

## 1. 总体架构

```
┌─────────────────────────────────────────────────┐
│                  src/gui (Qt)                    │
│  MainWindow ── GameWidget ── PreviewWidget      │
│       │              │                           │
│       └──────┬───────┘                           │
│              │  QTimer    QPainter               │
│              ▼                                    │
├─────────────────────────────────────────────────┤
│                src/core (libtetris)              │
│  Game ──── Board ──── Tetromino                  │
│   │          │                                    │
│   Scoring    Position                            │
│              Collision                            │
└─────────────────────────────────────────────────┘
```

核心逻辑 (`libtetris`) 是纯 C++ 库，不依赖 Qt；GUI 层通过组合方式使用核心库，通过 QTimer 驱动 tick，通过 QPainter 渲染画面。

## 2. 核心模块 (libtetris)

### 2.1 类型定义 (`types.hpp`)

```cpp
// 方块类型枚举
enum class TetrominoType : uint8_t { I, O, T, S, Z, J, L };

// 旋转状态
enum class Rotation : uint8_t { R0, R90, R180, R270 };

// 方向
enum class Direction : uint8_t { Left, Right, Down };

// 移动结果
enum class MoveResult { Success, Collision, OutOfBounds };

// 二维坐标
struct Position {
    int8_t row;
    int8_t col;
    auto operator<=>(const Position&) const = default;
};

// 4x4 方块形状矩阵
using ShapeMatrix = std::array<std::array<bool, 4>, 4>;
using ShapeSpan   = std::span<const std::array<bool, 4>, 4>;
```

### 2.2 方块系统 (`tetromino.hpp/.cpp`)

- **职责**：定义七种方块的形状数据与旋转行为
- **设计要点**：
  - 使用 `consteval` 预计算所有方块在四种旋转状态下的形状矩阵
  - 旋转采用 SRS (Super Rotation System) 标准
  - 每种方块存储其旋转状态下的 4×4 布尔矩阵
  - 提供墙踢 (Wall Kick) 偏移数据

```cpp
class Tetromino {
public:
    explicit constexpr Tetromino(TetrominoType type) noexcept;

    [[nodiscard]] constexpr TetrominoType type() const noexcept;
    [[nodiscard]] constexpr Rotation rotation() const noexcept;
    [[nodiscard]] ShapeSpan shape() const noexcept;  // 当前旋转形状
    [[nodiscard]] std::span<const Position> cells() const noexcept; // 占用格

    void rotate_clockwise() noexcept;
    void rotate_counter_clockwise() noexcept;

    // SRS 墙踢偏移
    [[nodiscard]] std::span<const Position> wall_kick_offsets(
        Rotation from, Rotation to) const noexcept;

private:
    TetrominoType type_;
    Rotation rotation_{Rotation::R0};
    std::vector<Position> cells_; // 当前旋转下的占据格
};
```

### 2.3 面板逻辑 (`board.hpp/.cpp`)

- **职责**：10×20 网格管理，碰撞检测，消行
- **使用 `std::expected`** 作为操作返回类型

```cpp
class Board {
public:
    static constexpr int8_t kWidth  = 10;
    static constexpr int8_t kHeight = 20;
    static constexpr int8_t kVisibleHeight = 20;    // 可见区域
    static constexpr int8_t kBufferHeight  = 4;     // 顶部缓冲区
    static constexpr int8_t kTotalHeight   = kVisibleHeight + kBufferHeight;

    Board() = default;

    // 查询
    [[nodiscard]] bool is_occupied(Position pos) const noexcept;
    [[nodiscard]] bool is_in_bounds(Position pos) const noexcept;
    [[nodiscard]] std::optional<TetrominoType> cell_at(Position pos) const noexcept;

    // 碰撞检测
    [[nodiscard]] bool can_place(
        std::span<const Position> cells) const noexcept;

    // 锁定方块
    void lock(std::span<const Position> cells, TetrominoType type);

    // 消行：返回消除行数及行索引
    [[nodiscard]] std::expected<uint8_t, BoardError> clear_lines();

    // 重置
    void reset() noexcept;

    // 视图
    [[nodiscard]] const auto& grid() const noexcept { return grid_; }

private:
    // 使用 optional 标记空格/占用，24 行（含缓冲区）
    std::array<std::array<std::optional<TetrominoType>, kWidth>, kTotalHeight> grid_{};
};
```

### 2.4 游戏状态机 (`game.hpp/.cpp`)

- **职责**：游戏流程控制、状态管理、积分系统

```cpp
enum class GameState { Ready, Playing, Paused, GameOver };
enum class GameAction { MoveLeft, MoveRight, SoftDrop, HardDrop,
                        RotateCW, RotateCCW, Hold, Pause, Resume, Restart };

class Game {
public:
    Game();

    // 状态查询
    [[nodiscard]] constexpr GameState state() const noexcept;
    [[nodiscard]] constexpr uint32_t score() const noexcept;
    [[nodiscard]] constexpr uint32_t level() const noexcept;
    [[nodiscard]] constexpr uint32_t lines_cleared() const noexcept;

    // 操作（返回 std::expected 表达成功/失败）
    [[nodiscard]] std::expected<void, GameError> process(GameAction action);
    [[nodiscard]] std::expected<void, GameError> tick(); // 每帧调用

    // 查询数据供 GUI 渲染
    [[nodiscard]] const Board& board() const noexcept;
    [[nodiscard]] const Tetromino& current_piece() const noexcept;
    [[nodiscard]] std::optional<TetrominoType> next_piece() const noexcept;
    [[nodiscard]] std::vector<Position> ghost_position() const noexcept;

    // 下落间隔 (ms)
    [[nodiscard]] std::chrono::milliseconds drop_interval() const noexcept;

private:
    void spawn_piece();
    uint32_t calculate_score(uint8_t lines) const noexcept;

    GameState   state_{GameState::Ready};
    Board       board_;
    Scoring     scoring_;
    std::optional<Tetromino> current_piece_;
    std::optional<TetrominoType> next_type_;
    std::mt19937 rng_;
};
```

### 2.5 计分系统 (`scoring.hpp/.cpp`)

```cpp
class Scoring {
public:
    Scoring() noexcept;

    void add_line_clear(uint8_t count, uint32_t level) noexcept;
    void add_soft_drop(uint32_t cells) noexcept;
    void add_hard_drop(uint32_t cells) noexcept;

    [[nodiscard]] constexpr uint32_t score() const noexcept;
    [[nodiscard]] constexpr uint32_t level() const noexcept;
    [[nodiscard]] constexpr uint32_t total_lines() const noexcept;

    void reset() noexcept;

private:
    uint32_t score_{0};
    uint32_t level_{1};
    uint32_t total_lines_{0};
    uint32_t lines_for_next_level_{10};

    // 经典计分规则（NES 标准）：
    // Single:   100 × level
    // Double:   300 × level
    // Triple:   500 × level
    // Tetris:   800 × level
    // SoftDrop:   1 × cell
    // HardDrop:   2 × cell
};
```

## 3. GUI 模块 (Qt Widgets)

### 3.1 主窗口 (`MainWindow`)

继承 `QMainWindow`，布局包含：
- `GameWidget` — 核心游戏绘制区域
- `PreviewWidget` — 下一个方块预览
- 分数/等级/行数标签 (QLabel)
- 控制面板（开始/暂停/重新开始按钮）

### 3.2 游戏绘制 (`GameWidget`)

- 继承 `QWidget`，重写 `paintEvent`
- 使用 `QPainter` 绘制游戏面板、当前方块、幽灵方块
- 固定绘制区域尺寸（按 10:20 比例）
- 捕捉键盘事件 (`keyPressEvent`) 转发给 `Game::process`

### 3.3 预览绘制 (`PreviewWidget`)

- 继承 `QWidget`，使用 `QPainter` 绘制下一个方块

## 4. 游戏循环

```cpp
// 使用 QTimer 驱动
timer_ = new QTimer(this);
connect(timer_, &QTimer::timeout, this, [this] {
    if (auto result = game_.tick(); !result) {
        // 处理 GameOver
    }
    update(); // 触发重绘
});
timer_->start(game_.drop_interval());
```

## 5. C++23 特性应用

| 特性 | 应用位置 |
|------|----------|
| `std::expected` | `Game::process()`, `Game::tick()`, `Board::clear_lines()` |
| `std::optional` | 棋盘单元格、当前方块、下一个方块类型 |
| `std::span` | 方块占据格列表、形状矩阵视图 |
| `std::format` | 日志/调试输出、状态字符串格式化 |
| `consteval` | 预计算方块形状矩阵和墙踢数据 |
| `std::ranges` | 遍历棋盘行、方块格、消行检测 |
| concepts | 约束模板参数（如渲染器接口） |
| `auto operator<=>` | `Position` 类型三路比较 |

## 6. 测试策略

- **核心逻辑** 100% 单元测试覆盖（Board、Tetromino、Game、Scoring）
- **GUI 层** 不做单元测试（纯渲染/事件处理，测试价值低）
- 使用 GTest，通过 `gcov`/`lcov` 生成覆盖率报告
- CMake 中启用 `--coverage` 标志，通过 `ctest` 运行
