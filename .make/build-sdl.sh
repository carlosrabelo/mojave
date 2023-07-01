#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/../mojave" && pwd)"
REPO_ROOT="$(cd "$(dirname "$0")/.." && pwd)"
SRC_DIR="$ROOT_DIR/src"
BUILD_DIR="$REPO_ROOT/build/bin"
OBJ_ROOT="$REPO_ROOT/build/obj/sdl"
BINARY_NAME="${BINARY_NAME:-mojave-sdl}"
CXX="${CXX:-g++}"
SDL_CFLAGS="$(pkg-config --cflags sdl2 2>/dev/null || echo "-I/usr/include/SDL2 -D_REENTRANT")"
SDL_LIBS="$(pkg-config --libs sdl2 2>/dev/null || echo "-lSDL2")"
CXXFLAGS="-std=c++20 -Wall -Wextra -Wpedantic -DMOJAVE_FRONTEND_SDL -I$SRC_DIR $SDL_CFLAGS"
OUTPUT="$BUILD_DIR/$BINARY_NAME"

mapfile -t SRC_FILES < <(find "$SRC_DIR" -name '*.cpp' \
    ! -path '*/frontend/tty/*' ! -path '*/frontend/qt6/*' \
    ! -name 'sinclair_qt_input.cpp' \
    -print | LC_ALL=C sort)

echo "Building $BINARY_NAME..."
REPO_ROOT="$REPO_ROOT" OBJ_ROOT="$OBJ_ROOT" OUTPUT="$OUTPUT" CXX="$CXX" CXXFLAGS="$CXXFLAGS" \
    LDFLAGS="$SDL_LIBS" \
    "$REPO_ROOT/.make/compile-objects.sh" "${SRC_FILES[@]}"
