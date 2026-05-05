#!/usr/bin/env bash
#
# tidy.sh — Clang-Tidy 静态分析脚本
#
# 用法：
#   bash scripts/tidy.sh              # 自动检测 build 目录
#   bash scripts/tidy.sh -d <dir>     # 指定构建目录
#   bash scripts/tidy.sh --check      # 仅核心 src/core/ (快速)
#   bash scripts/tidy.sh --fix        # 自动修复
#   bash scripts/tidy.sh --help       # 帮助

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_DIR="$(dirname "$SCRIPT_DIR")"

BUILD_DIR="${PROJECT_DIR}/build/debug"

show_help() {
    cat <<EOF
用法: bash scripts/tidy.sh [选项]

选项:
  (无参数)         自动检测 build/ 下 compile_commands.json
  -d <dir>         指定构建目录 (含 compile_commands.json)
  --check          快速模式 — 仅分析 src/core/
  --fix            自动修复可修复的问题
  --help           显示帮助

前置: 需先运行 cmake --preset debug 或 cmake --preset ci-debug
EOF
    exit 0
}

if [ "${1:-}" == "--help" ]; then
    show_help
fi

if [ "${1:-}" == "-d" ]; then
    shift
    BUILD_DIR="${1}"
    shift || true
fi

# 自动检测 compile_commands.json
if [ ! -f "${BUILD_DIR}/compile_commands.json" ]; then
    DB=$(find "${PROJECT_DIR}/build" -maxdepth 3 -name compile_commands.json 2>/dev/null | head -1)
    if [ -n "$DB" ]; then
        BUILD_DIR="$(dirname "$DB")"
        echo "[tidy] 自动检测: ${BUILD_DIR}"
    else
        echo "[tidy] compile_commands.json 不存在，请先运行: cmake --preset debug"
        exit 1
    fi
fi

FIX_FLAG=""
FILE_FILTER=".*\.(cpp|hpp)$"

if [ "${1:-}" == "--fix" ]; then
    FIX_FLAG="-fix"
elif [ "${1:-}" == "--check" ]; then
    FILE_FILTER=".*src/core/.*\.(cpp|hpp)$"
fi

NPROC=$(nproc 2>/dev/null || echo 4)
echo "[tidy] 目录: ${BUILD_DIR}, ${NPROC} 并行, 过滤器: ${FILE_FILTER}"

if ! run-clang-tidy \
    -p "$BUILD_DIR" \
    -header-filter='.*src/core/.*' \
    -j "$NPROC" \
    -quiet \
    "$FILE_FILTER" \
    $FIX_FLAG; then
    echo "[tidy] 发现问题 (详见上方输出)"
    exit 1
fi

echo "[tidy] 无问题"
