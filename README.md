# Tetris — C++23 / Qt 6.10 俄罗斯方块

经典俄罗斯方块桌面游戏，使用 C++23 标准与 Qt 6.10 Widgets 模块构建。

## 特性

- 经典七种方块 (I/O/T/S/Z/J/L) + SRS 旋转系统
- 消行、计分、等级递增、下落加速
- 幽灵方块预览、下一个方块预览
- 暂停/恢复/重新开始
- 核心逻辑与 GUI 严格分离，可独立测试
- 单元测试覆盖核心逻辑 (GTest)
- CI 集成：构建、测试、格式化检查、Clang-Tidy、Sanitizer、覆盖率

## 系统要求

| 组件 | 最低版本 |
|------|----------|
| CMake | 3.25+ |
| GCC | 14+ 或 Clang 18+ |
| Qt | 6.10+ (Widgets 模块) |
| GTest | 1.14+ |

支持平台：Linux (x86_64/arm64) / macOS (x86_64/arm64)。

## 快速开始

```bash
# 克隆仓库
git clone <repo-url> && cd tetris

# 配置 (Debug)
cmake --preset debug

# 构建
cmake --build --preset debug

# 运行
./build/debug/bin/tetris

# 测试
ctest --preset debug

# 格式化
bash scripts/format.sh

# 静态分析
bash scripts/tidy.sh
```

## CMake Presets

| Preset | 说明 |
|--------|------|
| `debug` | Debug 构建，带 AddressSanitizer |
| `release` | Release 构建 |
| `asan` | AddressSanitizer 专用 |
| `ci-debug` | CI Debug（无 Sanitizer） |
| `ci-asan` | CI ASan 构建 |
| `ci-coverage` | CI 覆盖率构建 |

## 项目结构

```
tetris/
├── CMakeLists.txt              # 顶层 CMake
├── CMakePresets.json           # CMake 预设
├── .clang-format               # 格式化规则
├── .clang-tidy                 # 静态分析规则
├── .gitignore
├── .vscode/
│   ├── settings.json
│   └── extensions.json
├── .github/
│   └── workflows/
│       └── ci.yml              # GitHub Actions CI
├── scripts/
│   ├── format.sh               # 格式化脚本
│   └── tidy.sh                 # 静态分析脚本
├── docs/
│   ├── architecture.md         # 架构设计文档
│   └── api.md                  # API 说明
├── TODO.md                     # 开发 TODO 清单
├── src/
│   ├── CMakeLists.txt
│   ├── core/                   # 核心逻辑库 (libtetris)
│   │   ├── CMakeLists.txt
│   │   ├── types.hpp           # 基础类型定义
│   │   ├── tetromino.hpp/.cpp  # 方块定义与旋转
│   │   ├── board.hpp/.cpp      # 游戏面板逻辑
│   │   ├── game.hpp/.cpp       # 游戏状态机
│   │   └── scoring.hpp/.cpp    # 计分与等级
│   └── gui/                    # Qt GUI
│       ├── CMakeLists.txt
│       ├── main.cpp            # 入口
│       ├── main_window.hpp/.cpp
│       ├── game_widget.hpp/.cpp
│       └── preview_widget.hpp/.cpp
└── tests/
    ├── CMakeLists.txt
    ├── test_board.cpp
    ├── test_tetromino.cpp
    ├── test_game.cpp
    └── test_scoring.cpp
```

## 技术栈

- **C++23**：`std::expected`、`std::optional`、`std::span`、`std::format`、`consteval`、`std::ranges`、concepts
- **Qt 6.10 Widgets**：QWidget + QPainter 渲染，QTimer 驱动游戏循环
- **CMake**：target-based 现代 CMake，CMakePresets.json
- **clang-format / clang-tidy**：代码格式化与静态分析
- **GTest**：单元测试 & 覆盖率
- **GitHub Actions**：CI/CD
