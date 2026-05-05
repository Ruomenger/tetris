# API 说明

## 命名空间

所有核心逻辑位于 `namespace tetris` 中。

## 核心类型 (`tetris::`)

### TetrominoType

```cpp
enum class TetrominoType : uint8_t {
    I, O, T, S, Z, J, L
};
```

### Rotation

```cpp
enum class Rotation : uint8_t {
    R0, R90, R180, R270
};
```

### Direction

```cpp
enum class Direction : uint8_t {
    Left, Right, Down
};
```

### MoveResult

```cpp
enum class MoveResult {
    Success,
    Collision,
    OutOfBounds
};
```

### Position

```cpp
struct Position {
    int8_t row;
    int8_t col;
    auto operator<=>(const Position&) const = default;
};
```

### GameState

```cpp
enum class GameState {
    Ready,
    Playing,
    Paused,
    GameOver
};
```

### GameAction

```cpp
enum class GameAction {
    MoveLeft, MoveRight, SoftDrop, HardDrop,
    RotateCW, RotateCCW, Hold,
    Pause, Resume, Restart
};
```

### GameError / BoardError

```cpp
enum class GameError {
    InvalidState,     // 当前状态下不允许该操作
    MoveBlocked,      // 移动被方块/边界阻挡
    GameOver          // 游戏结束
};

enum class BoardError {
    Collision,        // 放置位置有冲突
    InvalidPosition   // 坐标越界
};
```

---

## Board 类

游戏面板核心类，管理 10×20 网格、碰撞检测和消行。

### 构造函数

```cpp
Board();
```

### 查询方法

```cpp
bool is_occupied(Position pos) const noexcept;
```
返回指定位置是否已被锁定方块占据。

```cpp
bool is_in_bounds(Position pos) const noexcept;
```
返回指定位置是否在面板范围内（含缓冲区）。

```cpp
std::optional<TetrominoType> cell_at(Position pos) const noexcept;
```
返回指定位置的方块类型，未占据时返回 `std::nullopt`。

### 碰撞与锁定

```cpp
bool can_place(std::span<const Position> cells) const noexcept;
```
检测给定格子列表是否可以放置在当前位置。所有格子必须在范围内且未被占据时返回 `true`。

```cpp
void lock(std::span<const Position> cells, TetrominoType type);
```
将方块锁定到面板中。调用前应确保 `can_place()` 返回 `true`。

### 消行

```cpp
std::expected<uint8_t, BoardError> clear_lines();
```
检测并消除所有已填满的行，返回消除行数。失败时返回 `BoardError`。

### 工具方法

```cpp
void reset() noexcept;
```
清空面板，恢复初始状态。

---

## Tetromino 类

方块类，描述单一方块的形状与旋转。

### 构造函数

```cpp
explicit constexpr Tetromino(TetrominoType type) noexcept;
```

### 属性

```cpp
constexpr TetrominoType type() const noexcept;
constexpr Rotation rotation() const noexcept;
ShapeSpan shape() const noexcept;  // 返回 4×4 矩阵视图
std::span<const Position> cells() const noexcept;  // 占用格列表
```

### 旋转

```cpp
void rotate_clockwise() noexcept;
void rotate_counter_clockwise() noexcept;
```

### SRS 墙踢

```cpp
std::span<const Position> wall_kick_offsets(Rotation from, Rotation to) const noexcept;
```
返回从 `from` 旋转到 `to` 的墙踢偏移序列。测试每个偏移量直到找到可放置位置。

---

## Game 类

游戏状态机，封装完整游戏逻辑。

### 构造函数

```cpp
Game();
```

### 状态查询

```cpp
constexpr GameState state() const noexcept;
constexpr uint32_t score() const noexcept;
constexpr uint32_t level() const noexcept;
constexpr uint32_t lines_cleared() const noexcept;
const Board& board() const noexcept;
const Tetromino& current_piece() const noexcept;
std::optional<TetrominoType> next_piece() const noexcept;
std::vector<Position> ghost_position() const noexcept;
std::chrono::milliseconds drop_interval() const noexcept;
```

### 游戏操作

```cpp
std::expected<void, GameError> process(GameAction action);
```
处理用户输入（移动、旋转、暂停等）。失败时返回 `GameError`。

```cpp
std::expected<void, GameError> tick();
```
每帧调用，推动游戏逻辑（下落、锁定、消行、生成新方块）。失败时表示游戏结束。

---

## Scoring 类

计分与等级系统。

```cpp
Scoring() noexcept;

void add_line_clear(uint8_t count, uint32_t level) noexcept;
void add_soft_drop(uint32_t cells) noexcept;
void add_hard_drop(uint32_t cells) noexcept;

constexpr uint32_t score() const noexcept;
constexpr uint32_t level() const noexcept;
constexpr uint32_t total_lines() const noexcept;

void reset() noexcept;
```

### 计分规则

| 操作 | 分值 |
|------|------|
| 单消 (Single) | 100 × level |
| 双消 (Double) | 300 × level |
| 三消 (Triple) | 500 × level |
| 四消 (Tetris) | 800 × level |
| 软降 (Soft Drop) | 1 × 下降格数 |
| 硬降 (Hard Drop) | 2 × 下降格数 |

### 等级

每消除 10 行升一级。等级越高，下落速度越快：

```cpp
// 下落间隔 (ms) = max(50, 800 - (level - 1) * 60)
```

---

## GUI 类 (Qt)

### MainWindow : QMainWindow

主窗口，组装所有 GUI 组件。

```cpp
class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);
private:
    Game game_;
    GameWidget* game_widget_;
    PreviewWidget* preview_widget_;
    QLabel* score_label_;
    QLabel* level_label_;
    QLabel* lines_label_;
    QTimer* timer_;
};
```

### GameWidget : QWidget

游戏面板绘制控件。

```cpp
class GameWidget : public QWidget {
    Q_OBJECT
public:
    explicit GameWidget(Game& game, QWidget* parent = nullptr);

    static constexpr int kCellSize = 30; // 每格像素
    QSize sizeHint() const override;

protected:
    void paintEvent(QPaintEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;

private:
    void draw_grid(QPainter& p);
    void draw_locked_cells(QPainter& p);
    void draw_current_piece(QPainter& p);
    void draw_ghost_piece(QPainter& p);
    void draw_border(QPainter& p);

    Game& game_;
};
```

### PreviewWidget : QWidget

下一个方块预览控件。

```cpp
class PreviewWidget : public QWidget {
    Q_OBJECT
public:
    explicit PreviewWidget(Game& game, QWidget* parent = nullptr);

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    Game& game_;
    static constexpr int kCellSize = 20;
};
```
