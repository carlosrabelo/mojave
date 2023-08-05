# Machine hardware notes

Canonical hardware and preset notes for Mojave built-in machines.
These files are the source of truth for machine facts used when implementing or
changing a preset — keep them factually correct. Read the matching file before
coding.

| Preset ID | Document |
|-----------|----------|
| `trs80m1l1`, `trs80m1l2` | [trs80m1.md](trs80m1.md) |
| `trs80m3` | [trs80m3.md](trs80m3.md) |

Implementation order and checkboxes live in `TODO.md` at the repository root.
Preset isolation, device families, and naming conventions: [CONTRIBUTING.md](../../CONTRIBUTING.md) and [preset-layout.md](../shared/preset-layout.md).
Basic CPU smoke presets (`z80`, `m6502`, `m6507`) are headless TTY machines and are not covered here.
