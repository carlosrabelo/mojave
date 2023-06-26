#include "machines/shared/builtin_preset_registry.hpp"
#include "machines/shared/machine.hpp"
#include <cstring>

bool isBuiltinPresetId(const std::string&) {
    return false;
}

const BuiltinPresetDescriptor* findBuiltinPreset(const std::string&) {
    return nullptr;
}

std::unique_ptr<Machine> createBuiltinMachine(const std::string&) {
    return nullptr;
}

bool resolveBuiltinLoadAlias(const std::string&, const char*, uint16_t&) {
    return false;
}

std::string formatBuiltinPresetIdsForHelp() {
    return {};
}
