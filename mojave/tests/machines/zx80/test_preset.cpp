#include <cstdint>
#include "catch.hpp"
#include "machines/zx80/zx80_preset.hpp"
#include "machines/shared/machine.hpp"
#include "cpus/z80.hpp"
#include "devices/zx80/display_file.hpp"
#include "devices/zx80/character_rom.hpp"
#include "devices/zx80/video_generator.hpp"
#include "devices/zx80/port_decode.hpp"
#include "devices/sinclair/keyboard.hpp"
#include "devices/shared/framebuffer.hpp"

using Contract = Zx80PresetContract;

TEST_CASE("Sinclair ZX-80 preset contract memory map", "[machine][zx80][fast]") {
    REQUIRE(Contract::ram_bytes == 1024);
    REQUIRE(Contract::guest_cpu_clock_hz == 3'250'000);
    REQUIRE_FALSE(Contract::includes_virtual_tty);
    REQUIRE(Contract::needs_virtual_screen);

    REQUIRE(Contract::rom_start == 0x0000);
    REQUIRE(Contract::rom_end_exclusive == 0x1000);
    REQUIRE(Contract::ram_start == 0x4000);
    REQUIRE(Contract::ram_end_exclusive == 0x4400);

    REQUIRE(Contract::sysvar_start == 0x4000);
    REQUIRE(Contract::err_nr_address == 0x4000);
    REQUIRE(Contract::d_file_ptr_address == 0x400C);
    REQUIRE(Contract::dfile_halt_opcode == Zx80DisplayFile::kHaltOpcode);
    REQUIRE(Contract::dfile_columns == Zx80DisplayFile::kColumns);
    REQUIRE(Contract::dfile_rows == Zx80DisplayFile::kRows);
    REQUIRE(Contract::dfile_min_collapsed_bytes == Zx80DisplayFile::kMinCollapsedBytes);
    REQUIRE(Contract::dfile_max_bytes == Zx80DisplayFile::kMaxBytes);
    REQUIRE(Contract::charset_rom_start == Zx80CharacterRom::kCharsetBase);
    REQUIRE(Contract::charset_rom_end_exclusive == Zx80CharacterRom::kCharsetEndExclusive);
    REQUIRE(Contract::charset_i_register == Zx80CharacterRom::kIRegister);
    REQUIRE(Contract::vram_mirror_start == 0xC000);
    REQUIRE(Contract::vram_mirror_end_exclusive == 65536u);

    REQUIRE(Contract::keyboard_port_low_byte == SinclairKeyboard::kPortLowByte);
    REQUIRE(Contract::keyboard_row_count == SinclairKeyboard::kRowCount);
    REQUIRE(Contract::keyboard_bits_per_row == SinclairKeyboard::kBitsPerRow);
    REQUIRE(Contract::io_port_attach_start == 0x0000);
    REQUIRE(Contract::io_port_attach_end_exclusive == 0x0000);

    REQUIRE(Contract::load_rom_address == 0x0000);
    REQUIRE(Contract::load_rom_end_exclusive == 0x1000);
    REQUIRE(Contract::load_ram_address == 0x4000);
    REQUIRE(Contract::load_ram_end_exclusive == 0x4400);
    REQUIRE(Contract::default_omitted_load_address == 0x0000);
}

TEST_CASE("Sinclair ZX-80 preset id", "[machine][zx80][fast]") {
    REQUIRE(isZx80PresetId("zx80"));
    REQUIRE_FALSE(isZx80PresetId("zxspectrum"));
    REQUIRE_FALSE(isZx80PresetId("z80"));
}

