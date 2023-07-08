#ifndef MOJAVE_M6507_PRESET_HPP
#define MOJAVE_M6507_PRESET_HPP

#include <cstdint>
#include <memory>
#include <string>

struct M6507PresetContract {
    static constexpr uint32_t ram_bytes = 8192;
    static constexpr bool includes_virtual_tty = true;
    static constexpr bool needs_virtual_screen = false;
    static constexpr uint32_t guest_cpu_clock_hz = 1190000; // 1.19 MHz

    static constexpr uint16_t load_rom_address = 0x1000;
    static constexpr uint16_t load_rom_end_exclusive = 0x2000;
    static constexpr uint16_t load_ram_address = 0x0000;
    static constexpr uint16_t load_ram_end_exclusive = 0x1E00;
    static constexpr uint16_t default_omitted_load_address = 0x0000;

    static constexpr uint16_t tty_address = 0x1E00;
};

class Machine;

// Forward declarations
std::unique_ptr<Machine> createM6507Machine();
bool isM6507PresetId(const std::string& id);

#endif
