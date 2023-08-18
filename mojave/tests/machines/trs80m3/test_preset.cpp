#include <cstdint>
#include "catch.hpp"
#include "machines/trs80m3/trs80m3_preset.hpp"
#include "machines/shared/machine.hpp"
#include "cpus/z80.hpp"
#include "devices/shared/framebuffer.hpp"
#include "devices/trs80m3/video_controller.hpp"
#include "devices/trs80m3/io_latches.hpp"
#include "devices/trs80m3/port_decode.hpp"

using Contract = Trs80M3PresetContract;

TEST_CASE("TRS-80 Model III preset contract memory map", "[machine][trs80m3][fast]") {
    REQUIRE(Contract::ram_bytes == 49152);
    REQUIRE(Contract::guest_cpu_clock_hz == 2'027'520);
    REQUIRE(Contract::rtc_interrupt_hz == 30);
    REQUIRE_FALSE(Contract::includes_virtual_tty);
    REQUIRE(Contract::needs_virtual_screen);

    REQUIRE(Contract::rom_start == 0x0000);
    REQUIRE(Contract::io_latch_start == 0x37E0);
    REQUIRE(Contract::io_latch_end_exclusive == 0x37F0);
    REQUIRE(Contract::rom_tail_start == 0x37F0);
    REQUIRE(Contract::rom_end_exclusive == 0x3800);
    REQUIRE(Contract::ram_start == 0x4000);
    REQUIRE(Contract::ram_end_exclusive == 65536u);

    REQUIRE(Contract::printer_status_address == 0x37E8);
    REQUIRE(Contract::keyboard_start == 0x3800);
    REQUIRE(Contract::keyboard_end_exclusive == 0x3C00);
    REQUIRE(Contract::vram_start == 0x3C00);
    REQUIRE(Contract::vram_end_exclusive == 0x4000);
    REQUIRE(Contract::port_decode_start == 0xE0);
    REQUIRE(Contract::port_decode_end_exclusive == 0xF0);
    REQUIRE(Contract::interrupt_latch_port_start == 0xE0);
    REQUIRE(Contract::interrupt_latch_port_end_exclusive == 0xE4);
    REQUIRE(Contract::hardware_control_port_start == 0xEC);
    REQUIRE(Contract::hardware_control_port_end_exclusive == 0xF0);
    REQUIRE(Contract::cassette_screen_port_start == 0xFC);
    REQUIRE(Contract::cassette_screen_port_end_exclusive == 0x100);
    REQUIRE(Contract::floppy_port_start == 0xF0);
    REQUIRE(Contract::floppy_port_end_exclusive == 0xF5);

    REQUIRE(Contract::load_rom_address == 0x0000);
    REQUIRE(Contract::load_rom_end_exclusive == 0x3800);
    REQUIRE(Contract::load_ram_address == 0x4000);
    REQUIRE(Contract::load_ram_end_exclusive == 65536u);
    REQUIRE(Contract::default_omitted_load_address == 0x0000);
}

TEST_CASE("TRS-80 Model III preset id", "[machine][trs80m3][fast]") {
    REQUIRE(isTrs80M3PresetId("trs80m3"));
    REQUIRE_FALSE(isTrs80M3PresetId("trs80m1l2"));
    REQUIRE_FALSE(isTrs80M3PresetId("z80"));
}

TEST_CASE("TRS-80 Model III machine maps 14 KiB ROM and 48 KiB RAM", "[machine][trs80m3][fast]") {
    auto machine = createTrs80M3Machine();
    REQUIRE(machine != nullptr);
    REQUIRE(dynamic_cast<Z80*>(&machine->cpu()) != nullptr);
    REQUIRE(machine->guestCpuClockHz() == Contract::guest_cpu_clock_hz);

    machine->bus().write(0x0000, 0xAA);
    REQUIRE(machine->bus().read(0x0000) == 0x00);
    REQUIRE(machine->bus().read(0x37DF) == 0x00);
    REQUIRE(machine->bus().read(0x37F0) == 0x00);
    REQUIRE(machine->bus().read(0x37FF) == 0x00);

    machine->bus().write(0x4000, 0x11);
    machine->bus().write(0x7FFF, 0x22);
    REQUIRE(machine->bus().read(0x4000) == 0x11);
    REQUIRE(machine->bus().read(0x7FFF) == 0x22);

    machine->bus().write(0x8000, 0x33);
    machine->bus().write(0xFFFF, 0x44);
    REQUIRE(machine->bus().read(0x8000) == 0x33);
    REQUIRE(machine->bus().read(0xFFFF) == 0x44);
}

TEST_CASE("TRS-80 Model III machine maps memory-mapped I/O latches at 37E0",
          "[machine][trs80m3][fast]") {
    auto machine = createTrs80M3Machine();

    REQUIRE(machine->bus().read(Contract::printer_status_address) == Trs80M3IoLatches::kPrinterIdleReadValue);
    machine->bus().write(Contract::printer_status_address, 0x55);
    REQUIRE(machine->bus().read(Contract::printer_status_address) == Trs80M3IoLatches::kPrinterIdleReadValue);

    machine->bus().write(0x37E1, 0x01);
    REQUIRE(machine->bus().read(0x37E1) == 0x01);

    machine->bus().write(0x37E2, 0x04);
    REQUIRE(machine->bus().read(0x37E2) == 0x04);

    machine->bus().write(0x37E4, 0x01);
    REQUIRE(machine->bus().read(0x37E4) == 0x01);

    machine->bus().write(0x37EC, 0x88);
    REQUIRE(machine->bus().read(0x37EC) == 0x88);
    machine->bus().write(0x37ED, 0x0A);
    REQUIRE(machine->bus().read(0x37ED) == 0x0A);

    Trs80M3IoLatches* latches = nullptr;
    for (const auto& dev : machine->ownedDevices()) {
        if (auto* found = dynamic_cast<Trs80M3IoLatches*>(dev.get())) {
            latches = found;
            break;
        }
    }
    REQUIRE(latches != nullptr);
    REQUIRE(latches->diskCommand() == 0x88);
    REQUIRE(latches->lastPrinterByte() == 0x55);
}

