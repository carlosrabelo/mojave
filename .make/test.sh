#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/../mojave" && pwd)"
REPO_ROOT="$(cd "$(dirname "$0")/.." && pwd)"
SRC_DIR="$ROOT_DIR/src"
TEST_DIR="$ROOT_DIR/tests"
LIB_DIR="$ROOT_DIR/lib"
BUILD_DIR="$REPO_ROOT/build/bin"
OBJ_ROOT="$REPO_ROOT/build/obj/tests"
BINARY_NAME="${BINARY_NAME:-mojave}"
CXX="${CXX:-g++}"
CXXFLAGS="-std=c++20 -Wall -Wextra -Wpedantic -I$SRC_DIR -I$TEST_DIR -I$LIB_DIR"
OUTPUT="$BUILD_DIR/${BINARY_NAME}-tests"

mapfile -t SRC_FILES < <(find "$SRC_DIR" -name '*.cpp' ! -path '*/frontend/*' -print | LC_ALL=C sort)
mapfile -t TEST_SRCS < <(find "$TEST_DIR" -name '*.cpp' -print | LC_ALL=C sort)

REPO_ROOT="$REPO_ROOT" OBJ_ROOT="$OBJ_ROOT" OUTPUT="$OUTPUT" CXX="$CXX" CXXFLAGS="$CXXFLAGS" \
    "$REPO_ROOT/.make/compile-objects.sh" "${SRC_FILES[@]}" "${TEST_SRCS[@]}"

echo "Running tests..."
exec "$OUTPUT" "$@"
