#ifndef MOJAVE_ZX80_PRESET_HPP
#define MOJAVE_ZX80_PRESET_HPP

#include <cstdint>
#include <memory>
#include <string>

struct Zx80PresetContract {
    static constexpr uint32_t ram_bytes = 1024; // 1 KiB built-in RAM
    static constexpr bool includes_virtual_tty = false;
    static constexpr bool needs_virtual_screen = true;
    static constexpr uint32_t guest_cpu_clock_hz = 3'250'000; // Z80 @ 3.25 MHz

    // ROM — 4 KiB Integer BASIC
    static constexpr uint16_t rom_start = 0x0000;
    static constexpr uint16_t rom_end_exclusive = 0x1000;

    // RAM — 1 KiB workspace at 0x4000
    static constexpr uint16_t ram_start = 0x4000;
    static constexpr uint16_t ram_end_exclusive = 0x4400;

    // System variables (wired in later TODO items)
    static constexpr uint16_t sysvar_start = 0x4000;
    static constexpr uint16_t err_nr_address = 0x4000;
    static constexpr uint16_t flags_address = 0x4001;
    static constexpr uint8_t flags_k_mode_bit = 2;
    static constexpr uint8_t flags_k_mode_mask = 1u << flags_k_mode_bit;
    static constexpr uint8_t flags_k_cursor_bit = 3;
    static constexpr uint8_t flags_k_cursor_mask = 1u << flags_k_cursor_bit;
    static constexpr uint16_t flagx_address = 0x4019;
    static constexpr uint8_t flagx_k_mode_bit = 7;
    static constexpr uint8_t flagx_k_mode_mask = 1u << flagx_k_mode_bit;
    static constexpr uint16_t d_file_ptr_address = 0x400C;

    // Display file layout (see devices/zx80/display_file.hpp)
    static constexpr uint8_t dfile_halt_opcode = 0x76;
    static constexpr unsigned dfile_columns = 32;
    static constexpr unsigned dfile_rows = 24;
    static constexpr unsigned dfile_min_collapsed_bytes = 25;
    static constexpr unsigned dfile_max_bytes = 793;

    // ROM character bitmaps at 0x0E00 (I register = 0x0E on hardware)
    static constexpr uint16_t charset_rom_start = 0x0E00;
    static constexpr uint16_t charset_rom_end_exclusive = 0x1000;
    static constexpr uint8_t charset_i_register = 0x0E;

    // Video output (see devices/zx80/video_generator.hpp)
    static constexpr uint32_t video_frame_hz = 50;
    static constexpr uint32_t video_cycles_per_frame = guest_cpu_clock_hz / video_frame_hz;
    static constexpr uint16_t video_framebuffer_width = 256;
    static constexpr uint16_t video_framebuffer_height = 192;

    // Keyboard matrix (see devices/sinclair/keyboard.hpp)
    static constexpr uint16_t keyboard_port_low_byte = 0x00FE;
    static constexpr uint8_t keyboard_row_count = 8;
    static constexpr uint8_t keyboard_bits_per_row = 5;
    static constexpr uint16_t io_port_attach_start = 0x0000;
    static constexpr uint16_t io_port_attach_end_exclusive = 0x0000; // 0 = full 64K range

    // Cassette EAR/MIC stub (see devices/sinclair/cassette.hpp)
    static constexpr uint32_t cassette_baud = 250;
    static constexpr uint8_t cassette_ear_bit = 0x80;

    // Video RAM mirror trap region (wired in later TODO items)
    static constexpr uint16_t vram_mirror_start = 0xC000;
    static constexpr uint32_t vram_mirror_end_exclusive = 65536u;

    static constexpr uint16_t load_rom_address = rom_start;
    static constexpr uint16_t load_rom_end_exclusive = rom_end_exclusive;
    static constexpr uint16_t load_ram_address = ram_start;
    static constexpr uint32_t load_ram_end_exclusive = ram_end_exclusive;
    static constexpr uint16_t default_omitted_load_address = rom_start;
};

class Machine;

std::unique_ptr<Machine> createZx80Machine();
bool isZx80PresetId(const std::string& id);

#endif
