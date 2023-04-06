#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/../mojave" && pwd)"
LIB_DIR="$ROOT_DIR/lib"
CATCH_URL="https://raw.githubusercontent.com/catchorg/Catch2/v2.13.10/single_include/catch2/catch.hpp"
CATCH_FILE="$LIB_DIR/catch.hpp"

if [ -f "$CATCH_FILE" ]; then
	echo "Catch2 header already present: $CATCH_FILE"
	exit 0
fi

mkdir -p "$LIB_DIR"
echo "Downloading Catch2 single header..."
curl -sSL "$CATCH_URL" -o "$CATCH_FILE"
echo "Done: $CATCH_FILE"
