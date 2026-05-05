# 开发 TODO 清单

## 阶段 0：项目初始化 ✅

- [x] 0.1 初始化 git 仓库，创建 `.gitignore`
- [x] 0.2 创建项目目录结构 (`src/core`, `src/gui`, `tests`, `docs`, `scripts`)
- [x] 0.3 创建顶层 `CMakeLists.txt`（project, C++23, target 定义）
- [x] 0.4 创建 `CMakePresets.json`（debug, release, asan, ci-* 预设）
- [x] 0.5 配置 `.vscode/settings.json` 和 `.vscode/extensions.json`
- [x] 0.6 配置 `.clang-format`（基于 Google 风格）
- [x] 0.7 配置 `.clang-tidy`（C++23 规则集）
- [x] 0.8 创建 `scripts/format.sh`（格式化脚本）
- [x] 0.9 创建 `scripts/tidy.sh`（静态分析脚本）
- [x] 0.10 创建 `.github/workflows/ci.yml`（CI 流水线）
- [x] 0.11 验证构建骨架可编译（g++ 15.2.1 / Qt 6.10.3，零警告通过）

## 阶段 1：核心类型与工具 ✅

- [x] 1.1 实现 `types.hpp`（Position, TetrominoType, Rotation, Direction 等）
- [x] 1.2 为 Position 实现 `operator<=>` (`std::strong_ordering`)
- [x] 1.3 定义 GameError / BoardError 枚举
- [x] 1.4 创建 `src/core/CMakeLists.txt`，定义 `libtetris` target
- [x] 1.5 验证核心库编译通过

## 阶段 2：方块系统 ✅

- [x] 2.1 实现 `Tetromino` 类构造函数（使用 consteval 预计算形状矩阵）
- [x] 2.2 实现 `shape()` 方法，返回当前旋转状态的 `std::span`
- [x] 2.3 实现 `cells()` 方法，返回实际占据格的 `std::span`
- [x] 2.4 实现 `rotate_clockwise()` / `rotate_counter_clockwise()`
- [x] 2.5 实现 SRS 墙踢数据（consteval 预计算）
- [x] 2.6 编写 `tests/test_tetromino.cpp`：验证七种方块初始形状
- [x] 2.7 测试：验证旋转后形状正确性
- [x] 2.8 测试：验证墙踢偏移数据

## 阶段 3：面板逻辑 ✅

- [x] 3.1 实现 `Board` 类基本构造（24×10 网格，使用 `std::optional<TetrominoType>`）
- [x] 3.2 实现 `is_occupied()` / `is_in_bounds()` / `cell_at()`
- [x] 3.3 实现 `can_place()` 碰撞检测（使用 `std::ranges::all_of`）
- [x] 3.4 实现 `lock()` 方块锁定
- [x] 3.5 实现 `clear_lines()` 消行逻辑（使用 `std::expected` 返回）
- [x] 3.6 实现 `reset()` 面板重置
- [x] 3.7 编写 `tests/test_board.cpp`：测试空面板状态
- [x] 3.8 测试：碰撞检测（边界、占据）
- [x] 3.9 测试：锁定方块
- [x] 3.10 测试：消行（单行、多行、全满 Tetris）
- [x] 3.11 测试：面板重置

## 阶段 4：计分系统 ✅

- [x] 4.1 实现 `Scoring` 类基本结构
- [x] 4.2 实现 `add_line_clear()`（Single/Double/Triple/Tetris 分值）
- [x] 4.3 实现 `add_soft_drop()` / `add_hard_drop()`
- [x] 4.4 实现等级递进逻辑（10 行/级）和速度计算
- [x] 4.5 编写 `tests/test_scoring.cpp`：测试基础计分
- [x] 4.6 测试：各消行分值计算
- [x] 4.7 测试：软降/硬降计分
- [x] 4.8 测试：等级递增与速度变化

## 阶段 5：游戏状态机 ✅

- [x] 5.1 实现 `Game` 类构造函数（初始化棋盘、随机种子）
- [x] 5.2 实现方块生成 `spawn_piece()`（随机类型，使用 `std::mt19937`）
- [x] 5.3 实现 `process()`：移动、旋转、硬降、暂停/恢复/重新开始
- [x] 5.4 实现 `tick()`：自动下落、锁定检测、消行、生成新方块、GameOver 检测
- [x] 5.5 实现幽灵方块计算 `ghost_position()`
- [x] 5.6 实现状态转换逻辑（Ready→Playing↔Paused→GameOver）
- [x] 5.7 编写 `tests/test_game.cpp`：测试初始状态
- [x] 5.8 测试：方块移动（左/右/下）
- [x] 5.9 测试：旋转（含墙踢）
- [x] 5.10 测试：硬降锁定与生成新方块
- [x] 5.11 测试：GameOver 条件（出生阻塞）
- [x] 5.12 测试：暂停/恢复/重新开始
- [x] 5.13 测试：完整游戏流程（放置→消行→计分→等级递增→GameOver）
- [ ] 5.14 使用 lcov/gcov 检查核心代码覆盖率，目标 > 90%

## 阶段 6：GUI 实现 (骨架完成，渲染待完善)

- [x] 6.1 创建 `src/gui/CMakeLists.txt`，链接 `libtetris` 和 Qt6::Widgets
- [x] 6.2 实现 `main.cpp` 入口（QApplication + MainWindow）
- [x] 6.3 实现 `MainWindow`：布局管理（GameWidget + PreviewWidget + 信息面板）
- [ ] 6.4 实现 `GameWidget`：paintEvent 绘制当前方块、幽灵方块 ← 仅绘制网格和已锁定方块
- [x] 6.5 实现 `GameWidget`：keyPressEvent 键盘事件处理
- [ ] 6.6 实现 `PreviewWidget`：绘制下一个方块形状 ← 仅绘制背景
- [x] 6.7 配置 QTimer 游戏循环（动态更新间隔）
- [ ] 6.8 实现方块颜色方案（七种方块七种颜色）
- [x] 6.9 实现得分/等级/消行数实时更新显示
- [x] 6.10 实现暂停/重新开始按钮与流程
- [ ] 6.11 验证完整游戏可运行

## 阶段 7：CI 与自动化 (配置完成，待 CI 运行验证)

- [x] 7.1 配置 GitHub Actions CI 流水线
- [ ] 7.2 验证格式化检查 (`scripts/format.sh --check`) 在 CI 中通过
- [ ] 7.3 验证 Clang-Tidy 静态分析在 CI 中通过
- [ ] 7.4 验证 Debug/ASan 构建与测试在 CI 中通过
- [ ] 7.5 验证覆盖率报告生成与上传
- [ ] 7.6 添加 CI 状态徽章到 README.md

## 阶段 8：文档与收尾 (文档完成，收尾待做)

- [x] 8.1 完善 `README.md`（构建、运行、测试说明）
- [x] 8.2 完善 `docs/architecture.md`
- [x] 8.3 完善 `docs/api.md`
- [ ] 8.4 检查所有文件格式化一致
- [ ] 8.5 最终全量测试通过
- [ ] 8.6 提 PR / 提交最终版本
