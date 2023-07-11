# TRS-80 Model I (`trs80m1l1`, `trs80m1l2`)

Hardware and Mojave preset notes for the Model I Level I and Level II machines.

| Preset ID | BASIC | ROM | Base RAM | Expansion |
|-----------|-------|-----|----------|-----------|
| `trs80m1l1` | Level I | 4 KiB | 4 KiB | None |
| `trs80m1l2` | Level II | 12 KiB | 16 KiB | 32 KiB at 0x8000 (optional) |

Both presets share the same CPU clock, keyboard matrix, video engine, printer
status latch, and port `0xFF` cassette/screen decode. Level I first; Level II
reuses Model I devices from `devices/trs80m1/` (the only cross-preset device
folder allowed — shared **only** between `trs80m1l1` and `trs80m1l2`).

Never `#include` from `trs80m3/` or other presets. Model III duplicates similar
hardware in `devices/trs80m3/` by design.

## System overview (shared)

| Property | Value |
|----------|-------|
| CPU | Z80A @ 1,774,080 Hz (10.6445 MHz crystal ÷ 6) |
| Video | 64×16 text, uppercase only |
| Keyboard | 8 row addresses; **no** CONTROL or CAPS LOCK (those are Model III only) |
| Cassette | 500 baud FSK via port `0xFF` |
| Form factor | Separate keyboard, monitor, expansion interface |

### Key differences from Model III (`trs80m3`)

- Slower clock (1.774 MHz vs 2.03 MHz)
- Smaller ROM and different BASIC entry points
- No built-in 30 Hz RTC interrupt
- Uppercase-only video (no lowercase glyphs)
- Port `0xFF` for cassette/screen (Model III uses `0xFC`–`0xFF` and richer decode)
- Simpler I/O without Model III interrupt latch block
- Keyboard row `0x3880` is SHIFT only — no CONTROL / CAPS LOCK keys

## Preset `trs80m1l1` — Level I

### Memory map

| Address range | Size | Description |
|---------------|------|-------------|
| 0x0000 – 0x0FFF | 4 KiB | ROM (Level I BASIC) |
| 0x1000 – 0x37DF | — | Unmapped / mirrors |
| 0x37E8 | 1 byte | Printer status (active low) |
| 0x3800 – 0x3BFF | 1 KiB | Keyboard matrix (memory-mapped) |
| 0x3C00 – 0x3FFF | 1 KiB | Video VRAM (64×16) |
| 0x4000 – 0x4FFF | 4 KiB | RAM |

### Contract hints

- `load_rom_address = 0x0000`, `load_rom_end_exclusive = 0x1000`
- `guest_cpu_clock_hz = 1'774'080`
- `needs_virtual_screen = true`
- No expansion RAM or expansion-interface ports in the core profile

### Source layout

- Preset: `mojave/src/machines/trs80m1l1/`
- Devices: `mojave/src/devices/trs80m1/` (shared with Level II)
- Tests: `mojave/tests/machines/trs80m1l1/`, `mojave/tests/devices/trs80m1/`

## Preset `trs80m1l2` — Level II

### Memory map (core)

| Address range | Size | Description |
|---------------|------|-------------|
| 0x0000 – 0x2FFF | 12 KiB | ROM (Level II BASIC) |
| 0x3000 – 0x37DF | — | Unmapped / mirrors |
| 0x37E8 | 1 byte | Printer status (active low) |
| 0x3800 – 0x3BFF | 1 KiB | Keyboard matrix |
| 0x3C00 – 0x3FFF | 1 KiB | Video VRAM |
| 0x4000 – 0x7FFF | 16 KiB | Base RAM |

### Optional expansion (after core Level II)

| Address range | Size | Description |
|---------------|------|-------------|
| 0x8000 – 0xFFFF | 32 KiB | Expansion RAM (expansion interface) |

Expansion-interface port decode (optional): `0xE8`–`0xEF` printer and serial.

### Contract hints

- `load_rom_address = 0x0000`, `load_rom_end_exclusive = 0x3000`
- Same clock and shared `devices/trs80m1/` devices as Level I where addresses match
- Fork into `devices/trs80m1l2/` only when Level II hardware genuinely differs

### Source layout

- Preset: `mojave/src/machines/trs80m1l2/`
- Devices: `mojave/src/devices/trs80m1/` (shared with Level I) plus
  `mojave/src/devices/trs80m1l2/` for expansion-only devices
- Tests: `mojave/tests/machines/trs80m1l2/`, `mojave/tests/devices/trs80m1/`

## Shared I/O — port `0xFF` (cassette and screen mode)

- **Write**:
  - Bits 0–1: Cassette data output
  - Bit 2: Cassette motor (1 = on)
  - Bit 3: Screen columns (0 = 64, 1 = 32)
- **Read**:
  - Bits 5–7: Cassette data input

32-column text mode is optional — implement only after the core 64-column path works.

## Keyboard matrix (both presets)

Reading `0x3800 | (1 << row)` returns an 8-bit byte (1 = pressed).

| Address | Bit 0 | Bit 1 | Bit 2 | Bit 3 | Bit 4 | Bit 5 | Bit 6 | Bit 7 |
|---------|-------|-------|-------|-------|-------|-------|-------|-------|
| 0x3801 | @ | A | B | C | D | E | F | G |
| 0x3802 | H | I | J | K | L | M | N | O |
| 0x3804 | P | Q | R | S | T | U | V | W |
| 0x3808 | X | Y | Z | | | | | |
| 0x3810 | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 |
| 0x3820 | 8 | 9 | : | ; | , | - | . | / |
| 0x3840 | ENTER | CLEAR | BREAK | Up | Down | Left | Right | SPACE |
| 0x3880 | SHIFT | | | | | | | |

Wire host input through SDL/Qt6 keyboard bridges under `mojave/src/frontend/`.

## Video display generator (both presets)

- 64 columns × 16 rows character display
- VRAM: 1 KiB at `0x3C00`–`0x3FFF`
- Framebuffer output: 384×192 or 512×192 depending on cell width (6×12 typical)
- Character codes 0–127, uppercase only
- Block graphics: 128–191 (2×3 grid per cell)

## Cassette interface

- 500 baud FSK
- Motor and data lines via port `0xFF` (see above)

## Implementation order (matches `TODO.md`)

For each preset section in `TODO.md`, implement in this device order:

1. Preset contract + memory map — ROM/RAM regions, clock, load aliases
2. Video controller — VRAM read, glyph decode, `Framebuffer` binding
3. Printer status — memory-mapped `0x37E8`
4. Port `0xFF` — cassette bits and 32-column mode latch
5. Keyboard matrix — row decode (device only; no frontend includes)
6. Machine clock — 1.774 MHz guest pacing
7. Virtual screen — SDL/Qt6 framebuffer display
8. Host keyboard wiring — SDL/Qt6 bridges
9. Host layout typing — layout-aware character injection

Optional items (32-column mode; L2 expansion RAM/ports) only after the core
checklist above is solid and tested.

## Anti-patterns

| Do not | Do instead |
|--------|------------|
| Share Model I devices with Model III | Duplicate under `devices/trs80m3/` |
| Put CONTROL / CAPS LOCK on Model I | Those keys are Model III only |
| Start with 32-column mode | Ship 64-column first |
| Bundle keyboard device + SDL in one step | Device, then bridge, then host typing |
