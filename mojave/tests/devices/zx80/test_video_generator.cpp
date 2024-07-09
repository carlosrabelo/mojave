#include <array>
#include <cstdint>
#include <vector>
#include "catch.hpp"
#include "devices/shared/framebuffer.hpp"
#include "devices/zx80/character_rom.hpp"
#include "devices/zx80/display_file.hpp"
#include "devices/zx80/video_generator.hpp"
#include "machines/zx80/zx80_preset.hpp"
#include "machines/shared/machine.hpp"

namespace {

constexpr uint32_t kForeground = 0xFFFFFFFFu;
constexpr uint32_t kBackground = 0xFF000000u;

constexpr std::array<uint8_t, 8> kDigit1Glyph{0x00, 0x0C, 0x14, 0x04, 0x04, 0x04, 0x1E, 0x00};

struct UnifiedMemory {
    std::vector<uint8_t> rom_bytes = std::vector<uint8_t>(0x1000, 0x00);
    std::vector<uint8_t> ram_bytes = std::vector<uint8_t>(0x400, 0xFF);
};

Zx80DisplayFile::ReadFn readerFor(UnifiedMemory& mem) {
    return [&mem](uint16_t address) -> uint8_t {
        if (address < mem.rom_bytes.size())
            return mem.rom_bytes[address];
        if (address >= 0x4000 && address < 0x4400) {
            const uint16_t offset = static_cast<uint16_t>(address - 0x4000);
            return mem.ram_bytes[offset];
        }
        return 0xFF;
    };
}

Zx80DisplayFile::WriteFn writerFor(UnifiedMemory& mem) {
    return [&mem](uint16_t address, uint8_t value) {
        if (address >= 0x4000 && address < 0x4400) {
            const uint16_t offset = static_cast<uint16_t>(address - 0x4000);
            mem.ram_bytes[offset] = value;
        }
    };
}

void writeGlyph(UnifiedMemory& mem, uint8_t code, const std::array<uint8_t, 8>& glyph) {
    const uint16_t offset =
        static_cast<uint16_t>(Zx80CharacterRom::glyphIndex(code) * Zx80CharacterRom::kGlyphRows);
    for (uint8_t row = 0; row < Zx80CharacterRom::kGlyphRows; ++row)
        mem.rom_bytes[Zx80CharacterRom::kCharsetBase + offset + row] = glyph[row];
}

} // namespace

TEST_CASE("ZX-80 video generator constants", "[device][zx80][fast]") {
    REQUIRE(Zx80VideoGenerator::kColumns == 32u);
    REQUIRE(Zx80VideoGenerator::kRows == 24u);
    REQUIRE(Zx80VideoGenerator::kCellSize == 8u);
    REQUIRE(Zx80VideoGenerator::kFramebufferWidth == Zx80PresetContract::video_framebuffer_width);
    REQUIRE(Zx80VideoGenerator::kFramebufferHeight == Zx80PresetContract::video_framebuffer_height);
    REQUIRE(Zx80VideoGenerator::kFrameHz == Zx80PresetContract::video_frame_hz);
    REQUIRE(Zx80PresetContract::guest_cpu_clock_hz / Zx80VideoGenerator::kFrameHz == 65'000u);
}

TEST_CASE("ZX-80 video generator renders collapsed empty screen black", "[device][zx80][fast]") {
    UnifiedMemory mem;
    const uint16_t dfile = 0x4200;
    Zx80DisplayFile::writeCollapsed(writerFor(mem), dfile);
    Zx80DisplayFile::writePointer(writerFor(mem), dfile);

    Framebuffer fb(Zx80VideoGenerator::kFramebufferWidth, Zx80VideoGenerator::kFramebufferHeight);
    Zx80VideoGenerator::renderToFramebuffer(fb, readerFor(mem));

    REQUIRE(fb.getPixel(0, 0) == kBackground);
    REQUIRE(fb.getPixel(255, 191) == kBackground);
    REQUIRE(fb.isDirty());
}

TEST_CASE("ZX-80 video generator renders digit 1 glyph pixels", "[device][zx80][fast]") {
    UnifiedMemory mem;
    writeGlyph(mem, 29, kDigit1Glyph);

    const uint16_t dfile = 0x4200;
    mem.ram_bytes[dfile - 0x4000] = Zx80DisplayFile::kHaltOpcode;
    mem.ram_bytes[dfile - 0x4000 + 1] = 29;
    mem.ram_bytes[dfile - 0x4000 + 2] = Zx80DisplayFile::kHaltOpcode;
    for (unsigned row = 1; row < 24; ++row)
        mem.ram_bytes[dfile - 0x4000 + 2 + row] = Zx80DisplayFile::kHaltOpcode;
    Zx80DisplayFile::writePointer(writerFor(mem), dfile);

    Framebuffer fb(Zx80VideoGenerator::kFramebufferWidth, Zx80VideoGenerator::kFramebufferHeight);
    Zx80VideoGenerator::renderToFramebuffer(fb, readerFor(mem));

    REQUIRE(fb.getPixel(0, 0) == kBackground);
    REQUIRE(fb.getPixel(4, 1) == kForeground);
    REQUIRE(fb.getPixel(5, 1) == kForeground);
}

TEST_CASE("ZX-80 machine owns framebuffer and video generator", "[machine][zx80][fast]") {
    auto machine = createZx80Machine();

    Framebuffer* framebuffer = nullptr;
    Zx80VideoGenerator* video = nullptr;
    for (const auto& dev : machine->ownedDevices()) {
        if (auto* fb = dynamic_cast<Framebuffer*>(dev.get()))
            framebuffer = fb;
        if (auto* gen = dynamic_cast<Zx80VideoGenerator*>(dev.get()))
            video = gen;
    }

    REQUIRE(framebuffer != nullptr);
    REQUIRE(video != nullptr);
    REQUIRE(framebuffer->width() == Zx80VideoGenerator::kFramebufferWidth);
    REQUIRE(framebuffer->height() == Zx80VideoGenerator::kFramebufferHeight);
}

TEST_CASE("ZX-80 video generator ticks at 50 Hz", "[device][zx80][fast]") {
    auto machine = createZx80Machine();

    Zx80VideoGenerator* video = nullptr;
    for (const auto& dev : machine->ownedDevices()) {
        if (auto* gen = dynamic_cast<Zx80VideoGenerator*>(dev.get()))
            video = gen;
    }
    REQUIRE(video != nullptr);

    machine->reset();
    REQUIRE(video->framesRendered() == 0);

    video->tick(64'999);
    REQUIRE(video->framesRendered() == 0);

    video->tick(1);
    REQUIRE(video->framesRendered() == 1);
}
