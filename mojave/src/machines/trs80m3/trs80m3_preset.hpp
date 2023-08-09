#ifndef MOJAVE_TRS80M3_PRESET_HPP
#define MOJAVE_TRS80M3_PRESET_HPP

#include <cstdint>
#include <memory>
#include <string>

struct Trs80M3PresetContract {
    static constexpr uint32_t ram_bytes = 49152; // 48 KiB system RAM
    static constexpr bool includes_virtual_tty = false;
    static constexpr bool needs_virtual_screen = true;
    static constexpr uint32_t guest_cpu_clock_hz = 2'027'520; // Z80A @ 2.03 MHz
    static constexpr uint32_t rtc_interrupt_hz = 30;
    static constexpr uint32_t rtc_cycles_per_interrupt = guest_cpu_clock_hz / rtc_interrupt_hz;

    // ROM — Model III BASIC (14 KiB), split around memory-mapped I/O at 0x37E0
    static constexpr uint16_t rom_start = 0x0000;
    static constexpr uint16_t io_latch_start = 0x37E0;
    static constexpr uint16_t io_latch_end_exclusive = 0x37F0;
    static constexpr uint16_t rom_tail_start = 0x37F0;
    static constexpr uint16_t rom_end_exclusive = 0x3800;

    // System RAM — 48 KiB workspace
    static constexpr uint16_t ram_start = 0x4000;
    static constexpr uint32_t ram_end_exclusive = 65536u;
    static constexpr uint16_t ram_bus_end = 0; // bus sentinel for 65536

    // Device regions (wired in later TODO items)
    static constexpr uint16_t printer_status_address = 0x37E8;
    static constexpr uint16_t keyboard_start = 0x3800;
    static constexpr uint16_t keyboard_end_exclusive = 0x3C00;
    static constexpr uint16_t vram_start = 0x3C00;
    static constexpr uint16_t vram_end_exclusive = 0x4000;

    static constexpr uint16_t port_decode_start = 0xE0;
    static constexpr uint16_t port_decode_end_exclusive = 0xF0;
    static constexpr uint16_t interrupt_latch_port_start = 0xE0;
    static constexpr uint16_t interrupt_latch_port_end_exclusive = 0xE4;
    static constexpr uint16_t hardware_control_port_start = 0xEC;
    static constexpr uint16_t hardware_control_port_end_exclusive = 0xF0;
    static constexpr uint16_t cassette_screen_port_start = 0xFC;
    static constexpr uint16_t cassette_screen_port_end_exclusive = 0x100;
    static constexpr uint16_t floppy_port_start = 0xF0;
    static constexpr uint16_t floppy_port_end_exclusive = 0xF5;

    static constexpr uint16_t load_rom_address = rom_start;
    static constexpr uint16_t load_rom_end_exclusive = rom_end_exclusive;
    static constexpr uint16_t load_ram_address = ram_start;
    static constexpr uint32_t load_ram_end_exclusive = ram_end_exclusive;
    static constexpr uint16_t default_omitted_load_address = rom_start;
};

class Machine;

std::unique_ptr<Machine> createTrs80M3Machine();
bool isTrs80M3PresetId(const std::string& id);

#endif
