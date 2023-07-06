#include <cstdint>
#include "catch.hpp"
#include "machines/z80/z80_preset.hpp"
#include "machines/shared/machine.hpp"
#include "cpus/z80.hpp"
#include "devices/shared/virtual_tty.hpp"

TEST_CASE("Z80 Preset Machine Integration", "[machine][z80][fast]") {
    auto machine = createZ80Machine();
    REQUIRE(machine != nullptr);

    // Verify CPU family is Z80
    REQUIRE(dynamic_cast<Z80*>(&machine->cpu()) != nullptr);

    // Verify 64 KiB RAM mapping (0x0000 - 0xFFFF)
    machine->bus().write(0x0000, 0xAA);
    REQUIRE(machine->bus().read(0x0000) == 0xAA);

    machine->bus().write(0x7FFF, 0xBB);
    REQUIRE(machine->bus().read(0x7FFF) == 0xBB);

    machine->bus().write(0x8000, 0xCC);
    REQUIRE(machine->bus().read(0x8000) == 0xCC);

    machine->bus().write(0xFFFF, 0xDD);
    REQUIRE(machine->bus().read(0xFFFF) == 0xDD);

    // Verify VirtualTTY mapping on ports 0 and 1
    // Write character to TTY data port (port 0)
    machine->bus().writePort(0x0000, 'H');
    // Find VirtualTTY in owned devices and read the TX char
    VirtualTTY* tty = nullptr;
    for (const auto& dev : machine->ownedDevices()) {
        if (auto t = dynamic_cast<VirtualTTY*>(dev.get())) {
            tty = t;
            break;
        }
    }
    REQUIRE(tty != nullptr);
    REQUIRE(tty->readChar() == 'H');

    // Run tiny Z80 program
    // NOP (0x00), HALT (0x76)
    machine->bus().write(0x0000, 0x00); // NOP
    machine->bus().write(0x0001, 0x76); // HALT
    machine->reset(); // PC = 0x0000

    REQUIRE_FALSE(machine->cpu().halted());
    unsigned cycles1 = machine->step(); // Run NOP
    REQUIRE(cycles1 == 4);
    REQUIRE_FALSE(machine->cpu().halted());

    unsigned cycles2 = machine->step(); // Run HALT
    REQUIRE(cycles2 == 4);
    REQUIRE(machine->cpu().halted());
}
