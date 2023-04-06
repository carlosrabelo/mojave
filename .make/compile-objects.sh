#!/usr/bin/env bash
# Incremental compile + link helper.
#
# Compiles each .cpp to $OBJ_ROOT/<relpath-from-REPO_ROOT>.o with -MMD -MP,
# rebuilds only when the source or its headers change, then links $OUTPUT.
#
# Required env:
#   REPO_ROOT  — repository root (for relative object paths)
#   OBJ_ROOT   — object tree root (e.g. build/obj/tests)
#   OUTPUT     — linked binary path
#   CXXFLAGS   — compile flags (do not include -c)
#
# Optional env:
#   CXX        — compiler (default g++)
#   LDFLAGS    — extra link flags / libraries (appended after objects)
#   JOBS       — parallel compile jobs (default: nproc or 4)
#
# Sources are passed as positional arguments (absolute or relative .cpp paths).
set -euo pipefail

if [ $# -lt 1 ]; then
    echo "compile-objects.sh: no source files" >&2
    exit 1
fi

: "${REPO_ROOT:?REPO_ROOT is required}"
: "${OBJ_ROOT:?OBJ_ROOT is required}"
: "${OUTPUT:?OUTPUT is required}"
: "${CXXFLAGS:?CXXFLAGS is required}"

CXX="${CXX:-g++}"
LDFLAGS="${LDFLAGS:-}"
if [ -z "${JOBS:-}" ]; then
    JOBS="$(nproc 2>/dev/null || echo 4)"
fi

mkdir -p "$OBJ_ROOT" "$(dirname "$OUTPUT")"

declare -a ABS_SOURCES=()
declare -a OBJECTS=()
for src in "$@"; do
    if [ ! -f "$src" ]; then
        echo "compile-objects.sh: missing source: $src" >&2
        exit 1
    fi
    abs="$(cd "$(dirname "$src")" && pwd)/$(basename "$src")"
    case "$abs" in
        "$REPO_ROOT"/*) rel="${abs#"$REPO_ROOT"/}" ;;
        *)
            echo "compile-objects.sh: source outside REPO_ROOT: $abs" >&2
            exit 1
            ;;
    esac
    ABS_SOURCES+=("$abs")
    OBJECTS+=("$OBJ_ROOT/${rel}.o")
done

needs_rebuild() {
    local abs="$1"
    local obj="$2"
    local dep="${obj%.o}.d"

    if [ ! -f "$obj" ] || [ "$abs" -nt "$obj" ]; then
        return 0
    fi
    if [ ! -f "$dep" ]; then
        return 0
    fi

    # Make dep file may wrap lines with backslash; flatten then check headers.
    local line deps
    line="$(tr '\n' ' ' <"$dep" | sed 's/\\//g')"
    deps="${line#*:}"
    for d in $deps; do
        if [ -f "$d" ] && [ "$d" -nt "$obj" ]; then
            return 0
        fi
    done
    return 1
}

work_dir="$(mktemp -d)"
trap 'rm -rf "$work_dir"' EXIT

declare -a PIDS=()
fail=0

wait_slot() {
    while [ "${#PIDS[@]}" -ge "$JOBS" ]; do
        local pid="${PIDS[0]}"
        PIDS=("${PIDS[@]:1}")
        if ! wait "$pid"; then
            fail=1
        fi
    done
}

for i in "${!ABS_SOURCES[@]}"; do
    wait_slot
    (
        abs="${ABS_SOURCES[$i]}"
        obj="${OBJECTS[$i]}"
        dep="${obj%.o}.d"
        rel="${abs#"$REPO_ROOT"/}"
        mkdir -p "$(dirname "$obj")"

        if ! needs_rebuild "$abs" "$obj"; then
            exit 0
        fi

        echo "  CXX $rel"
        # shellcheck disable=SC2086
        $CXX $CXXFLAGS -c -MMD -MP -MF "$dep" -o "$obj" "$abs"
        touch "$work_dir/compiled.$i"
    ) &
    PIDS+=("$!")
done

for pid in "${PIDS[@]}"; do
    if ! wait "$pid"; then
        fail=1
    fi
done

if [ "$fail" -ne 0 ]; then
    echo "compile-objects.sh: compile failed" >&2
    exit 1
fi

compiled=0
if compgen -G "$work_dir/compiled.*" >/dev/null; then
    compiled="$(find "$work_dir" -name 'compiled.*' | wc -l)"
fi

needs_link=0
if [ ! -f "$OUTPUT" ]; then
    needs_link=1
elif [ "$compiled" -gt 0 ]; then
    needs_link=1
else
    for obj in "${OBJECTS[@]}"; do
        if [ "$obj" -nt "$OUTPUT" ]; then
            needs_link=1
            break
        fi
    done
fi

if [ "$needs_link" -eq 0 ]; then
    echo "Up to date: $OUTPUT"
    exit 0
fi

echo "  LD  ${OUTPUT#"$REPO_ROOT"/}"
# shellcheck disable=SC2086
$CXX "${OBJECTS[@]}" -o "$OUTPUT" $LDFLAGS
echo "Done: $OUTPUT (${compiled} translated)"
