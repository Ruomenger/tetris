#!/usr/bin/env bash
#
# tidy.sh — Clang-Tidy 静态分析脚本
#
# 用法：
#   bash scripts/tidy.sh              # 对编译数据库中的文件执行静态分析
#   bash scripts/tidy.sh --fix        # 自动修复可修复的问题
#   bash scripts/tidy.sh <file>       # 仅分析指定文件

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_DIR="$(dirname "$SCRIPT_DIR")"

COMPILE_DB="${PROJECT_DIR}/build/debug/compile_commands.json"

if [ ! -f "$COMPILE_DB" ]; then
    echo "[tidy] compile_commands.json not found. Run cmake --preset debug first."
    echo "[tidy] You may need to run: cmake --preset debug -B build/debug"
    exit 1
fi

FIX_MODE=""
if [ "${1:-}" == "--fix" ]; then
    FIX_MODE="-fix"
    shift || true
fi

# 可选：指定单个文件
if [ $# -ge 1 ]; then
    FILES=("$@")
else
    FILES=$(find "${PROJECT_DIR}/src" "${PROJECT_DIR}/tests" \
        -type f \( -name '*.cpp' -o -name '*.hpp' \) 2>/dev/null || true)
fi

if [ -z "$FILES" ]; then
    echo "[tidy] No files to analyze."
    exit 0
fi

echo "[tidy] Running Clang-Tidy ..."
EXIT_CODE=0

for file in $FILES; do
    echo "  Analyzing: $file"
    if ! run-clang-tidy -p "${PROJECT_DIR}/build/debug" \
        -header-filter='.*src/.*' \
        $FIX_MODE \
        "$file" 2>&1; then
        echo "  [ISSUES] $file"
        EXIT_CODE=1
    else
        echo "  [OK] $file"
    fi
done

if [ $EXIT_CODE -eq 0 ]; then
    echo "[tidy] No issues found."
else
    echo "[tidy] Issues detected."
fi

exit $EXIT_CODE
