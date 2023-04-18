# Preset layout and naming

Conventions for built-in machine presets: folder layout, type names, shared
**device families**, and isolation. Dependency rules are summarized in
[CONTRIBUTING.md](../../CONTRIBUTING.md); this note is the naming checklist.

## Preset id vs family id

| Term | Meaning | Examples |
|------|---------|----------|
| **Preset id** | Registry string and primary folder name | `trs80m1l1`, `trs80m3`, `zx80`, `zx81` |
| **Family id** | Hardware family folder when 2+ presets share *identical* device behaviour | `trs80m1` (exists); `sinclair` (future) |

Identical means the same silicon/PCB API and constants — not “similar.” Similar
hardware stays duplicated under each preset until a second member needs the same
API; then open a family folder.

## Path and type naming

| Layer | Path pattern | Type / file naming |
|-------|--------------|--------------------|
| Contract / factory | `machines/<id>/<id>_preset.{hpp,cpp}` | `<Id>PresetContract`, `create<Id>Machine`, `is<Id>PresetId` |
| Preset-only devices | `devices/<id>/<concept>.{hpp,cpp}` | `<Id><Concept>` (e.g. `Zx81VideoGenerator`) |
| Family devices | `devices/<family>/<concept>.{hpp,cpp}` | `<Family><Concept>` (e.g. `Trs80M1Keyboard`; future `SinclairKeyboard`) |
| Host keyboard | `machines/<id>/` + `frontend/*<id>*` | Bridge filename **embeds full preset id** (e.g. `zx81_host_keyboard_bridge_sdl.cpp`) |
| Tests | Mirror under `tests/devices/<id\|family>/`, `tests/machines/<id>/` | Same basenames as sources |

For **new** presets, prefer embedding the full preset id in host bridge filenames
(`trs80m1l1_…`, `trs80m3_…`). Older TRS bridges may keep historical names; do not
rename them solely for style.

## Basename vocabulary

Reuse these concept basenames across machines instead of inventing synonyms:

`keyboard`, `cassette`, `port_decode`, `video_generator` / `video_controller`,
`display_file`, `character_rom`, `host_keyboard_adapter`, `typing_chord`,
`host_keymap`, `<id>_host_keyboard_bridge`.

## Isolation tokens

Isolation `RULES` match path and type substrings. Keep these distinct:

| Token | Meaning |
|-------|---------|
| `Z80` | CPU / basic `z80` smoke preset |
| `Zx80` / `zx80` | Sinclair ZX-80 preset |
| `Zx81` / `zx81` | Sinclair ZX-81 preset |
| `Trs80M1` / `trs80m1` | Model I family (not Model III) |

Do not invent type names that collide across these groups.

## Allowed dependencies

A preset may use:

1. Own folders: `machines/<id>/`, `devices/<id>/`, `frontend/*<id>*`, tests
2. Generic shared: `devices/shared/`, `cpus/`, `bus/`, `session/`
3. Declared family folders listed in isolation `RULES` (e.g. `devices/trs80m1/`
   for `trs80m1l1` / `trs80m1l2`)

Forbidden: `#include` another preset's folder; machine-specific code under
`devices/shared/`; including a family folder without being a declared member.

## Current and planned families

| Family | Members | Status |
|--------|---------|--------|
| `devices/trs80m1/` | `trs80m1l1`, `trs80m1l2` | Exists |
| `devices/sinclair/` | `zx80`, `zx81` (later `zxspectrum` may reuse membrane / EAR-MIC / host-input) | Exists — ULA/video stay per-preset |

ZX-80/81 share `SinclairKeyboard`, `SinclairCassette`, and host-input helpers under
`devices/sinclair/`. Per-preset folders keep video/ULA, port decode, display file,
character ROM, contracts, and `*HostKeyboardBridge` filenames that embed the
preset id.

## What stays per-preset

Even when a family exists, keep these under the preset (or preset-only devices):

- Video / ULA behaviour that diverges
- Memory maps and ROM load bounds
- Port decode that differs
- ROM character set base offsets
- NMI / INT policy
- Preset contract and `create*Machine()` wiring

## New-machine checklist

1. Write `docs/machines/<id>.md`
2. Decide family membership: reuse an existing family, open a new family, or stay solo
3. Add contract + registry entry
4. Implement devices (family folder first if a member, else `devices/<id>/`)
5. Clock / interrupts
6. SDL / Qt6 virtual screen
7. Host keyboard layers (device → bridge → layout typing)
8. Update isolation `RULES` in `.make/check-preset-isolation.sh` (required for every new family or family member) and run `make check`

Use atomic `TODO.md` items for each layer. See also [machines/README.md](../machines/README.md).

## Future work

Spectrum (and other Sinclair machines) may join `devices/sinclair/` for membrane
keyboard / cassette / host-input when those APIs match. Isolation `RULES` must
list every new member.
