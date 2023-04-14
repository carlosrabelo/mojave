#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/../mojave" && pwd)"
SRC_DIR="$ROOT_DIR/src"
CXX="${CXX:-g++}"
CXXFLAGS="-std=c++20 -Wall -Wextra -Wpedantic -I$SRC_DIR"

echo "=== Syntax check ==="
find "$SRC_DIR" -name '*.cpp' -print0 | xargs -0 -I {} $CXX $CXXFLAGS -fsyntax-only {}

echo "=== Build ==="
"$(dirname "$0")/build.sh"

echo "=== Test ==="
"$(dirname "$0")/test.sh"

echo "=== Quality OK ==="
