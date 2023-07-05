#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/../mojave" && pwd)"
REPO_ROOT="$(cd "$(dirname "$0")/.." && pwd)"
SRC_DIR="$ROOT_DIR/src"
BUILD_DIR="$REPO_ROOT/build/bin"
OBJ_ROOT="$REPO_ROOT/build/obj/qt6"
BINARY_NAME="${BINARY_NAME:-mojave-qt6}"
CXX="${CXX:-g++}"
QT_CFLAGS="$(pkg-config --cflags Qt6Widgets 2>/dev/null || echo "-I/usr/include/x86_64-linux-gnu/qt6/QtWidgets -I/usr/include/x86_64-linux-gnu/qt6 -DQT_WIDGETS_LIB -I/usr/include/x86_64-linux-gnu/qt6/QtGui -DQT_GUI_LIB -I/usr/include/x86_64-linux-gnu/qt6/QtCore -DQT_CORE_LIB")"
QT_LIBS="$(pkg-config --libs Qt6Widgets 2>/dev/null || echo "-lQt6Widgets -lQt6Gui -lQt6Core")"
CXXFLAGS="-std=c++20 -Wall -Wextra -Wpedantic -DMOJAVE_FRONTEND_QT6 -I$SRC_DIR $QT_CFLAGS"
OUTPUT="$BUILD_DIR/$BINARY_NAME"

mapfile -t SRC_FILES < <(find "$SRC_DIR" -name '*.cpp' \
    ! -path '*/frontend/tty/*' ! -path '*/frontend/sdl/*' \
    ! -name 'sinclair_sdl_input.cpp' \
    -print | LC_ALL=C sort)

echo "Building $BINARY_NAME..."
REPO_ROOT="$REPO_ROOT" OBJ_ROOT="$OBJ_ROOT" OUTPUT="$OUTPUT" CXX="$CXX" CXXFLAGS="$CXXFLAGS" \
    LDFLAGS="$QT_LIBS" \
    "$REPO_ROOT/.make/compile-objects.sh" "${SRC_FILES[@]}"
