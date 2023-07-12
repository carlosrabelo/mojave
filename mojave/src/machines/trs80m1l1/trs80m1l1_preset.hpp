#ifndef MOJAVE_TRS80M1L1_PRESET_HPP
#define MOJAVE_TRS80M1L1_PRESET_HPP

#include <cstdint>
#include <memory>
#include <string>

struct Trs80M1L1PresetContract {
    static constexpr uint32_t ram_bytes = 4096; // 4 KiB base RAM
    static constexpr bool includes_virtual_tty = false;
    static constexpr bool needs_virtual_screen = true;
    static constexpr uint32_t guest_cpu_clock_hz = 1'774'080; // Z80A @ 1.774 MHz

    // ROM — Level I BASIC (4 KiB, no expansion)
    static constexpr uint16_t rom_start = 0x0000;
    static constexpr uint16_t rom_end_exclusive = 0x1000;

    // RAM — 4 KiB workspace
    static constexpr uint16_t ram_start = 0x4000;
    static constexpr uint16_t ram_end_exclusive = 0x5000;

    // Device regions (wired in later TODO items)
    static constexpr uint16_t printer_status_address = 0x37E8;
    static constexpr uint16_t keyboard_start = 0x3800;
    static constexpr uint16_t keyboard_end_exclusive = 0x3C00;
    static constexpr uint16_t vram_start = 0x3C00;
    static constexpr uint16_t vram_end_exclusive = 0x4000;

    static constexpr uint16_t system_port = 0x00FF;

    static constexpr uint16_t load_rom_address = rom_start;
    static constexpr uint16_t load_rom_end_exclusive = rom_end_exclusive;
    static constexpr uint16_t load_ram_address = ram_start;
    static constexpr uint16_t load_ram_end_exclusive = ram_end_exclusive;
    static constexpr uint16_t default_omitted_load_address = rom_start;
};

class Machine;

std::unique_ptr<Machine> createTrs80M1L1Machine();
bool isTrs80M1L1PresetId(const std::string& id);

#endif
