# Mojave

Cycle-accurate multi-system emulator for classic 8-bit computers.

## Highlights

- Z80, MOS 6502, MOS 6507 CPU emulation with documented and undocumented opcodes
- Composite bus with separate memory and port I/O address spaces
- Physical memory with read-only ROM regions and out-of-bounds safety
- Device tick system for cycle-accurate timing of peripherals
- Virtual TTY for guest-host I/O via port or memory mapping
- Plug-in virtual screens: any display-capable machine writes to a shared framebuffer

## Prerequisites

- **C++20** compiler (g++ 12+ or clang 16+)
- **make** — build system

## Installation

### Build from Source

```bash
git clone https://github.com/username/mojave.git
cd mojave
make setup          # Download Catch2 test header
make build-tty      # Build the headless mojave binary
```

## Usage

```bash
bin/mojave --help                         # Show usage
```

## Project Layout

```
mojave/              # Component root
├── src/
│   ├── bus/
│   ├── cpus/
│   ├── devices/
│   ├── machines/
│   ├── session/
├── tests/              # Catch2 suite (mirrors src/ layout)
├── lib/                # Vendored Catch2 header
.make/               # Build scripts
```

## Development

```bash
make setup           # Download dependencies (Catch2)
make build-tty       # Compile headless mojave binary
make test-fast       # Incremental compile; run [fast] tests
make test-changed    # Incremental compile; tests for dirty mojave/ paths
make test-all        # Full suite (commit gate)
make check           # Syntax check and isolation guard
make quality         # Syntax check, build, and test
make coverage        # Build and run tests with coverage report
```

## License

This project is licensed under the GNU General Public License v3.0 — see [LICENSE](LICENSE) for details.
