#ifndef MOJAVE_Z80_PRESET_HPP
#define MOJAVE_Z80_PRESET_HPP

#include <cstdint>
#include <memory>
#include <string>

struct Z80PresetContract {
    static constexpr uint32_t ram_bytes = 65536; // 64 KiB RAM
    static constexpr bool includes_virtual_tty = true;
    static constexpr bool needs_virtual_screen = false;
    static constexpr uint32_t guest_cpu_clock_hz = 4000000; // 4 MHz

    static constexpr uint16_t load_rom_address = 0x0000;
    static constexpr uint16_t load_rom_end_exclusive = 0x8000; // 32 KiB ROM limit
    static constexpr uint16_t load_ram_address = 0x8000;
    static constexpr uint16_t load_ram_end_exclusive = 0x0000; // 64 KiB wrap
    static constexpr uint16_t default_omitted_load_address = 0x0000;
};

class Machine;

// Forward declarations
std::unique_ptr<Machine> createZ80Machine();
bool isZ80PresetId(const std::string& id);

#endif
