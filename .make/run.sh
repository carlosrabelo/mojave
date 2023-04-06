#!/usr/bin/env bash
set -euo pipefail

REPO_ROOT="$(cd "$(dirname "$0")/.." && pwd)"
BUILD_DIR="$REPO_ROOT/build/bin"
BINARY_NAME="${BINARY_NAME:-mojave}"

if [ ! -f "$BUILD_DIR/$BINARY_NAME" ]; then
	echo "Binary not found. Build it first."
	exit 1
fi

exec "$BUILD_DIR/$BINARY_NAME" "$@"
