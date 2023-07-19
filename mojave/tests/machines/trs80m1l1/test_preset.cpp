#include <cstdint>
#include "catch.hpp"
#include "machines/trs80m1l1/trs80m1l1_preset.hpp"
#include "machines/shared/machine.hpp"
#include "cpus/z80.hpp"
#include "devices/trs80m1/printer_status.hpp"
#include "devices/trs80m1/system_port.hpp"
#include "devices/trs80m1/keyboard.hpp"

using Contract = Trs80M1L1PresetContract;

TEST_CASE("TRS-80 Model I Level I preset contract memory map", "[machine][trs80m1l1][fast]") {
    REQUIRE(Contract::ram_bytes == 4096);
    REQUIRE(Contract::guest_cpu_clock_hz == 1'774'080);
    REQUIRE_FALSE(Contract::includes_virtual_tty);
    REQUIRE(Contract::needs_virtual_screen);

    REQUIRE(Contract::rom_start == 0x0000);
    REQUIRE(Contract::rom_end_exclusive == 0x1000);
    REQUIRE(Contract::ram_start == 0x4000);
    REQUIRE(Contract::ram_end_exclusive == 0x5000);

    REQUIRE(Contract::printer_status_address == 0x37E8);
    REQUIRE(Contract::keyboard_start == 0x3800);
    REQUIRE(Contract::keyboard_end_exclusive == 0x3C00);
    REQUIRE(Contract::vram_start == 0x3C00);
    REQUIRE(Contract::vram_end_exclusive == 0x4000);

    REQUIRE(Contract::load_rom_address == 0x0000);
    REQUIRE(Contract::load_rom_end_exclusive == 0x1000);
    REQUIRE(Contract::load_ram_address == 0x4000);
    REQUIRE(Contract::load_ram_end_exclusive == 0x5000);
    REQUIRE(Contract::default_omitted_load_address == 0x0000);
}

TEST_CASE("TRS-80 Model I Level I preset id", "[machine][trs80m1l1][fast]") {
    REQUIRE(isTrs80M1L1PresetId("trs80m1l1"));
    REQUIRE_FALSE(isTrs80M1L1PresetId("trs80m1l2"));
    REQUIRE_FALSE(isTrs80M1L1PresetId("z80"));
}

TEST_CASE("TRS-80 Model I Level I machine maps 4 KiB ROM and 4 KiB RAM", "[machine][trs80m1l1][fast]") {
    auto machine = createTrs80M1L1Machine();
    REQUIRE(machine != nullptr);
    REQUIRE(dynamic_cast<Z80*>(&machine->cpu()) != nullptr);

    machine->bus().write(0x0000, 0xAA);
    REQUIRE(machine->bus().read(0x0000) == 0x00);
    REQUIRE(machine->bus().read(0x0FFF) == 0x00);

    machine->bus().write(0x4000, 0x11);
    machine->bus().write(0x4FFF, 0x22);
    REQUIRE(machine->bus().read(0x4000) == 0x11);
    REQUIRE(machine->bus().read(0x4FFF) == 0x22);
}

TEST_CASE("TRS-80 Model I Level I machine maps 64x16 VRAM at 0x3C00", "[machine][trs80m1l1][fast]") {
    auto machine = createTrs80M1L1Machine();

    machine->bus().write(Contract::vram_start, 'H');
    REQUIRE(machine->bus().read(Contract::vram_start) == 'H');
    machine->bus().write(Contract::vram_end_exclusive - 1, 'I');
    REQUIRE(machine->bus().read(Contract::vram_end_exclusive - 1) == 'I');
}

TEST_CASE("TRS-80 Model I Level I machine maps printer status at 0x37E8", "[machine][trs80m1l1][fast]") {
    auto machine = createTrs80M1L1Machine();

    REQUIRE(machine->bus().read(Contract::printer_status_address) == Trs80M1PrinterStatus::kIdleReadValue);
    machine->bus().write(Contract::printer_status_address, 0x55);
    REQUIRE(machine->bus().read(Contract::printer_status_address) == Trs80M1PrinterStatus::kIdleReadValue);
}

TEST_CASE("TRS-80 Model I Level I machine decodes system port at 0xFF", "[machine][trs80m1l1][fast]") {
    auto machine = createTrs80M1L1Machine();

    machine->bus().writePort(Contract::system_port, 0x07);
    REQUIRE(machine->bus().readPort(Contract::system_port) == Trs80M1SystemPort::kIdleCassetteRead);

    machine->bus().writePort(Contract::system_port, 0x0F);
    Trs80M1SystemPort* port = nullptr;
    for (const auto& dev : machine->ownedDevices()) {
        if (auto* found = dynamic_cast<Trs80M1SystemPort*>(dev.get())) {
            port = found;
            break;
        }
    }
    REQUIRE(port != nullptr);
    REQUIRE(port->motorOn());
    REQUIRE(port->wideScreen());
}

TEST_CASE("TRS-80 Model I Level I machine maps keyboard matrix at 0x3800", "[machine][trs80m1l1][fast]") {
    auto machine = createTrs80M1L1Machine();

    Trs80M1Keyboard* keyboard = nullptr;
    for (const auto& dev : machine->ownedDevices()) {
        if (auto* found = dynamic_cast<Trs80M1Keyboard*>(dev.get())) {
            keyboard = found;
            break;
        }
    }
    REQUIRE(keyboard != nullptr);

    keyboard->pressNamedKey('B');
    REQUIRE(machine->bus().read(Trs80M1Keyboard::rowAddress(0)) == 0x04);
    keyboard->releaseNamedKey('B');
    REQUIRE(machine->bus().read(Trs80M1Keyboard::rowAddress(0)) == 0x00);
}

TEST_CASE("TRS-80 Model I Level I unmapped regions read as floating bus 0xFF", "[machine][trs80m1l1][fast]") {
    auto machine = createTrs80M1L1Machine();

    REQUIRE(machine->bus().read(0x1000) == 0xFF);
    REQUIRE(machine->bus().read(0x5000) == 0xFF);
    REQUIRE(machine->bus().read(0xFFFF) == 0xFF);

    machine->bus().write(0x2000, 0x55);
    REQUIRE(machine->bus().read(0x2000) == 0xFF);
}

TEST_CASE("TRS-80 Model I Level I has no expansion RAM above base map", "[machine][trs80m1l1][fast]") {
    auto machine = createTrs80M1L1Machine();

    machine->bus().write(0x8000, 0xCC);
    REQUIRE(machine->bus().read(0x8000) == 0xFF);
    machine->bus().write(0xFFFF, 0xDD);
    REQUIRE(machine->bus().read(0xFFFF) == 0xFF);
}
