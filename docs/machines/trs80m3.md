# TRS-80 Model III (`trs80m3`)

Hardware and Mojave preset notes for the Model III machine.

Build on Model I **patterns** where hardware matches (keyboard row scan, VRAM
base, cassette bit semantics); implement Model III deltas in `devices/trs80m3/`
only. Never `#include` from `devices/trs80m1/` — duplicate and adapt code
instead (`make check` enforces this).

For Model I platform background, see [trs80m1.md](trs80m1.md) as a read-only
reference — do not link its sources.

## System overview

| Property | Value |
|----------|-------|
| Manufacturer | Tandy Corporation |
| Year | 1980 |
| CPU | Z80A @ 2,027,520 Hz |
| ROM | 14 KiB (Model III BASIC) |
| RAM | 48 KiB |
| Video | 64×16 text, upper and lowercase |
| Storage | Cassette 500/1500 baud + up to 2 floppy slots |
| Form factor | All-in-one integrated unit |

### Key differences from Model I (`trs80m1l1` / `trs80m1l2`)

- Faster clock (2.03 MHz vs 1.774 MHz)
- Larger ROM (14 KiB) and different BASIC entry points
- Built-in 30 Hz maskable interrupt via port `0xE0`
- Lowercase video glyphs (no hardware mod)
- Richer port decode (`0xE0`–`0xEF`) and memory-mapped I/O latches
- Keyboard adds CONTROL and CAPS LOCK keys on matrix row `0x3880`
- ~90% software backward compatible with Model I

## System clock

- CPU: Z80A at 2,027,520 Hz (`guest_cpu_clock_hz = 2'027'520`)
- Frame interrupt: 30 Hz maskable RTC, enabled via interrupt mask at port `0xE0`
  (`rtc_interrupt_hz = 30`; `rtc_cycles_per_interrupt = guest_cpu_clock_hz / 30`)

## Memory map

| Address range | Size | Description |
|---------------|------|-------------|
| 0x0000 – 0x37DF | ~14 KiB | ROM (Model III BASIC, first span) |
| 0x37E0 – 0x37EF | 16 bytes | Memory-mapped I/O latch window (overlays ROM) |
| 0x37F0 – 0x37FF | 16 bytes | ROM tail |
| 0x3800 – 0x3BFF | 1 KiB | Keyboard matrix |
| 0x3C00 – 0x3FFF | 1 KiB | Video VRAM (64×16) |
| 0x4000 – 0xFFFF | 48 KiB | System RAM |

The 14 KiB ROM occupies `0x0000`–`0x37FF` but is **split around** the I/O latch
window at `0x37E0`–`0x37EF`. Only a few addresses in that window are real I/O
(printer, cassette/disk latches). Every other offset must still return the
underlying ROM byte — Model III BASIC jumps into `0x37EB` after the Memory Size
prompt, so filling unused latch reads with `0xFF` prevents `READY`.

Video is memory-mapped, not port-mapped.

### Contract hints

- `load_rom_address = 0x0000`, `load_rom_end_exclusive = 0x3800` (14 KiB span)
- `io_latch_start = 0x37E0`, `io_latch_end_exclusive = 0x37F0`
- `rom_tail_start = 0x37F0`
- `ram_start = 0x4000`, `ram_end_exclusive = 65536`
- `guest_cpu_clock_hz = 2'027'520`
- `needs_virtual_screen = true`

### Source layout

- Preset: `mojave/src/machines/trs80m3/`
- Devices: `mojave/src/devices/trs80m3/`
- Tests: `mojave/tests/machines/trs80m3/`, `mojave/tests/devices/trs80m3/`

## Memory-mapped I/O latches (`0x37E0`–`0x37EF`)

Complement port decode. Core checklist keeps cassette and disk idle; printer
reports ready.

| Address | Use |
|---------|-----|
| 0x37E0 | Interrupt / cassette / drive-select latch |
| 0x37E1 | Disk drive 0 select |
| 0x37E4 | Cassette drive select |
| 0x37E8 | Printer data (write) / status (read; idle ready) |
| 0x37E9 | Printer output mirror |
| 0x37EC | Disk command (write) / status (read; idle) |
| 0x37ED | Disk track |
| 0x37EE | Disk sector |
| 0x37EF | Disk data |

Unused offsets in the window fall through to the ROM image (see memory map).

## I/O ports

Attach the Model III port block with low-byte decode
(`attachPort(..., /*decode_low_byte=*/true)`) so `0xE0`–`0xFF` mirror on the
low address byte.

### `0xE0` – `0xE3`: interrupt latch / mask

- **Read**: Interrupt latch status
- **Write**: Interrupt mask register
- Bit 2: Real-time clock interrupt (30 Hz)

### `0xE4` – `0xE7`: NMI options

- **Read**: NMI status (disk DRQ / INTRQ / index)
- **Write**: Enable disk DRQ (bit 6) and INTRQ (bit 7) as NMI sources
- Disk-index NMI path is a later checklist item (floppy stub)

### `0xE8` – `0xEB`: RS-232 (optional stub)

| Port | Use |
|------|-----|
| 0xE8 | Modem status (read) / UART master reset (write) |
| 0xE9 | Baud latch (write) |
| 0xEA | UART control (write) / status (read) |
| 0xEB | Data (TX write / RX read) |

Idle stub is enough for the core checklist. Default UART setup in ROM is
300 baud, 8-bit, 1 stop, no parity.

### `0xEC` – `0xEF`: hardware control

Write bits (Model III):

