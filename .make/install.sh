#!/usr/bin/env bash
set -euo pipefail

REPO_ROOT="$(cd "$(dirname "$0")/.." && pwd)"
BUILD_DIR="$REPO_ROOT/build/bin"
BINARY_NAME="${BINARY_NAME:-mojave}"

if [ ! -f "$BUILD_DIR/$BINARY_NAME" ]; then
    echo "Binary not found. Build it first."
    exit 1
fi

if [ -z "${PREFIX:-}" ]; then
    if [ "$(id -u)" -eq 0 ]; then
        PREFIX="/usr/local"
    else
        PREFIX="$HOME/.local"
    fi
fi

echo "Installing $BINARY_NAME to $PREFIX/bin/"
install -d "$PREFIX/bin"
install -m 755 "$BUILD_DIR/$BINARY_NAME" "$PREFIX/bin/$BINARY_NAME"
echo "Done: $PREFIX/bin/$BINARY_NAME"
