# Mojave

Implementation order. Each item is one atomic unit: implement, test, next.
No skipping stages. This list is the rebuild blueprint: checked items exist
in the tree; unchecked items are not built yet.

```
foundation → test infrastructure → CPU stubs
  → Z80 opcodes → MOS 6502 opcodes → MOS 6507
  → session + CLI + config → virtual screens + frontends
  → basic presets → trs80m1l1 → trs80m1l2 → trs80m3 → zx80 → zx81
  → removable media (host attach) → upcoming machines (docs first)
```

Finish `trs80m1l1` before `trs80m1l2`; finish Model I before Model III.
Finish `zx80` before `zx81`. Write `docs/machines/<id>.md` before the first
implementation checkbox of a new preset.

---

## Foundation

- [x] Directory structure and build automation (`Makefile`, `.make/`)
- [x] Abstract Device interface and device tick (`tick(cycles)`)
- [x] Physical Memory with ROM, `directPointer()`, and out-of-bounds safety
- [x] Composite Bus (memory maps + port I/O, optional low-byte port decode)
- [x] Port I/O (`PortDevice` + Bus port routing)
- [x] Abstract Cpu (reset, step, halt, register snapshot, page-table hook)
- [x] Composite Machine (CPU + Bus + owned devices + tick fan-out)

## Test infrastructure

- [x] Mirror test tree and shared test utilities
- [x] Guest program fixtures (asm → `.bin` build system)
- [x] Test runner targets (`test-fast`, `test-changed`, `test-all`, `coverage`)
- [x] Preset isolation guard (`.make/check-preset-isolation.sh`) and `make check`
- [x] Test and preset-layout conventions documented (`CONTRIBUTING.md`, `docs/shared/`)

## CPU stubs

- [x] Z80 stub with registers, page-table fast path, and minimal step
- [x] MOS 6502 stub with registers (A, X, Y, SP, P), page-table fast path, and minimal step
- [x] MOS 6507 stub (class inheriting M6502; 13-bit address mask; external IRQ/NMI ignored)

## Z80 opcodes

Complete this section before any Z80 machine preset.

- [x] Base dispatch table (256 opcodes)
- [x] 8-bit and 16-bit loads, stack operations (00–3F)
- [x] LD r,r' and HALT (40–7F)
- [x] 8-bit ALU with flags (80–BF)
- [ ] Control flow, CALL/RET, RST, PUSH/POP (C0–FF)
- [ ] CB prefix: shifts, rotates, BIT, SET, RES (including SLL)
- [ ] ED prefix: ADC/SBC HL, block I/O, IN/OUT, RLD/RRD, official `ED 63`/`ED 6B`
- [ ] Unofficial ED bytes as 8-cycle NOP
- [ ] IX/IY: DD/FD prefix, indexed addressing, DDCB/FDCB
- [ ] Interrupts (IFF, NMI, INT modes 0/1/2, EI delay)
- [ ] Undocumented: WZ/MEMPTR, IXH/IXL, F bits 3/5
- [ ] Full coverage: every dispatch table entry bound to a handler

## MOS 6502 opcodes

Complete this section before `m6502` / `m6507` presets. M6507 reuses these handlers.

- [ ] 13 addressing modes (absolute, zero-page, indexed, indirect, relative)
- [ ] Dispatch table (151 official opcodes, including NOP `0xEA`)
- [ ] Loads (LDA/LDX/LDY), stores (STA/STX/STY), transfers (TAX etc.)
- [ ] ALU: ADC, SBC, AND, ORA, EOR with flags
- [ ] BCD mode for ADC and SBC
- [ ] Compare (CMP/CPX/CPY), BIT, INC/DEC, shifts (ASL/LSR/ROL/ROR)
- [ ] Control flow (JMP, JSR, RTS, branches with page-cross penalty)
- [ ] BRK, RTI, IRQ, NMI, flag instructions
- [ ] NMOS unofficial opcodes (NOP variants, KIL/JAM, LAX, SAX, DCP, ISB, SLO, RLA, SRE, RRA, and remaining undocumented)

## MOS 6507

- [ ] 8 KiB physical mirror wrap on every access (read/write/fetch/stack)
- [ ] External IRQ and NMI ignored; BRK reads vector from masked address
- [ ] M6502 opcode test suite in the reduced address space

## Session, CLI and clock

