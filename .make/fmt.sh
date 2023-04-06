#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/../mojave" && pwd)"
SRC_DIR="$ROOT_DIR/src"
TEST_DIR="$ROOT_DIR/tests"

if ! command -v clang-format &>/dev/null; then
	echo "clang-format not found — skipping fmt"
	exit 0
fi

find "$SRC_DIR" "$TEST_DIR" \( -name '*.cpp' -o -name '*.hpp' \) -print0 | xargs -0 clang-format -i
echo "Formatted"
