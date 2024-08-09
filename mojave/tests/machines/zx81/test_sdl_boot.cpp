#include <array>
#include <cstdint>
#include "catch.hpp"
#include "machines/zx81/zx81_preset.hpp"
#include "machines/shared/machine.hpp"
#include "devices/shared/framebuffer.hpp"
#include "devices/shared/memory.hpp"
#include "devices/zx81/character_rom.hpp"
#include "devices/zx81/display_file.hpp"
#include "devices/zx81/video_generator.hpp"
#include "session/runner.hpp"

namespace {

constexpr uint32_t kForeground = 0xFFFFFFFFu;
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

unsigned countWhitePixels(const Framebuffer& fb) {
    unsigned white = 0;
    for (uint16_t y = 0; y < fb.height(); ++y) {
        for (uint16_t x = 0; x < fb.width(); ++x) {
            if (fb.getPixel(x, y) == kForeground)
                ++white;
        }
    }
    return white;
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

TEST_CASE("Sinclair ZX-81 video refresh re-rasterizes after a framebuffer wipe",
          "[machine][zx81][fast]") {
    auto machine = createZx81Machine();
    Framebuffer* fb = findMachineFramebuffer(*machine);
    Zx81VideoGenerator* video = findMachineVideo(*machine);
    REQUIRE(fb != nullptr);
    REQUIRE(video != nullptr);

    runner::boot(*machine);
    loadGlyphToRom(*machine, 29, kDigit1Glyph);
    writeSingleLineDfile(machine->bus(), 0x4200, 29);
    machine->bus().write(Zx81PresetContract::cdflag_address, Zx81PresetContract::cdflag_slow_display_mask);

    fb->fill(0xFF000000u);
    REQUIRE(countWhitePixels(*fb) == 0);

    video->refreshFramebuffer();
    REQUIRE(countWhitePixels(*fb) > 0);
    REQUIRE(fb->getPixel(4, 1) == kForeground);
    REQUIRE(fb->getPixel(5, 1) == kForeground);
}