- [ ] Virtual TTY (data/status registers, circular buffers; port and memory-mapped)
- [ ] Host binary loader (`--load-bin`)
- [ ] `MojaveConfig` / `mojave.ini` (`[roms]` path aliases)
- [ ] Session `runUntilHalt` with TTY polling
- [ ] CLI (`--machine`, `--dump-mem`, `--dump-reg`, `--help`, validation)
- [ ] Builtin preset registry (contracts, factories, load-address aliases)
- [ ] Machine clock (0 Hz = unlimited) and real-time pacing
- [ ] Frame runner (`stepForFrame` / pace) so halted CPUs still refresh video
- [ ] Load address aliases (`rom` / `ram` per preset; omitted address uses contract default)
- [ ] Multiple `--load-bin`; execution starts after all loads complete

## Virtual screens and frontends

- [ ] Framebuffer interface (Device subclass, RGBA, dirty rects, snow fill)
- [ ] Screen registry (machines request a virtual screen by id, or none for headless)
- [ ] Refresh helper so display machines redraw while the CPU is halted
- [ ] Host keyboard bridge (SDL/Qt6 dispatch into the current machine)
- [ ] `mojave` TTY frontend — headless; basic presets `z80`, `m6502`, `m6507`
- [ ] `mojave-sdl` — standalone virtual screen (snow pattern, `--vsd`, `--scale`)
- [ ] `mojave-qt6` — Qt6 MDI shell with an always-visible virtual screen widget

## Basic presets

Headless TTY machines. No `docs/machines/` entry.

- [ ] `z80` — Z80, 64 KiB RAM, Virtual TTY (port I/O)
- [ ] `m6502` — 6502, 64 KiB RAM, Virtual TTY at 0xFFE0; default load 0x0000
- [ ] `m6507` — 6507, 8 KiB physical mirror, Virtual TTY at 0x1E00

## TRS-80 Model I Level I (`trs80m1l1`)

Family devices live under `devices/trs80m1/` (shared with Level II).

- [ ] Hardware notes (`docs/machines/trs80m1.md`)
- [ ] Profile contract + memory map (4 KiB ROM, 4 KiB RAM; no expansion)
- [ ] Video controller (64×16 text VRAM, character generator, uppercase, block graphics)
- [ ] Memory-mapped printer status (0x37E8)
- [ ] Port 0xFF decode (cassette bits idle, 32-column screen mode)
- [ ] Keyboard matrix device (8 row addresses; no CONTROL/CAPS LOCK)
- [ ] Machine clock 1.774 MHz
- [ ] Plug into SDL/Qt6 virtual screen
- [ ] Wire keyboard matrix to SDL/Qt6 host input
- [ ] Host layout keyboard typing (SDL/Qt6)
- [ ] 32-column text mode

## TRS-80 Model I Level II (`trs80m1l2`)

Reuse `devices/trs80m1/`; Level II-only expansion stays under `devices/trs80m1l2/`.

- [ ] Profile contract + memory map (12 KiB ROM, 16 KiB base RAM, expansion to 48 KiB)
- [ ] Video controller (reuse Model I family: 64×16, uppercase, block graphics)
- [ ] Memory-mapped printer status (0x37E8)
- [ ] Port 0xFF decode (cassette bits idle, 32-column screen mode)
- [ ] Keyboard matrix device (reuse Model I family)
- [ ] Wire keyboard matrix to SDL/Qt6 host input
- [ ] Machine clock 1.774 MHz
- [ ] Plug into SDL/Qt6 virtual screen
- [ ] Host layout keyboard typing (SDL/Qt6)
- [ ] Expansion RAM map (32 KiB at 0x8000)
- [ ] Expansion interface port decode (E8h–EFh printer and serial idle stub)
- [ ] 32-column text mode

## TRS-80 Model III (`trs80m3`)

Own device folder (`devices/trs80m3/`). Do not include Model I family headers.

- [ ] Hardware notes (`docs/machines/trs80m3.md`)
- [ ] Profile contract + memory map (14 KiB ROM, 48 KiB RAM, I/O latch window)
- [ ] Video: lowercase glyphs and Model III framebuffer (512×192, block graphics)
- [ ] Memory-mapped I/O latches (cassette idle, disk, printer idle)
- [ ] Port I/O decode (E0h–EFh interrupt latch, NMI, hardware control)
- [ ] Keyboard matrix additions (CONTROL, CAPS LOCK)
- [ ] Machine clock 2.03 MHz + 30 Hz maskable interrupt
- [ ] Floppy controller stub (F0h–F4h idle) and disk-index NMI path
- [ ] RS-232 serial port stub (E8h–EBh)
- [ ] Plug into SDL/Qt6 virtual screen
- [ ] 32-column text / 128×48 graphics mode
- [ ] Host layout keyboard typing (SDL/Qt6)

