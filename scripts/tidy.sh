#!/usr/bin/env bash
#
# tidy.sh — Clang-Tidy 静态分析脚本
#
# 用法：
#   bash scripts/tidy.sh              # 分析所有源码 (并行)
#   bash scripts/tidy.sh --fix        # 自动修复
#   bash scripts/tidy.sh --check      # 仅核心 src/core/ (快速)
#   bash scripts/tidy.sh --help       # 帮助

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_DIR="$(dirname "$SCRIPT_DIR")"

COMPILE_DB_DIR="${PROJECT_DIR}/build/debug"

show_help() {
    cat <<EOF
用法: bash scripts/tidy.sh [选项]

选项:
  (无参数)        分析所有 src/ 和 tests/ 源文件 (并行)
  --fix           自动修复可修复的问题
  --check         快速模式 — 仅分析 src/core/ (核心逻辑)
  --help          显示帮助

前置: 需先运行 cmake --preset debug 生成 compile_commands.json
EOF
    exit 0
}

if [ "${1:-}" == "--help" ]; then
    show_help
fi

if [ ! -f "${COMPILE_DB_DIR}/compile_commands.json" ]; then
    echo "[tidy] compile_commands.json 不存在，先执行: cmake --preset debug"
    exit 1
fi

FIX_FLAG=""
FILE_FILTER=".*\.(cpp|hpp)$"

if [ "${1:-}" == "--fix" ]; then
    FIX_FLAG="-fix"
elif [ "${1:-}" == "--check" ]; then
    FILE_FILTER=".*src/core/.*\.(cpp|hpp)$"
fi

NPROC=$(nproc 2>/dev/null || echo 4)
echo "[tidy] 使用 $NPROC 并行任务，过滤器: $FILE_FILTER"

if ! run-clang-tidy \
    -p "$COMPILE_DB_DIR" \
    -header-filter='.*src/core/.*' \
    -j "$NPROC" \
    -quiet \
    "$FILE_FILTER" \
    $FIX_FLAG; then
    echo "[tidy] 发现问题 (详见上方输出)"
    exit 1
fi

echo "[tidy] 无问题"
