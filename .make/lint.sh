#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/../mojave" && pwd)"
SRC_DIR="$ROOT_DIR/src"

if ! command -v cppcheck &>/dev/null; then
	echo "cppcheck not found — skipping lint"
	exit 0
fi

cppcheck --std=c++20 --enable=all --suppress=missingInclude "$SRC_DIR"
