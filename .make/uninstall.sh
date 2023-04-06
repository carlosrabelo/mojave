#!/usr/bin/env bash
set -euo pipefail

BINARY_NAME="${BINARY_NAME:-mojave}"

if [ -z "${PREFIX:-}" ]; then
    if [ "$(id -u)" -eq 0 ]; then
        PREFIX="/usr/local"
    else
        PREFIX="$HOME/.local"
    fi
fi

echo "Removing $BINARY_NAME from $PREFIX/bin/"
rm -f "$PREFIX/bin/$BINARY_NAME"
echo "Done"