TEST_CASE("TRS-80 Model III unmapped keyboard address reads as floating bus 0xFF",
          "[machine][trs80m3][fast]") {
    auto machine = createTrs80M3Machine();

    REQUIRE(machine->bus().read(Contract::keyboard_start) == 0xFF);
    machine->bus().write(Contract::keyboard_start, 0x55);
    REQUIRE(machine->bus().read(Contract::keyboard_start) == 0xFF);
}

TEST_CASE("TRS-80 Model III machine maps 64x16 VRAM at 0x3C00", "[machine][trs80m3][fast]") {
    auto machine = createTrs80M3Machine();

    machine->bus().write(Contract::vram_start, 'H');
    REQUIRE(machine->bus().read(Contract::vram_start) == 'H');
    machine->bus().write(Contract::vram_end_exclusive - 1, 'I');
    REQUIRE(machine->bus().read(Contract::vram_end_exclusive - 1) == 'I');
}

TEST_CASE("TRS-80 Model III video controller renders uppercase and lowercase text",
          "[machine][trs80m3][fast]") {
    auto machine = createTrs80M3Machine();

    Framebuffer* fb = nullptr;
    for (const auto& dev : machine->ownedDevices()) {
        if (auto* found = dynamic_cast<Framebuffer*>(dev.get())) {
            fb = found;
            break;
        }
    }
    REQUIRE(fb != nullptr);
    REQUIRE(fb->width() == Trs80M3VideoController::kFramebufferWidth);
    REQUIRE(fb->height() == Trs80M3VideoController::kFramebufferHeight);

    machine->reset();
    REQUIRE(fb->getPixel(0, 0) == 0xFF000000u);

    machine->bus().write(Contract::vram_start, 'A');
    REQUIRE(fb->getPixel(3, 2) == 0xFFFFFFFFu);

    machine->bus().write(static_cast<uint16_t>(Contract::vram_start + 1), 'a');
    REQUIRE(fb->getPixel(11, 4) == 0xFFFFFFFFu);
    REQUIRE(fb->getPixel(11, 2) == 0xFF000000u);
}

TEST_CASE("TRS-80 Model III machine renders block graphics in VRAM", "[machine][trs80m3][fast]") {
    auto machine = createTrs80M3Machine();

    Framebuffer* fb = nullptr;
    for (const auto& dev : machine->ownedDevices()) {
        if (auto* found = dynamic_cast<Framebuffer*>(dev.get())) {
            fb = found;
            break;
        }
    }
    REQUIRE(fb != nullptr);

    machine->reset();
    machine->bus().write(Contract::vram_start, 161);
    REQUIRE(fb->getPixel(1, 1) == 0xFFFFFFFFu);
    REQUIRE(fb->getPixel(2, 7) == 0xFFFFFFFFu);
}

TEST_CASE("TRS-80 Model III machine decodes ports E0-EF", "[machine][trs80m3][fast]") {
    auto machine = createTrs80M3Machine();

    machine->bus().writePort(Contract::interrupt_latch_port_start, Trs80M3PortDecode::kRtcInterruptMask);
    REQUIRE(machine->bus().readPort(Contract::interrupt_latch_port_start) == 0xFF);

    Trs80M3PortDecode* ports = nullptr;
    for (const auto& dev : machine->ownedDevices()) {
        if (auto* found = dynamic_cast<Trs80M3PortDecode*>(dev.get())) {
            ports = found;
            break;
        }
    }
    REQUIRE(ports != nullptr);
    REQUIRE(ports->rtcEnabled());

    ports->setRtcPending(true);
    REQUIRE(machine->bus().readPort(0xE1) ==
            static_cast<uint8_t>(~Trs80M3PortDecode::kRtcInterruptMask));

    const uint8_t control = static_cast<uint8_t>(Trs80M3PortDecode::kDoubleWidthMask |
                                                 Trs80M3PortDecode::kVideoWaitMask);
    machine->bus().writePort(Contract::hardware_control_port_start, control);
    REQUIRE(ports->doubleWidth());
    REQUIRE(ports->videoWaitsEnabled());
    REQUIRE(ports->cassetteMotorOn());
    REQUIRE(machine->bus().readPort(Contract::hardware_control_port_start) == 0xFF);
    REQUIRE(ports->interruptStatus() == 0xFF);

    REQUIRE(machine->bus().readPort(0xE8) == Trs80M3PortDecode::kRs232IdleStatus);
}

TEST_CASE("TRS-80 Model III runs inline NOP HALT program in RAM", "[machine][trs80m3][fast]") {
    auto machine = createTrs80M3Machine();

    machine->bus().write(0x4000, 0x00); // NOP
    machine->bus().write(0x4001, 0x76); // HALT
    machine->reset();
    dynamic_cast<Z80&>(machine->cpu()).regs().pc = 0x4000;

    REQUIRE_FALSE(machine->cpu().halted());
    machine->step();
    REQUIRE_FALSE(machine->cpu().halted());
    machine->step();
    REQUIRE(machine->cpu().halted());
}
