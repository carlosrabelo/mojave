#include "catch.hpp"
#include "machines/shared/builtin_preset_registry.hpp"
#include "machines/shared/machine.hpp"
#include "devices/shared/framebuffer.hpp"

TEST_CASE("Builtin preset registry lists supported presets", "[machine][fast]") {
    REQUIRE(isBuiltinPresetId("z80"));
    REQUIRE(isBuiltinPresetId("m6502"));
    REQUIRE_FALSE(isBuiltinPresetId("m6507"));
    REQUIRE_FALSE(isBuiltinPresetId("missing"));
}

TEST_CASE("Builtin preset registry exposes descriptor metadata", "[machine][fast]") {
    const auto* z80 = findBuiltinPreset("z80");
    REQUIRE(z80 != nullptr);
    REQUIRE(z80->includes_virtual_tty);
    REQUIRE_FALSE(z80->needs_virtual_screen);
}

TEST_CASE("Builtin CPU presets create without a Framebuffer", "[machine][fast]") {
    for (const char* id : {"z80", "m6502"}) {
        const auto* preset = findBuiltinPreset(id);
        REQUIRE(preset != nullptr);
        REQUIRE_FALSE(preset->needs_virtual_screen);
        REQUIRE(preset->includes_virtual_tty);

        auto machine = createBuiltinMachine(id);
        REQUIRE(machine != nullptr);

        bool has_framebuffer = false;
        for (const auto& dev : machine->ownedDevices()) {
            if (dynamic_cast<Framebuffer*>(dev.get())) {
                has_framebuffer = true;
                break;
            }
        }
        REQUIRE_FALSE(has_framebuffer);
    }
}

TEST_CASE("Builtin preset registry creates machines", "[machine][fast]") {
    REQUIRE(createBuiltinMachine("z80") != nullptr);
    REQUIRE(createBuiltinMachine("m6502") != nullptr);
    REQUIRE(createBuiltinMachine("missing") == nullptr);
}

TEST_CASE("Builtin preset registry help lists registered ids", "[machine][fast]") {
    REQUIRE(formatBuiltinPresetIdsForHelp() == "z80, m6502");
}

TEST_CASE("Builtin preset registry resolves z80 load aliases", "[machine][fast]") {
    uint16_t addr = 0;
    REQUIRE(resolveBuiltinLoadAlias("z80", "rom", addr));
    REQUIRE(addr == 0x0000);
    REQUIRE(resolveBuiltinLoadAlias("z80", "ram", addr));
    REQUIRE(addr == 0x8000);
    REQUIRE(resolveBuiltinLoadAlias("m6502", "rom", addr));
    REQUIRE(addr == 0x0000);
    REQUIRE(resolveBuiltinLoadAlias("m6502", "ram", addr));
    REQUIRE(addr == 0x0000);
    REQUIRE_FALSE(resolveBuiltinLoadAlias("z80", "bank2", addr));
}
