#include "catch.hpp"
#include "machines/shared/builtin_preset_registry.hpp"
#include "machines/shared/machine.hpp"

TEST_CASE("Builtin preset registry is empty before presets land", "[machine][fast]") {
    REQUIRE_FALSE(isBuiltinPresetId("z80"));
    REQUIRE_FALSE(isBuiltinPresetId("missing"));
    REQUIRE(findBuiltinPreset("z80") == nullptr);
    REQUIRE(createBuiltinMachine("z80") == nullptr);
    uint16_t addr = 0;
    REQUIRE_FALSE(resolveBuiltinLoadAlias("z80", "rom", addr));
    REQUIRE(formatBuiltinPresetIdsForHelp().empty());
}
