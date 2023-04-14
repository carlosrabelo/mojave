#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/../mojave" && pwd)"
REPO_ROOT="$(cd "$(dirname "$0")/.." && pwd)"
SRC_DIR="$ROOT_DIR/src"
TEST_DIR="$ROOT_DIR/tests"
LIB_DIR="$ROOT_DIR/lib"
BUILD_DIR="$REPO_ROOT/build/bin"
REPORT_DIR="$REPO_ROOT/build/coverage"
BINARY_NAME="${BINARY_NAME:-mojave}"
CXX="${CXX:-g++}"
CXXFLAGS="-std=c++20 -O0 -g -fprofile-arcs -ftest-coverage -I$SRC_DIR -I$TEST_DIR -I$LIB_DIR"
LDFLAGS="-fprofile-arcs -ftest-coverage"

mkdir -p "$BUILD_DIR" "$REPORT_DIR"
mapfile -t SRC_FILES < <(find "$SRC_DIR" -name '*.cpp' ! -path '*/frontend/tty/main.cpp' ! -path '*/frontend/sdl/main.cpp' ! -path '*/frontend/qt6/main.cpp' -print | LC_ALL=C sort)
mapfile -t TEST_SRCS < <(find "$TEST_DIR" -name '*.cpp' -print | LC_ALL=C sort)
$CXX $CXXFLAGS $LDFLAGS "${SRC_FILES[@]}" "${TEST_SRCS[@]}" -o "$BUILD_DIR/${BINARY_NAME}-coverage"
echo "Running tests (coverage build)..."
"$BUILD_DIR/${BINARY_NAME}-coverage" "$@"
echo "Generating coverage report..."
gcovr --root "$REPO_ROOT" --filter "$SRC_DIR" --html-details "$REPORT_DIR/index.html" --xml "$REPORT_DIR/coverage.xml"
echo "Coverage report: $REPORT_DIR/index.html"
