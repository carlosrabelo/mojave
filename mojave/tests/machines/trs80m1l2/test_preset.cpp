#include <cstdint>
#include "catch.hpp"
#include "machines/trs80m1l2/trs80m1l2_preset.hpp"
#include "machines/shared/machine.hpp"
#include "cpus/z80.hpp"
#include "devices/trs80m1/video_controller.hpp"
#include "devices/trs80m1/printer_status.hpp"
#include "devices/trs80m1/system_port.hpp"
#include "devices/trs80m1/keyboard.hpp"
#include "devices/trs80m1l2/expansion_ports.hpp"
#include "devices/shared/framebuffer.hpp"

using Contract = Trs80M1L2PresetContract;

TEST_CASE("TRS-80 Model I Level II preset contract memory map", "[machine][trs80m1l2][fast]") {
    REQUIRE(Contract::base_ram_bytes == 16384);
    REQUIRE(Contract::expansion_ram_bytes == 32768);
    REQUIRE(Contract::ram_bytes == 49152);
    REQUIRE(Contract::guest_cpu_clock_hz == 1'774'080);
    REQUIRE_FALSE(Contract::includes_virtual_tty);
    REQUIRE(Contract::needs_virtual_screen);

    REQUIRE(Contract::rom_start == 0x0000);
    REQUIRE(Contract::rom_end_exclusive == 0x3000);
    REQUIRE(Contract::ram_start == 0x4000);
    REQUIRE(Contract::ram_end_exclusive == 0x8000);
    REQUIRE(Contract::expansion_ram_start == 0x8000);
    REQUIRE(Contract::expansion_ram_end_exclusive == 65536u);

    REQUIRE(Contract::printer_status_address == 0x37E8);
    REQUIRE(Contract::keyboard_start == 0x3800);
    REQUIRE(Contract::keyboard_end_exclusive == 0x3C00);
    REQUIRE(Contract::vram_start == 0x3C00);
    REQUIRE(Contract::vram_end_exclusive == 0x4000);
    REQUIRE(Contract::expansion_port_start == 0xE8);
    REQUIRE(Contract::expansion_port_end_exclusive == 0xF0);

    REQUIRE(Contract::load_rom_address == 0x0000);
    REQUIRE(Contract::load_rom_end_exclusive == 0x3000);
    REQUIRE(Contract::load_ram_address == 0x4000);
    REQUIRE(Contract::load_ram_end_exclusive == 65536u);
    REQUIRE(Contract::default_omitted_load_address == 0x0000);
}

TEST_CASE("TRS-80 Model I Level II preset id", "[machine][trs80m1l2][fast]") {
    REQUIRE(isTrs80M1L2PresetId("trs80m1l2"));
    REQUIRE_FALSE(isTrs80M1L2PresetId("trs80m1l1"));
    REQUIRE_FALSE(isTrs80M1L2PresetId("z80"));
}

TEST_CASE("TRS-80 Model I Level II machine maps 12 KiB ROM and 16 KiB base RAM", "[machine][trs80m1l2][fast]") {
    auto machine = createTrs80M1L2Machine();
    REQUIRE(machine != nullptr);
    REQUIRE(dynamic_cast<Z80*>(&machine->cpu()) != nullptr);

    machine->bus().write(0x0000, 0xAA);
    REQUIRE(machine->bus().read(0x0000) == 0x00);
    REQUIRE(machine->bus().read(0x2FFF) == 0x00);

    machine->bus().write(0x4000, 0x11);
    machine->bus().write(0x7FFF, 0x22);
    REQUIRE(machine->bus().read(0x4000) == 0x11);
    REQUIRE(machine->bus().read(0x7FFF) == 0x22);
}

TEST_CASE("TRS-80 Model I Level II unmapped regions read as floating bus 0xFF", "[machine][trs80m1l2][fast]") {
    auto machine = createTrs80M1L2Machine();

    REQUIRE(machine->bus().read(0x3000) == 0xFF);
    REQUIRE(machine->bus().read(0x37E0) == 0xFF);
    REQUIRE(machine->bus().read(0x3800) == 0x00);

    machine->bus().write(0x3500, 0x55);
    REQUIRE(machine->bus().read(0x3500) == 0xFF);
}

