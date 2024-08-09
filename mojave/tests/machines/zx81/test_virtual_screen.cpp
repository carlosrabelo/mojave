#include <array>
#include <cstdint>
#include "catch.hpp"
#include "machines/zx81/zx81_preset.hpp"
#include "machines/shared/builtin_preset_registry.hpp"
#include "machines/shared/machine.hpp"
#include "devices/shared/framebuffer.hpp"
#include "devices/shared/screen_registry.hpp"
#include "devices/zx81/character_rom.hpp"
#include "devices/zx81/display_file.hpp"
#include "devices/zx81/video_generator.hpp"
#include "devices/shared/memory.hpp"
#include "bus/bus.hpp"
#include "session/runner.hpp"
#include "session/clock.hpp"
#include "cpus/z80.hpp"

namespace {

constexpr uint32_t kForeground = 0xFFFFFFFFu;
constexpr uint32_t kBackground = 0xFF000000u;

constexpr std::array<uint8_t, 8> kDigit1Glyph{0x00, 0x0C, 0x14, 0x04, 0x04, 0x04, 0x1E, 0x00};

Framebuffer* findMachineFramebuffer(Machine& machine) {
    for (const auto& dev : machine.ownedDevices()) {
        if (auto* fb = dynamic_cast<Framebuffer*>(dev.get()))
            return fb;
    }
    return nullptr;
}

Zx81VideoGenerator* findMachineVideo(Machine& machine) {
    for (const auto& dev : machine.ownedDevices()) {
        if (auto* video = dynamic_cast<Zx81VideoGenerator*>(dev.get()))
            return video;
    }
    return nullptr;
}

void loadGlyphToRom(Machine& machine, uint8_t code, const std::array<uint8_t, 8>& glyph) {
    const Mapping* mapping = machine.bus().find(Zx81CharacterRom::kCharsetBase);
    REQUIRE(mapping != nullptr);
    auto* mem = dynamic_cast<Memory*>(mapping->device);
    REQUIRE(mem != nullptr);

    const uint16_t offset = static_cast<uint16_t>(
        Zx81CharacterRom::kCharsetBase - Zx81PresetContract::rom_start +
        Zx81CharacterRom::glyphIndex(code) * Zx81CharacterRom::kGlyphRows);
    mem->load(offset, glyph.data(), Zx81CharacterRom::kGlyphRows);
}

void writeSingleLineDfile(Bus& bus, uint16_t dfile, uint8_t code) {
    bus.write(dfile, Zx81DisplayFile::kNewline);
    bus.write(static_cast<uint16_t>(dfile + 1), code);
    bus.write(static_cast<uint16_t>(dfile + 2), Zx81DisplayFile::kNewline);
    for (unsigned row = 1; row < Zx81DisplayFile::kRows; ++row)
        bus.write(static_cast<uint16_t>(dfile + 2 + row), Zx81DisplayFile::kNewline);

    bus.write(Zx81DisplayFile::kDFilePointerAddress, static_cast<uint8_t>(dfile & 0xFF));
    bus.write(static_cast<uint16_t>(Zx81DisplayFile::kDFilePointerAddress + 1),
              static_cast<uint8_t>((dfile >> 8) & 0xFF));
}

} // namespace

TEST_CASE("Sinclair ZX-81 preset is eligible for SDL and Qt6 frontends",
          "[machine][zx81][integration]") {
    const BuiltinPresetDescriptor* preset = findBuiltinPreset("zx81");
    REQUIRE(preset != nullptr);
    REQUIRE(preset->needs_virtual_screen);

    auto machine = createBuiltinMachine("zx81");
    REQUIRE(machine != nullptr);

    Framebuffer* fb = findMachineFramebuffer(*machine);
    REQUIRE(fb != nullptr);
    REQUIRE(fb->width() == Zx81VideoGenerator::kFramebufferWidth);
    REQUIRE(fb->height() == Zx81VideoGenerator::kFramebufferHeight);
}

TEST_CASE("Sinclair ZX-81 virtual screen shows DFILE through the screen registry",
          "[machine][zx81][integration]") {
    auto machine = createZx81Machine();
    Framebuffer* fb = findMachineFramebuffer(*machine);
    Zx81VideoGenerator* video = findMachineVideo(*machine);
    REQUIRE(fb != nullptr);
    REQUIRE(video != nullptr);

    ScreenRegistry registry;
    registry.registerScreen("main", *fb);

    runner::boot(*machine);
    video->refreshFramebuffer();
    REQUIRE(fb->getPixel(0, 0) == kBackground);

    const uint16_t dfile = 0x4200;
    loadGlyphToRom(*machine, 29, kDigit1Glyph);
    writeSingleLineDfile(machine->bus(), dfile, 29);
    machine->bus().write(Zx81PresetContract::cdflag_address, Zx81PresetContract::cdflag_slow_display_mask);
    video->refreshFramebuffer();

    Framebuffer* screen = registry.screen("main");
    REQUIRE(screen == fb);
    REQUIRE(screen->getPixel(4, 1) == kForeground);
    REQUIRE(screen->getPixel(5, 1) == kForeground);
}

TEST_CASE("Sinclair ZX-81 frontend frame loop uses guest clock pacing",
          "[machine][zx81][integration]") {
    auto machine = createZx81Machine();
    const BuiltinPresetDescriptor* preset = findBuiltinPreset("zx81");
    REQUIRE(preset != nullptr);

    Clock clock(preset->guest_cpu_clock_hz);
    runner::boot(*machine);

    machine->bus().write(Zx81PresetContract::ram_start, 0x18);
    machine->bus().write(static_cast<uint16_t>(Zx81PresetContract::ram_start + 1), 0xFE);
    dynamic_cast<Z80&>(machine->cpu()).regs().pc = Zx81PresetContract::ram_start;

    const unsigned budget = static_cast<unsigned>(clock.cyclesPerFrame(Zx81PresetContract::video_frame_hz));
    const unsigned cycles = runner::stepForFrame(*machine, clock, Zx81PresetContract::video_frame_hz);
    REQUIRE(cycles >= budget);
    REQUIRE(cycles - budget <= 12u);
}
