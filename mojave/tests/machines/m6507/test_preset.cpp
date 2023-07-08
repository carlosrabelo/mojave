#include <cstdint>
#include "catch.hpp"
#include "machines/m6507/m6507_preset.hpp"
#include "machines/shared/machine.hpp"
#include "cpus/m6507.hpp"
#include "devices/shared/virtual_tty.hpp"

TEST_CASE("M6507 Preset Machine Integration", "[machine][m6507][fast]") {
    auto machine = createM6507Machine();
    REQUIRE(machine != nullptr);

    // Verify CPU family is M6507
    REQUIRE(dynamic_cast<M6507*>(&machine->cpu()) != nullptr);

    // Verify 8 KiB RAM mapping (excluding TTY window 0x1E00-0x1E01)
    machine->bus().write(0x0000, 0xAA);
    REQUIRE(machine->bus().read(0x0000) == 0xAA);

    machine->bus().write(0x1DFF, 0xBB);
    REQUIRE(machine->bus().read(0x1DFF) == 0xBB);

    machine->bus().write(0x1E02, 0x11);
    REQUIRE(machine->bus().read(0x1E02) == 0x11);

    machine->bus().write(0x1FFF, 0xDD);
    REQUIRE(machine->bus().read(0x1FFF) == 0xDD);

    // Verify VirtualTTY mapping on memory 0x1E00 - 0x1E01 (corresponds to physical 0xFE00 under 13-bit mask)
    machine->bus().write(0x1E00, 'Z');

    // Find VirtualTTY in owned devices and read the TX char
    VirtualTTY* tty = nullptr;
    for (const auto& dev : machine->ownedDevices()) {
        if (auto t = dynamic_cast<VirtualTTY*>(dev.get())) {
            tty = t;
            break;
        }
    }
    REQUIRE(tty != nullptr);
    REQUIRE(tty->readChar() == 'Z');

    // Run tiny M6507 program via reset vector
    // Reset vector for 6507 resides at masked 0x1FFC/0x1FFD
    machine->bus().write(0x1FFC, 0x00);
    machine->bus().write(0x1FFD, 0x02); // PC points to 0x0200

    // Write NOPs at 0x0200
    machine->bus().write(0x0200, 0xEA); // NOP
    machine->bus().write(0x0201, 0xEA); // NOP

    machine->reset(); // PC vira 0x0200
    M6507& m6507_cpu = static_cast<M6507&>(machine->cpu());
    REQUIRE(m6507_cpu.regs().pc == 0x0200);

    unsigned cycles1 = machine->step(); // Run NOP
    REQUIRE(cycles1 == 2);
    REQUIRE(m6507_cpu.regs().pc == 0x0201);

    unsigned cycles2 = machine->step(); // Run second NOP
    REQUIRE(cycles2 == 2);
    REQUIRE(m6507_cpu.regs().pc == 0x0202);
}
