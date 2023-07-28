#ifndef MOJAVE_TRS80M1L2_PRESET_HPP
#define MOJAVE_TRS80M1L2_PRESET_HPP

#include <cstdint>
#include <memory>
#include <string>

struct Trs80M1L2PresetContract {
    static constexpr uint32_t base_ram_bytes = 16384;      // 16 KiB base RAM
    static constexpr uint32_t expansion_ram_bytes = 32768;  // 32 KiB expansion RAM
    static constexpr uint32_t ram_bytes = base_ram_bytes + expansion_ram_bytes; // 48 KiB total
    static constexpr bool includes_virtual_tty = false;
    static constexpr bool needs_virtual_screen = true;
    static constexpr uint32_t guest_cpu_clock_hz = 1'774'080; // Z80A @ 1.774 MHz

    // ROM — Level II BASIC (12 KiB)
    static constexpr uint16_t rom_start = 0x0000;
    static constexpr uint16_t rom_end_exclusive = 0x3000;

    // Base RAM — 16 KiB workspace
    static constexpr uint16_t ram_start = 0x4000;
    static constexpr uint16_t ram_end_exclusive = 0x8000;

    // Expansion RAM — 32 KiB via expansion interface (0x8000–0xFFFF)
    static constexpr uint16_t expansion_ram_start = 0x8000;
    static constexpr uint32_t expansion_ram_end_exclusive = 65536u;
    static constexpr uint16_t expansion_ram_bus_end = 0; // bus sentinel for 65536

    // Device regions (wired in later TODO items)
    static constexpr uint16_t printer_status_address = 0x37E8;
    static constexpr uint16_t keyboard_start = 0x3800;
    static constexpr uint16_t keyboard_end_exclusive = 0x3C00;
    static constexpr uint16_t vram_start = 0x3C00;
    static constexpr uint16_t vram_end_exclusive = 0x4000;

    static constexpr uint16_t system_port = 0x00FF;

    static constexpr uint16_t expansion_port_start = 0xE8;
    static constexpr uint16_t expansion_port_end_exclusive = 0xF0;

    static constexpr uint16_t load_rom_address = rom_start;
    static constexpr uint16_t load_rom_end_exclusive = rom_end_exclusive;
    static constexpr uint16_t load_ram_address = ram_start;
    static constexpr uint32_t load_ram_end_exclusive = expansion_ram_end_exclusive;
    static constexpr uint16_t default_omitted_load_address = rom_start;
};

class Machine;

std::unique_ptr<Machine> createTrs80M1L2Machine();
bool isTrs80M1L2PresetId(const std::string& id);

#endif
