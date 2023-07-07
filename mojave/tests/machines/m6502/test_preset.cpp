#include <cstdint>
#include "catch.hpp"
#include "machines/m6502/m6502_preset.hpp"
#include "machines/shared/machine.hpp"
#include "cpus/m6502.hpp"
#include "devices/shared/virtual_tty.hpp"

TEST_CASE("M6502 Preset Machine Integration", "[machine][m6502][fast]") {
    auto machine = createM6502Machine();
    REQUIRE(machine != nullptr);

    // Verify CPU family is M6502
    REQUIRE(dynamic_cast<M6502*>(&machine->cpu()) != nullptr);

    // Verify 64 KiB RAM mapping (excluding TTY window 0xFFE0-0xFFE1)
    machine->bus().write(0x0000, 0xAA);
    REQUIRE(machine->bus().read(0x0000) == 0xAA);

    machine->bus().write(0x7FFF, 0xBB);
    REQUIRE(machine->bus().read(0x7FFF) == 0xBB);

    machine->bus().write(0xFFDF, 0xCC);
    REQUIRE(machine->bus().read(0xFFDF) == 0xCC);

    machine->bus().write(0xFFE2, 0x11);
    REQUIRE(machine->bus().read(0xFFE2) == 0x11);

    machine->bus().write(0xFFFF, 0xDD);
    REQUIRE(machine->bus().read(0xFFFF) == 0xDD);

    // Verify VirtualTTY mapping on memory 0xFFE0 - 0xFFE1
    // Write character to TTY data port (address 0xFFE0)
    machine->bus().write(0xFFE0, 'K');
    
    // Find VirtualTTY in owned devices and read the TX char
    VirtualTTY* tty = nullptr;
    for (const auto& dev : machine->ownedDevices()) {
        if (auto t = dynamic_cast<VirtualTTY*>(dev.get())) {
            tty = t;
            break;
        }
    }
    REQUIRE(tty != nullptr);
    REQUIRE(tty->readChar() == 'K');

    // Run tiny M6502 program via reset vector
    // Vector points to 0x0200
    machine->bus().write(0xFFFC, 0x00);
    machine->bus().write(0xFFFD, 0x02);

    // Write NOPs at 0x0200
    machine->bus().write(0x0200, 0xEA); // NOP
    machine->bus().write(0x0201, 0xEA); // NOP

    machine->reset(); // Loads PC from $FFFC
    M6502& m6502_cpu = static_cast<M6502&>(machine->cpu());
    REQUIRE(m6502_cpu.regs().pc == 0x0200);

    unsigned cycles1 = machine->step(); // Run first NOP
    REQUIRE(cycles1 == 2);
    REQUIRE(m6502_cpu.regs().pc == 0x0201);

    unsigned cycles2 = machine->step(); // Run second NOP
    REQUIRE(cycles2 == 2);
    REQUIRE(m6502_cpu.regs().pc == 0x0202);
}
