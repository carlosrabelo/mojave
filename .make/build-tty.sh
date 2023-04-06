#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/../mojave" && pwd)"
REPO_ROOT="$(cd "$(dirname "$0")/.." && pwd)"
SRC_DIR="$ROOT_DIR/src"
BUILD_DIR="$REPO_ROOT/build/bin"
OBJ_ROOT="$REPO_ROOT/build/obj/tty"
BINARY_NAME="${BINARY_NAME:-mojave}"
CXX="${CXX:-g++}"
CXXFLAGS="-std=c++20 -Wall -Wextra -Wpedantic -I$SRC_DIR"
OUTPUT="$BUILD_DIR/$BINARY_NAME"

mapfile -t SRC_FILES < <(find "$SRC_DIR" -name '*.cpp' \
    ! -path '*/frontend/sdl/*' ! -path '*/frontend/qt6/*' \
    ! -name 'sinclair_sdl_input.cpp' ! -name 'sinclair_qt_input.cpp' \
    -print | LC_ALL=C sort)

echo "Building $BINARY_NAME..."
REPO_ROOT="$REPO_ROOT" OBJ_ROOT="$OBJ_ROOT" OUTPUT="$OUTPUT" CXX="$CXX" CXXFLAGS="$CXXFLAGS" \
    "$REPO_ROOT/.make/compile-objects.sh" "${SRC_FILES[@]}"