TEST_CASE("TRS-80 Model I Level II machine maps 64x16 VRAM at 0x3C00", "[machine][trs80m1l2][fast]") {
    auto machine = createTrs80M1L2Machine();

    machine->bus().write(Contract::vram_start, 'H');
    REQUIRE(machine->bus().read(Contract::vram_start) == 'H');
    machine->bus().write(Contract::vram_end_exclusive - 1, 'I');
    REQUIRE(machine->bus().read(Contract::vram_end_exclusive - 1) == 'I');
}

TEST_CASE("TRS-80 Model I Level II video controller renders uppercase text", "[machine][trs80m1l2][fast]") {
    auto machine = createTrs80M1L2Machine();

    Framebuffer* fb = nullptr;
    for (const auto& dev : machine->ownedDevices()) {
        if (auto* found = dynamic_cast<Framebuffer*>(dev.get())) {
            fb = found;
            break;
        }
    }
    REQUIRE(fb != nullptr);
    REQUIRE(fb->width() == Trs80M1VideoController::kFramebufferWidth);
    REQUIRE(fb->height() == Trs80M1VideoController::kFramebufferHeight);

    machine->reset();
    REQUIRE(fb->getPixel(0, 0) == 0xFF000000u);

    machine->bus().write(Contract::vram_start, 'A');
    REQUIRE(fb->getPixel(2, 2) == 0xFFFFFFFFu);
    REQUIRE(fb->getPixel(3, 5) == 0xFFFFFFFFu);

    machine->bus().write(static_cast<uint16_t>(Contract::vram_start + 1), 'a');
    REQUIRE(fb->getPixel(8, 2) == 0xFFFFFFFFu);
}

TEST_CASE("TRS-80 Model I Level II video controller renders block graphics", "[machine][trs80m1l2][fast]") {
    auto machine = createTrs80M1L2Machine();

    Framebuffer* fb = nullptr;
    for (const auto& dev : machine->ownedDevices()) {
        if (auto* found = dynamic_cast<Framebuffer*>(dev.get())) {
            fb = found;
            break;
        }
    }
    REQUIRE(fb != nullptr);

    machine->reset();
    machine->bus().write(Contract::vram_start, 129);
    REQUIRE(fb->getPixel(1, 1) == 0xFFFFFFFFu);
    REQUIRE(fb->getPixel(2, 3) == 0xFFFFFFFFu);
    REQUIRE(fb->getPixel(4, 0) == 0xFF000000u);
}

TEST_CASE("TRS-80 Model I Level II machine maps printer status at 0x37E8", "[machine][trs80m1l2][fast]") {
    auto machine = createTrs80M1L2Machine();

    REQUIRE(machine->bus().read(Contract::printer_status_address) == Trs80M1PrinterStatus::kIdleReadValue);
    machine->bus().write(Contract::printer_status_address, 0x55);
    REQUIRE(machine->bus().read(Contract::printer_status_address) == Trs80M1PrinterStatus::kIdleReadValue);
}

TEST_CASE("TRS-80 Model I Level II machine decodes system port at 0xFF", "[machine][trs80m1l2][fast]") {
    auto machine = createTrs80M1L2Machine();

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

TEST_CASE("TRS-80 Model I Level II machine maps keyboard matrix at 0x3800", "[machine][trs80m1l2][fast]") {
    auto machine = createTrs80M1L2Machine();

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

TEST_CASE("TRS-80 Model I Level II machine decodes expansion ports E8-EF", "[machine][trs80m1l2][fast]") {
    auto machine = createTrs80M1L2Machine();

    REQUIRE(machine->bus().readPort(0xE8) == Trs80M1L2ExpansionPorts::kSerialStatusMask);
    machine->bus().writePort(0xE9, 0x55);
    REQUIRE(machine->bus().readPort(0xEA) == Trs80M1L2ExpansionPorts::kUartTxEmptyMask);

    machine->bus().writePort(0xEB, 'X');
    Trs80M1L2ExpansionPorts* ports = nullptr;
    for (const auto& dev : machine->ownedDevices()) {
        if (auto* found = dynamic_cast<Trs80M1L2ExpansionPorts*>(dev.get())) {
            ports = found;
            break;
        }
    }
    REQUIRE(ports != nullptr);
    REQUIRE(ports->lastTxByte() == 'X');

    machine->bus().writePort(0xEC, 'P');
    REQUIRE(ports->lastPrinterByte() == 'P');
    REQUIRE(machine->bus().readPort(0xEC) == Trs80M1L2ExpansionPorts::kPrinterPortStatus);
    REQUIRE(machine->bus().readPort(0xEF) == 0xFF);
}
