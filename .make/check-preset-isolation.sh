#!/usr/bin/env bash
# Preset isolation guard.
#
# Enforces that machine presets stay decoupled at compile time: a file owned by
# one preset must not include headers from, or reference types of, another
# preset. This keeps changes to one machine from affecting the others.
#
# What counts as a preset's own files (matched by path):
#   mojave/src/machines/<id>/      mojave/src/devices/<id>/
#   mojave/src/frontend/*<id>*     mojave/tests/machines/<id>/
#   mojave/tests/devices/<id>/
#
# The generic shared layer (mojave/src/devices/shared/, cpus/, bus/, session/)
# is intentionally exempt. Declared device-family folders (devices/trs80m1/,
# devices/sinclair/) are allowed only for their member presets — outsiders are
# blocked by RULES below.
#
# A preset may only depend on:
#   - its own folder
#   - the generic shared layer (memory, framebuffer, device/port base, screen)
#   - declared family folders for which it is a member
#   - CPU cores (cpus/) and core infrastructure (bus/, session/)
# never on another preset's folder.
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "$0")/.." && pwd)"

# Each entry: "<preset-id>|<forbidden-path-fragment>|<forbidden-type-token>"
# A preset-id is also a path fragment (e.g. trs80m1l1 files live under .../trs80m1l1/).
#
# Rule: no file whose path contains the preset-id may contain the forbidden
# path fragment (an #include of the other preset) or the forbidden type token
# (a class name from the other preset).
RULES=(
    "trs80m3|trs80m1|Trs80M1"
    "trs80m1l1|trs80m3|Trs80M3"
    "trs80m1l2|trs80m3|Trs80M3"
)

# devices/trs80m1/ is shared by the Model I presets (l1 and l2) by design.
# It must not be touched by Model III, and Model III must not be touched by it.
RULES+=(
    "trs80m3|devices/trs80m1|Trs80M1"
)

# devices/sinclair/ is shared by zx80 and zx81 (membrane keyboard, cassette stub,
# host-input helpers). Outsiders must not include it; Sinclair types use the
# Sinclair* token (distinct from Z80 CPU / Zx80 / Zx81 preset tokens).
RULES+=(
    "trs80m3|devices/sinclair|Sinclair"
    "trs80m1l1|devices/sinclair|Sinclair"
    "trs80m1l2|devices/sinclair|Sinclair"
)

RULES+=(
    "zx80|trs80m1|Trs80M1"
    "zx80|trs80m3|Trs80M3"
    "zx80|devices/trs80m1|Trs80M1"
    "zx80|devices/trs80m3|Trs80M3"
    "trs80m3|zx80|Zx80"
    "trs80m1l1|zx80|Zx80"
    "trs80m1l2|zx80|Zx80"
)

RULES+=(
    "zx81|zx80|Zx80"
    "zx81|trs80m1|Trs80M1"
    "zx81|trs80m3|Trs80M3"
    "zx81|devices/trs80m1|Trs80M1"
    "zx81|devices/trs80m3|Trs80M3"
    "zx80|zx81|Zx81"
    "trs80m3|zx81|Zx81"
    "trs80m1l1|zx81|Zx81"
    "trs80m1l2|zx81|Zx81"
)

status=0

for rule in "${RULES[@]}"; do
    IFS='|' read -r preset forbidden_path forbidden_token <<< "$rule"

    # Files owned by this preset.
    mapfile -t own_files < <({
        find "$ROOT_DIR/mojave/src/machines/$preset" "$ROOT_DIR/mojave/src/devices/$preset" \
             "$ROOT_DIR/mojave/tests/machines/$preset" "$ROOT_DIR/mojave/tests/devices/$preset" \
             -type f 2>/dev/null
        find "$ROOT_DIR/mojave/src/frontend" -type f -name "*${preset}*" 2>/dev/null
    } | LC_ALL=C sort -u)

    for f in "${own_files[@]}"; do
        [ -f "$f" ] || continue
        # Two real contamination vectors:
        #   1. #include of the other preset's header (path fragment in an include line)
        #   2. use of the other preset's C++ identifiers (camelCase type token)
        # Lowercase preset-id strings used in negative assertions (e.g. the
        # literal "trs80m1l2" passed to isTrs80M3PresetId) are NOT a dependency
        # and are intentionally ignored.
        includes=$(grep -nE "^[[:space:]]*#[[:space:]]*include.*${forbidden_path}" "$f" 2>/dev/null || true)
        types=$(grep -nE "${forbidden_token}[A-Za-z0-9_]*" "$f" 2>/dev/null || true)
        matches="${includes}${types:+$'\n'"$types"}"
        if [ -n "$matches" ]; then
            echo "ISOLATION VIOLATION: preset '$preset' references '$forbidden_path'/'$forbidden_token'" >&2
            echo "  file: $f" >&2
            echo "$matches" | sed 's/^/    /' >&2
            status=1
        fi
    done
done

if [ "$status" -ne 0 ]; then
    echo >&2
    echo "Preset isolation broken. Each preset must depend only on its own folder," >&2
    echo "declared family folders (e.g. devices/trs80m1/, devices/sinclair/)," >&2
    echo "the generic shared layer (devices/shared/, cpus/, bus/, session/), and CPU cores." >&2
    echo "Do NOT include from or reference another preset; use a family folder for identical hardware." >&2
    exit 1
fi

echo "Preset isolation OK"