TEST_CASE("Sinclair ZX-80 machine maps 4 KiB ROM and 1 KiB RAM", "[machine][zx80][fast]") {
    auto machine = createZx80Machine();
    REQUIRE(machine != nullptr);
    REQUIRE(dynamic_cast<Z80*>(&machine->cpu()) != nullptr);
    REQUIRE(machine->guestCpuClockHz() == Contract::guest_cpu_clock_hz);

    machine->bus().write(0x0000, 0xAA);
    REQUIRE(machine->bus().read(0x0000) == 0x00);
    machine->bus().write(0x0FFF, 0xBB);
    REQUIRE(machine->bus().read(0x0FFF) == 0x00);

    machine->bus().write(0x4000, 0x11);
    machine->bus().write(0x43FF, 0x22);
    REQUIRE(machine->bus().read(0x4000) == 0x11);
    REQUIRE(machine->bus().read(0x43FF) == 0x22);
}

TEST_CASE("Sinclair ZX-80 unmapped addresses read floating bus", "[machine][zx80][fast]") {
    auto machine = createZx80Machine();

    REQUIRE(machine->bus().read(0x1000) == 0xFF);
    REQUIRE(machine->bus().read(0x3FFF) == 0xFF);
    REQUIRE(machine->bus().read(0x4400) == 0xFF);
    REQUIRE(machine->bus().read(0xBFFF) == 0xFF);
    REQUIRE(machine->bus().read(0xC000) == 0xFF);
}

TEST_CASE("Sinclair ZX-80 preset owns framebuffer and video generator", "[machine][zx80][fast]") {
    auto machine = createZx80Machine();

    bool has_framebuffer = false;
    bool has_video = false;
    bool has_keyboard = false;
    bool has_ports = false;
    for (const auto& dev : machine->ownedDevices()) {
        if (dynamic_cast<Framebuffer*>(dev.get()))
            has_framebuffer = true;
        if (dynamic_cast<Zx80VideoGenerator*>(dev.get()))
            has_video = true;
        if (dynamic_cast<SinclairKeyboard*>(dev.get()))
            has_keyboard = true;
        if (dynamic_cast<Zx80PortDecode*>(dev.get()))
            has_ports = true;
    }
    REQUIRE(has_framebuffer);
    REQUIRE(has_video);
    REQUIRE(has_keyboard);
    REQUIRE(has_ports);
}

TEST_CASE("Sinclair ZX-80 machine maps keyboard IN from address high byte", "[machine][zx80][fast]") {
    auto machine = createZx80Machine();

    REQUIRE(machine->bus().readPort(0xFEFE) == 0xFF);
    REQUIRE(machine->bus().readPort(0x7FFE) == 0xFF);
    REQUIRE(machine->bus().readPort(0xFDFF) == 0xFF);

    SinclairKeyboard* keyboard = nullptr;
    for (const auto& dev : machine->ownedDevices()) {
        keyboard = dynamic_cast<SinclairKeyboard*>(dev.get());
        if (keyboard != nullptr)
            break;
    }
    REQUIRE(keyboard != nullptr);

    keyboard->pressKey(SinclairKeyboard::Key::Shift);
    REQUIRE(machine->bus().readPort(0xFEFE) == 0xFE);
    REQUIRE(machine->bus().readPort(0xFDFE) == 0xFF);

    keyboard->releaseAll();
    keyboard->pressKey(SinclairKeyboard::Key::Space);
    REQUIRE(machine->bus().readPort(0x7FFE) == 0xFE);
    REQUIRE(machine->bus().readPort(0xFEFE) == 0xFF);
}

TEST_CASE("Sinclair ZX-80 tiny program runs from RAM", "[machine][zx80][fast]") {
    auto machine = createZx80Machine();

    machine->bus().write(0x4000, 0x00); // NOP
    machine->bus().write(0x4001, 0x76); // HALT
    machine->reset();
    auto* z80 = dynamic_cast<Z80*>(&machine->cpu());
    REQUIRE(z80 != nullptr);
    z80->regs().pc = 0x4000;

    REQUIRE_FALSE(machine->cpu().halted());
    REQUIRE(machine->step() == 4);
    REQUIRE_FALSE(machine->cpu().halted());
    REQUIRE(machine->step() == 4);
    REQUIRE(machine->cpu().halted());
}
