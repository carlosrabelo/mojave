# Contributing

Conventions for quality gates, tests, and machine preset layout in Mojave.

## Quality gates

| Command | Purpose |
|---------|---------|
| `make test-fast` | Development loop — incremental compile, then `[fast]` tests |
| `make test-changed` | Incremental compile; Catch filter derived from dirty `mojave/` paths |
| `make test-all` | Full test suite — must pass before every commit |
| `make check` | Syntax, build, full tests, and preset isolation guard |
| `make coverage` | HTML report in `build/coverage/` — recommended for large changes |

Builds compile each `.cpp` to `build/obj/<variant>/…/*.o` with header
dependency tracking (`-MMD`). The first run after `make clean` is a cold
compile; later edits rebuild only touched translation units and relink.

Use `make test-fast` or `make test-changed` while iterating; run `make test-all`
or `make check` before committing. Run `make check` after adding or moving
preset-owned source files.

## Tests

### Framework

- **Catch2** — `TEST_CASE`, `REQUIRE`, `CHECK`, `REQUIRE_THROWS`.
- Every public function or method should have at least one test.
- Unit tests use inline byte arrays — no external ROM files.
- Shared fixtures: `mojave/tests/helpers.hpp`, `mojave/tests/helpers/`.

### Mirror tree

`mojave/tests/` mirrors `mojave/src/`, preserving subfolders:

| Source | Test |
|--------|------|
| `src/bus/bus.hpp` | `tests/bus/test_bus.cpp` |
| `src/cpus/cpu.hpp` | `tests/cpus/test_cpu.cpp` |
| `src/devices/shared/memory.cpp` | `tests/devices/shared/test_memory.cpp` |
| `src/machines/<id>/<id>_preset.hpp` | `tests/machines/<id>/test_preset.cpp` |

### Tags

Add `[fast]` to every test that runs in <1 ms with no I/O and no guest binaries.

| Tag | Purpose |
|-----|---------|
| `[fast]` | Core unit tests |
| `[integration]` | Multi-component wiring |
| `[guests]` | Loads guest program binaries |
| `[cpu]` `[bus]` `[memory]` `[device]` `[machine]` | Area filters |

### Guest program fixtures

- Sources: `mojave/tests/guests/<cpu>/*.asm` (e.g. `z80/halt.asm`).
- Built via `mojave/tests/guests/Makefile`; falls back to inline hex when no
  assembler is installed.
- Loaded with `loadGuest()` from `helpers/guest_loader.hpp`.

## Machine presets

Each built-in preset (`trs80m1l1`, `trs80m1l2`, `trs80m3`, `zx80`, `zx81`, …)
stays compile-time isolated — a change to one preset must not affect another
unless they deliberately share a **device family** folder.

Full path/naming vocabulary and the new-machine checklist live in
[docs/shared/preset-layout.md](docs/shared/preset-layout.md).

### Dependency rules

A preset **may** depend on:

- its own folders (`machines/<id>/`, `devices/<id>/`, `frontend/*<id>*`, and
  matching paths under `tests/`)
- the generic shared layer (`devices/shared/`, `cpus/`, `bus/`, `session/`)
- **declared device family folders** (see below)

A preset **must not**:

- `#include` headers from, or reference types of, another preset's folders
- put machine-specific devices under `devices/shared/` (that layer is only for
  generic building blocks: memory, framebuffer, port base, TTY, screen registry)

When hardware **differs**, keep devices under `devices/<id>/`. When hardware is
**identical** across two or more presets, use a device family folder instead of
copy-paste.

### Device families

A **family id** is a folder under `devices/<family>/` for silicon/PCB behaviour
shared by member presets. Families are first-class exceptions to “one folder per
preset id,” not dumping grounds under `devices/shared/`.

| Family folder | Members | Typical contents |
|---------------|---------|------------------|
| `devices/trs80m1/` | `trs80m1l1`, `trs80m1l2` | Model I keyboard, video, system port, … |
| `devices/sinclair/` | `zx80`, `zx81` (later `zxspectrum` may join for membrane / EAR-MIC helpers) | Membrane keyboard, cassette stub, host-input helpers — **not** ULA/video |

**When to open a family:** the second preset needs the same device API and
constants as an existing one. The first preset may keep a local copy until then.

**What stays per-preset even inside a family:** video/ULA, memory maps, port
decode that differs, ROM charset base addresses, NMI/INT policy, the preset
contract, and `create*Machine()` wiring.

Model III (`trs80m3`) and all non-member presets must not include from a family
they do not belong to. The retired `devices/shared/trs80m1/` path was removed
because “shared” invited cross-preset coupling.

### Enforcement and new presets

`make check` runs `.make/check-preset-isolation.sh`. The build fails with
`ISOLATION VIOLATION` if a preset-owned file crosses these boundaries.

Every **new family** (or new member of an existing family) must update the
isolation `RULES` in that script: allow each member to reference the family
path/types, and forbid outsiders.

To add a preset, follow the checklist in
[docs/shared/preset-layout.md](docs/shared/preset-layout.md) and the atomic
items in `TODO.md`. Use `trs80m3` as a wiring reference for contract headers,
framebuffer binding, and the layered host-keyboard steps (device, then SDL/Qt6
bridge, then layout typing). Prefer `trs80m1l1`/`trs80m1l2` as the reference
for family-shared devices.

### Hardware notes

Canonical hardware and preset notes live under `docs/machines/`. Read the
matching document before implementing or changing a machine preset.
