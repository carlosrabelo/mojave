#include "catch.hpp"
#include "devices/shared/memory.hpp"
#include "devices/zx81/ram_mirror.hpp"
#include "devices/zx81/display_file.hpp"
#include "machines/zx81/zx81_preset.hpp"
#include "machines/shared/machine.hpp"

TEST_CASE("ZX-81 RAM mirror wraps 1 KiB and forces NOP except NEWLINE", "[device][zx81][fast]") {
    Memory ram(1024);
    Zx81RamMirror mirror(ram);

    ram.write(0x007D, Zx81DisplayFile::kNewline);
    ram.write(0x007E, 0x29); // digit '1' character code

    REQUIRE(mirror.read(0x007D) == Zx81DisplayFile::kNewline);
    REQUIRE(mirror.read(0x007E) == 0x00);
    REQUIRE(mirror.read(0x047D) == Zx81DisplayFile::kNewline); // 1 KiB wrap

    mirror.write(0x0001, 0xAA);
    REQUIRE(ram.read(0x0001) == 0xAA);
}

TEST_CASE("ZX-81 machine maps 0xC000+ onto RAM with the ULA fetch trap", "[machine][zx81][fast]") {
    auto machine = createZx81Machine();

    machine->bus().write(0x4000, Zx81DisplayFile::kNewline);
    machine->bus().write(0x4001, 0x29);

    REQUIRE(machine->bus().read(0xC000) == Zx81DisplayFile::kNewline);
    REQUIRE(machine->bus().read(0xC001) == 0x00);
    REQUIRE(machine->bus().read(0xC400) == Zx81DisplayFile::kNewline);

    machine->bus().write(0xC002, 0xAA);
    REQUIRE(machine->bus().read(0x4002) == 0xAA);
}
