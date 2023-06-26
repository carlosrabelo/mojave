#ifndef MOJAVE_BUILTIN_PRESET_REGISTRY_HPP
#define MOJAVE_BUILTIN_PRESET_REGISTRY_HPP

#include <cstdint>
#include <memory>
#include <string>

class Machine;

enum class BuiltinCpuFamily {
    Z80,
    M6502,
    M6507,
};

struct GuestLoadRegions {
    uint32_t rom_address;
    uint32_t rom_end_exclusive;
    uint32_t ram_address;
    uint32_t ram_end_exclusive;
    uint16_t default_omitted_load_address;
};

struct BuiltinPresetDescriptor {
    const char* id;
    BuiltinCpuFamily cpu_family;
    bool is_basic;
    bool includes_virtual_tty;
    bool needs_virtual_screen;
    uint32_t guest_cpu_clock_hz;
    GuestLoadRegions load_regions;
};

bool isBuiltinPresetId(const std::string& id);
const BuiltinPresetDescriptor* findBuiltinPreset(const std::string& id);
std::unique_ptr<Machine> createBuiltinMachine(const std::string& id);
bool resolveBuiltinLoadAlias(const std::string& id, const char* alias, uint16_t& out);
std::string formatBuiltinPresetIdsForHelp();

#endif
