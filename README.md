# Mojave

Cycle-accurate multi-system emulator for classic 8-bit computers.

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
├── tests/              # Catch2 suite (mirrors src/ layout)
├── lib/                # Vendored Catch2 header
.make/               # Build scripts
```

## Development

```bash
make setup           # Download dependencies (Catch2)
make build-tty       # Compile headless mojave binary
make test-all        # Full suite (commit gate)
```

## License

This project is licensed under the GNU General Public License v3.0 — see [LICENSE](LICENSE) for details.
