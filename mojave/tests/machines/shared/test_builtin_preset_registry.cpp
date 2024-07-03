#include "catch.hpp"
#include "machines/shared/builtin_preset_registry.hpp"
#include "machines/shared/machine.hpp"
#include "devices/shared/framebuffer.hpp"

TEST_CASE("Builtin preset registry lists supported presets", "[machine][fast]") {
    REQUIRE(isBuiltinPresetId("z80"));
    REQUIRE(isBuiltinPresetId("m6502"));
    REQUIRE(isBuiltinPresetId("m6507"));
    REQUIRE(isBuiltinPresetId("trs80m1l1"));
    REQUIRE(isBuiltinPresetId("trs80m1l2"));
    REQUIRE(isBuiltinPresetId("trs80m3"));
    REQUIRE(isBuiltinPresetId("zx80"));
    REQUIRE_FALSE(isBuiltinPresetId("missing"));
}

TEST_CASE("Builtin preset registry exposes descriptor metadata", "[machine][fast]") {
    const auto* z80 = findBuiltinPreset("z80");
    REQUIRE(z80 != nullptr);
    REQUIRE(z80->includes_virtual_tty);
    REQUIRE_FALSE(z80->needs_virtual_screen);

    const auto* trs80m1l1 = findBuiltinPreset("trs80m1l1");
    REQUIRE(trs80m1l1 != nullptr);
    REQUIRE_FALSE(trs80m1l1->includes_virtual_tty);
    REQUIRE(trs80m1l1->needs_virtual_screen);
    REQUIRE(trs80m1l1->guest_cpu_clock_hz == 1'774'080u);

    const auto* trs80m1l2 = findBuiltinPreset("trs80m1l2");
    REQUIRE(trs80m1l2 != nullptr);
    REQUIRE_FALSE(trs80m1l2->includes_virtual_tty);
    REQUIRE(trs80m1l2->needs_virtual_screen);
    REQUIRE(trs80m1l2->guest_cpu_clock_hz == 1'774'080u);
    REQUIRE(trs80m1l2->load_regions.rom_end_exclusive == 0x3000);
    REQUIRE(trs80m1l2->load_regions.ram_end_exclusive == 65536u);

    const auto* trs80m3 = findBuiltinPreset("trs80m3");
    REQUIRE(trs80m3 != nullptr);
    REQUIRE_FALSE(trs80m3->includes_virtual_tty);
    REQUIRE(trs80m3->needs_virtual_screen);
    REQUIRE(trs80m3->guest_cpu_clock_hz == 2'027'520u);
    REQUIRE(trs80m3->load_regions.rom_end_exclusive == 0x3800);
    REQUIRE(trs80m3->load_regions.ram_end_exclusive == 65536u);

    const auto* zx80 = findBuiltinPreset("zx80");
    REQUIRE(zx80 != nullptr);
    REQUIRE_FALSE(zx80->includes_virtual_tty);
    REQUIRE(zx80->needs_virtual_screen);
    REQUIRE(zx80->guest_cpu_clock_hz == 3'250'000u);
    REQUIRE(zx80->load_regions.rom_end_exclusive == 0x1000);
    REQUIRE(zx80->load_regions.ram_end_exclusive == 0x4400);
}

TEST_CASE("Builtin CPU presets create without a Framebuffer", "[machine][fast]") {
    for (const char* id : {"z80", "m6502", "m6507"}) {
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
    REQUIRE(createBuiltinMachine("m6507") != nullptr);
    auto trs80m1l1 = createBuiltinMachine("trs80m1l1");
    REQUIRE(trs80m1l1 != nullptr);
    bool has_framebuffer = false;
    for (const auto& dev : trs80m1l1->ownedDevices()) {
        if (dynamic_cast<Framebuffer*>(dev.get())) {
            has_framebuffer = true;
            break;
        }
    }
    REQUIRE(has_framebuffer);
    REQUIRE(createBuiltinMachine("trs80m1l2") != nullptr);
    auto trs80m3 = createBuiltinMachine("trs80m3");
    REQUIRE(trs80m3 != nullptr);
    has_framebuffer = false;
    for (const auto& dev : trs80m3->ownedDevices()) {
        if (dynamic_cast<Framebuffer*>(dev.get())) {
            has_framebuffer = true;
            break;
        }
    }
    REQUIRE(has_framebuffer);
    REQUIRE(createBuiltinMachine("zx80") != nullptr);
    REQUIRE(createBuiltinMachine("missing") == nullptr);
}

TEST_CASE("Builtin preset registry help lists registered ids", "[machine][fast]") {
    REQUIRE(formatBuiltinPresetIdsForHelp() ==
            "z80, m6502, m6507, trs80m1l1, trs80m1l2, trs80m3, zx80");
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
    REQUIRE(resolveBuiltinLoadAlias("m6507", "rom", addr));
    REQUIRE(addr == 0x1000);
    REQUIRE(resolveBuiltinLoadAlias("m6507", "ram", addr));
    REQUIRE(addr == 0x0000);
    REQUIRE(resolveBuiltinLoadAlias("trs80m1l1", "rom", addr));
    REQUIRE(addr == 0x0000);
    REQUIRE(resolveBuiltinLoadAlias("trs80m1l1", "ram", addr));
    REQUIRE(addr == 0x4000);
    REQUIRE(resolveBuiltinLoadAlias("trs80m1l2", "rom", addr));
    REQUIRE(addr == 0x0000);
    REQUIRE(resolveBuiltinLoadAlias("trs80m1l2", "ram", addr));
    REQUIRE(addr == 0x4000);
    REQUIRE(resolveBuiltinLoadAlias("trs80m3", "rom", addr));
    REQUIRE(addr == 0x0000);
    REQUIRE(resolveBuiltinLoadAlias("trs80m3", "ram", addr));
    REQUIRE(addr == 0x4000);
    REQUIRE(resolveBuiltinLoadAlias("zx80", "rom", addr));
    REQUIRE(addr == 0x0000);
    REQUIRE(resolveBuiltinLoadAlias("zx80", "ram", addr));
    REQUIRE(addr == 0x4000);
    REQUIRE_FALSE(resolveBuiltinLoadAlias("z80", "bank2", addr));
}
