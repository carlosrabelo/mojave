#include "machines/shared/builtin_preset_registry.hpp"
#include "machines/shared/machine.hpp"
#include "machines/z80/z80_preset.hpp"
#include <cstring>

namespace {

template<typename Contract>
constexpr GuestLoadRegions loadRegionsFromContract() {
    return GuestLoadRegions{
        Contract::load_rom_address,
        Contract::load_rom_end_exclusive,
        Contract::load_ram_address,
        Contract::load_ram_end_exclusive,
        Contract::default_omitted_load_address,
    };
}

using CreateFn = std::unique_ptr<Machine> (*)();

struct BuiltinPresetEntry {
    BuiltinPresetDescriptor descriptor;
    CreateFn create;
};

const BuiltinPresetEntry kPresets[] = {
    {
        {
            "z80",
            BuiltinCpuFamily::Z80,
            true,
            Z80PresetContract::includes_virtual_tty,
            Z80PresetContract::needs_virtual_screen,
            Z80PresetContract::guest_cpu_clock_hz,
            loadRegionsFromContract<Z80PresetContract>(),
        },
        createZ80Machine,
    },
};

constexpr std::size_t kPresetCount = sizeof(kPresets) / sizeof(kPresets[0]);

const BuiltinPresetEntry* findEntry(const std::string& id) {
    for (std::size_t i = 0; i < kPresetCount; ++i) {
        if (id == kPresets[i].descriptor.id)
            return &kPresets[i];
    }
    return nullptr;
}

} // namespace

bool isBuiltinPresetId(const std::string& id) {
    return findEntry(id) != nullptr;
}

const BuiltinPresetDescriptor* findBuiltinPreset(const std::string& id) {
    const BuiltinPresetEntry* entry = findEntry(id);
    return entry ? &entry->descriptor : nullptr;
}

std::unique_ptr<Machine> createBuiltinMachine(const std::string& id) {
    const BuiltinPresetEntry* entry = findEntry(id);
    if (!entry)
        return nullptr;
    return entry->create();
}

bool resolveBuiltinLoadAlias(const std::string& id, const char* alias, uint16_t& out) {
    const BuiltinPresetDescriptor* preset = findBuiltinPreset(id);
    if (!preset)
        return false;

    if (std::strcmp(alias, "rom") == 0) {
        out = static_cast<uint16_t>(preset->load_regions.rom_address);
        return true;
    }
    if (std::strcmp(alias, "ram") == 0) {
        out = static_cast<uint16_t>(preset->load_regions.ram_address);
        return true;
    }
    return false;
}

std::string formatBuiltinPresetIdsForHelp() {
    std::string list;
    for (std::size_t i = 0; i < kPresetCount; ++i) {
        if (i > 0)
            list += ", ";
        list += kPresets[i].descriptor.id;
    }
    return list;
}