| Bit | Function |
|-----|----------|
| 1 | Cassette motor (0 = on, 1 = off) |
| 2 | Double-width characters (0 = 64-column, 1 = 32-column) |
| 3 | Alternate character set |
| 4 | External I/O bus enable |
| 5 | Video wait states |

**Read** clears the RTC interrupt latch.

### `0xF0` – `0xF4`: floppy controller (idle stub)

| Port | Use |
|------|-----|
| 0xF0 | FDC command (write) / status (read) |
| 0xF1 | Track |
| 0xF2 | Sector |
| 0xF3 | Data |
| 0xF4 | Drive select and density / side options |

Core checklist: idle status, no image transport. Disk-index NMI is wired
through the `0xE4` mask. A real FDC is a later removable-media item.

### `0xFC` – `0xFF`: cassette and screen mode

- **Write**:
  - Bits 0–1: Cassette write level
  - Bit 2: Cassette motor
  - Bit 3: Screen columns (0 = 64, 1 = 32)
- **Read**:
  - Bit 7: Cassette read

32-column text / 128×48 block-graphics mode is optional — implement only after
the core 64-column path works.

## Keyboard matrix

Same row scan as Model I (`0x3800 | (1 << row)`), with extra keys on row `0x3880`:

| Address | Bit 0 | Bit 1 | Bit 2 | Bit 3 | Bit 4 | Bit 5 | Bit 6 | Bit 7 |
|---------|-------|-------|-------|-------|-------|-------|-------|-------|
| 0x3801 | @ | A | B | C | D | E | F | G |
| 0x3802 | H | I | J | K | L | M | N | O |
| 0x3804 | P | Q | R | S | T | U | V | W |
| 0x3808 | X | Y | Z | | | | | |
| 0x3810 | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 |
| 0x3820 | 8 | 9 | : | ; | , | - | . | / |
| 0x3840 | ENTER | CLEAR | BREAK | Up | Down | Left | Right | SPACE |
| 0x3880 | SHIFT | CONTROL | CAPS LOCK | | | | | |

The Model III **keyboard matrix** includes CONTROL and CAPS LOCK on row
`0x3880` (bit 1 and bit 2). Separately, Model III BASIC also keeps a
**software CAPS flag** in RAM (`SHFLCK` at `0x4019`), often toggled via Right
Shift in ROM behaviour. Do not conflate the hardware matrix bit with the RAM
flag when wiring host input or debugging case conversion.

Wire host input through SDL/Qt6 keyboard bridges under `mojave/src/frontend/`.

## Video display generator

- 64×16 character display, VRAM at `0x3C00`–`0x3FFF`
- Framebuffer: 512×192 pixels (8×12 cells)
- Character codes 0–127 including lowercase
- Block graphics: 128–191 (2×3 grid per cell → 128×48 in 64-column mode)
- 32-column / 128×48 graphics mode is optional (`TODO.md`)

## Cassette interface

- 500 baud (Model I compatible) and 1500 baud high-speed
- Motor and data lines via ports `0xFC`–`0xFF` and the `0x37E0` latch window
- Core checklist: idle levels, no image transport

## Implementation order (matches `TODO.md`)

1. Preset contract + memory map — 14 KiB ROM, 48 KiB RAM, I/O latch window
2. Video controller — lowercase glyphs, 512×192 framebuffer, block graphics
3. Memory-mapped I/O latches — cassette, disk, printer idle
4. Port I/O decode — `0xE0`–`0xEF` interrupt latch, NMI, hardware control;
   use `attachPort(..., /*decode_low_byte=*/true)` for 8-bit port mirrors
5. Keyboard matrix — CONTROL and CAPS LOCK additions (device only)
6. Machine clock + 30 Hz interrupt — Z80 maskable INT when unmasked
7. Floppy controller stub — `0xF0`–`0xF4` idle and disk-index NMI path
8. RS-232 serial stub — `0xE8`–`0xEB`
9. Virtual screen — SDL/Qt6 framebuffer wiring
10. Host layout typing — SDL/Qt6 chord tables and layout-aware injection

Optional after core: 32-column text / 128×48 graphics.

## Patterns borrowed from Model I (duplicate, do not include)

| Concept | Model I source of truth | Model III action |
|---------|------------------------|------------------|
| Keyboard row scan | `0x3800 \| (1 << row)` | Same mechanism in `devices/trs80m3/keyboard.*` |
| VRAM base | `0x3C00` | Same base; own font in `trs80m3` devices |
| Cassette port bits | Port `0xFF` on Model I | Model III `0xFC`–`0xFF` |
| Printer status | `0x37E8` | Same address; own latch in the I/O window |

Do not copy Model I ROM size or port-only `0xFF` decode blindly — Model III has
its own map, latch overlay, and interrupt block.

## Anti-patterns

| Do not | Do instead |
|--------|------------|
| `#include` from `devices/trs80m1/` | Own copies under `devices/trs80m3/` |
| Assume uppercase-only video | Model III has lowercase glyphs |
| Treat CAPS only as `SHFLCK` in RAM | Matrix has a CAPS LOCK key; ROM also uses `0x4019` |
| Return `0xFF` for every latch-window read | Fall unused offsets through to the ROM image |
| Skip the 30 Hz interrupt | Core checklist includes clock + INT |
| Treat port `0xFF` like Model I alone | Use `0xFC`–`0xFF` and the interrupt block |
| Start with 32-column mode | Ship 64-column first |
| Bundle keyboard device + SDL in one step | Device, then host typing |