## Sinclair ZX-80 (`zx80`)

Family devices (`devices/sinclair/`): membrane keyboard, cassette stub, host typing.
ULA, display file, and charset stay under `devices/zx80/`.

- [ ] Hardware notes (`docs/machines/zx80.md`)
- [ ] Profile contract + memory map (4 KiB ROM at 0x0000, 1 KiB RAM at 0x4000)
- [ ] Display file layout (DFILE: HALT-prefixed lines, 32 columns; `D_FILE` at 0x400C)
- [ ] ROM character bitmap reader (64 glyphs × 8×8 from 4 KiB ROM)
- [ ] Video generator (50 Hz, DFILE → 256×192 framebuffer; v1 frame tick)
- [ ] Keyboard matrix (40 keys, 8×5; IN read with row from address high byte)
- [ ] Cassette EAR/MIC interface (port decode; idle levels, no image transport)
- [ ] Machine clock 3.25 MHz
- [ ] Plug into SDL/Qt6 virtual screen
- [ ] Wire keyboard matrix to SDL/Qt6 host input
- [ ] Host layout keyboard typing (SDL/Qt6; ZX key legends)

## Sinclair ZX-81 (`zx81`)

Reuse `devices/sinclair/`. ULA/SLOW NMI and RAM mirror stay under `devices/zx81/`.

- [ ] Hardware notes (`docs/machines/zx81.md`)
- [ ] Profile contract + memory map (8 KiB ROM at 0x0000, 1 KiB RAM at 0x4000)
- [ ] Display file layout (DFILE: HALT-prefixed lines, 32 columns; `D_FILE` at 0x400C)
- [ ] ROM character bitmap reader (64 glyphs × 8×8 from 8 KiB ROM)
- [ ] ULA video (50 Hz, DFILE → 256×192; FAST blanks, SLOW keeps display via NMI)
- [ ] RAM mirror / VRAM trap (0xC000+)
- [ ] Keyboard matrix (reuse Sinclair family)
- [ ] Cassette EAR/MIC interface (idle levels, no image transport)
- [ ] Machine clock 3.25 MHz + NMI for SLOW mode
- [ ] Plug into SDL/Qt6 virtual screen
- [ ] Wire keyboard matrix to SDL/Qt6 host input
- [ ] Host layout keyboard typing (SDL/Qt6; ZX key legends)

## Removable media

Design is in `docs/shared/`. Guest cassette/disk ports today are idle stubs.
`--load-bin` is a developer memory inject, not tape or disk.

- [ ] Host attach model documented (`docs/shared/removable-media.md`)
- [ ] Cassette image design documented (`docs/shared/cassette-images.md`)
- [ ] Diskette and cartridge placeholders documented
- [ ] Shared attach/eject API (session + same actions in SDL and Qt6)
- [ ] Cassette image transport (play/pause/rewind/record; replace idle EAR/MIC stubs)
- [ ] Diskette images and a real FDC for `trs80m3` (replace floppy stub)
- [ ] Cartridge image attach for later cart machines

## Upcoming machines

Write `docs/machines/<id>.md` before any implementation checkbox for that id.

- [ ] `zxspectrum` — hardware notes (`docs/machines/zxspectrum.md`)
- [ ] `zxspectrum` — Z80, 48K, ULA, display, keyboard, tape
- [ ] `c64` — hardware notes (`docs/machines/c64.md`)
- [ ] `c64` — MOS 6510, VIC-II, SID, CIA, datasette, disk
- [ ] `nes` — hardware notes (`docs/machines/nes.md`)
- [ ] `nes` — MOS 6502 clone, PPU, APU, controllers
- [ ] `sms` — hardware notes (`docs/machines/sms.md`)
- [ ] `sms` — Z80, VDP, PSG, controllers
- [ ] `msx1` — hardware notes (`docs/machines/msx1.md`)
- [ ] `msx1` — Z80, TMS9918, AY-3-8910, keyboard, tape
