#!/usr/bin/env bash
# Incremental-build mojave-tests, then run Catch filtered by changed paths.
#
# Maps dirty/diff paths under mojave/ to Catch tags. Commit gate remains
# `make test-all`. Safe default when the change set is broad or unclear: [fast].
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

declare -a CHANGED=()
if git -C "$REPO_ROOT" rev-parse --is-inside-work-tree >/dev/null 2>&1; then
    while IFS= read -r line; do
        [ -n "$line" ] && CHANGED+=("$line")
    done < <({
        git -C "$REPO_ROOT" diff --name-only HEAD 2>/dev/null || true
        git -C "$REPO_ROOT" ls-files --others --exclude-standard 2>/dev/null || true
    } | LC_ALL=C sort -u | grep -E '^mojave/' || true)
fi

if [ "${#CHANGED[@]}" -eq 0 ]; then
    echo "No changed files under mojave/; nothing to test."
    exit 0
fi

echo "Changed under mojave/ (${#CHANGED[@]}):"
printf '  %s\n' "${CHANGED[@]}"

# Collect tag names without brackets (z80, sinclair, fast, ...).
declare -A SEEN=()
declare -a TAG_LIST=()
add_tag() {
    local t="$1"
    if [ -z "${SEEN[$t]:-}" ]; then
        SEEN[$t]=1
        TAG_LIST+=("$t")
    fi
}

for path in "${CHANGED[@]}"; do
    case "$path" in
        mojave/src/cpus/z80/*|mojave/tests/cpus/test_z80.cpp)
            add_tag "z80" ;;
        mojave/src/cpus/m6502/*|mojave/tests/cpus/test_m6502.cpp)
            add_tag "m6502" ;;
        mojave/src/cpus/m6507*|mojave/tests/cpus/test_m6507.cpp)
            add_tag "m6507" ;;
        mojave/src/devices/sinclair/*|mojave/tests/devices/sinclair/*)
            add_tag "sinclair" ;;
        mojave/src/machines/zx80/*|mojave/src/devices/zx80/*|mojave/tests/machines/zx80/*|mojave/tests/devices/zx80/*|mojave/src/frontend/*zx80*)
            add_tag "zx80" ;;
        mojave/src/machines/zx81/*|mojave/src/devices/zx81/*|mojave/tests/machines/zx81/*|mojave/tests/devices/zx81/*|mojave/src/frontend/*zx81*)
            add_tag "zx81" ;;
        mojave/src/machines/trs80m3/*|mojave/src/devices/trs80m3/*|mojave/tests/machines/trs80m3/*|mojave/tests/devices/trs80m3/*|mojave/src/frontend/*trs80m3*)
            add_tag "trs80m3" ;;
        mojave/src/machines/trs80m1l1/*|mojave/tests/machines/trs80m1l1/*|mojave/src/frontend/*trs80m1l1*)
            add_tag "trs80m1l1" ;;
        mojave/src/machines/trs80m1l2/*|mojave/tests/machines/trs80m1l2/*)
            add_tag "trs80m1l2" ;;
        mojave/src/devices/trs80m1/*|mojave/tests/devices/trs80m1/*)
            add_tag "trs80m1l1"
            add_tag "trs80m1l2"
            ;;
        mojave/src/bus/*|mojave/tests/bus/*)
            add_tag "bus" ;;
        mojave/src/frontend/shared/sinclair*)
            add_tag "sinclair"
            add_tag "zx80"
            add_tag "zx81"
            ;;
        *)
            # session, shared devices, helpers, unclear → full fast suite
            add_tag "fast" ;;
    esac
done

tag_count="${#TAG_LIST[@]}"
FILTER="[fast]"
if [ "$tag_count" -eq 0 ]; then
    FILTER="[fast]"
elif [ -n "${SEEN[fast]:-}" ]; then
    FILTER="[fast]"
elif [ "$tag_count" -gt 3 ]; then
    FILTER="[fast]"
else
    FILTER=""
    for t in "${TAG_LIST[@]}"; do
        if [ -z "$FILTER" ]; then
            FILTER="[$t]"
        else
            FILTER="$FILTER,[$t]"
        fi
    done
fi

echo "Running tests (filter: $FILTER)..."
exec "$OUTPUT" "$FILTER"
