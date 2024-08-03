#include <array>
#include <cstdint>
#include <vector>
#include "catch.hpp"
#include "devices/shared/framebuffer.hpp"
#include "devices/shared/memory.hpp"
#include "devices/zx81/character_rom.hpp"
#include "devices/zx81/display_file.hpp"
#include "devices/zx81/video_generator.hpp"
#include "machines/zx81/zx81_preset.hpp"
#include "machines/shared/machine.hpp"

namespace {

constexpr uint32_t kForeground = 0xFFFFFFFFu;
constexpr uint32_t kBackground = 0xFF000000u;

constexpr std::array<uint8_t, 8> kDigit1Glyph{0x00, 0x0C, 0x14, 0x04, 0x04, 0x04, 0x1E, 0x00};

struct UnifiedMemory {
    std::vector<uint8_t> rom_bytes = std::vector<uint8_t>(0x2000, 0x00);
    std::vector<uint8_t> ram_bytes = std::vector<uint8_t>(0x400, 0xFF);
};

Zx81DisplayFile::ReadFn readerFor(UnifiedMemory& mem) {
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

Zx81DisplayFile::WriteFn writerFor(UnifiedMemory& mem) {
    return [&mem](uint16_t address, uint8_t value) {
        if (address >= 0x4000 && address < 0x4400) {
            const uint16_t offset = static_cast<uint16_t>(address - 0x4000);
            mem.ram_bytes[offset] = value;
        }
    };
}

void writeGlyph(UnifiedMemory& mem, uint8_t code, const std::array<uint8_t, 8>& glyph) {
    const uint16_t offset =
        static_cast<uint16_t>(Zx81CharacterRom::glyphIndex(code) * Zx81CharacterRom::kGlyphRows);
    for (uint8_t row = 0; row < Zx81CharacterRom::kGlyphRows; ++row)
        mem.rom_bytes[Zx81CharacterRom::kCharsetBase + offset + row] = glyph[row];
}

void writeDigit1Display(UnifiedMemory& mem, uint16_t dfile) {
    writeGlyph(mem, 29, kDigit1Glyph);
    mem.ram_bytes[dfile - 0x4000] = Zx81DisplayFile::kNewline;
    mem.ram_bytes[dfile - 0x4000 + 1] = 29;
    mem.ram_bytes[dfile - 0x4000 + 2] = Zx81DisplayFile::kNewline;
    for (unsigned row = 1; row < 24; ++row)
        mem.ram_bytes[dfile - 0x4000 + 2 + row] = Zx81DisplayFile::kNewline;
    Zx81DisplayFile::writePointer(writerFor(mem), dfile);
    mem.ram_bytes[Zx81VideoGenerator::kCdflagAddress - 0x4000] = 0x00;
}

Memory* findReadOnlyRom(Machine& machine) {
    for (const auto& dev : machine.ownedDevices()) {
        if (auto* mem = dynamic_cast<Memory*>(dev.get())) {
            if (mem->isReadOnly())
                return mem;
        }
    }
    return nullptr;
}

void loadDigit1Glyph(Machine& machine) {
    Memory* rom = findReadOnlyRom(machine);
    REQUIRE(rom != nullptr);
    const uint16_t offset =
        static_cast<uint16_t>(Zx81CharacterRom::kCharsetBase + 29 * Zx81CharacterRom::kGlyphRows);
    rom->load(offset, kDigit1Glyph.data(), static_cast<uint16_t>(kDigit1Glyph.size()));
}

void writeDigit1Dfile(Machine& machine, uint16_t dfile) {
    machine.bus().write(dfile, Zx81DisplayFile::kNewline);
    machine.bus().write(static_cast<uint16_t>(dfile + 1), 29);
    machine.bus().write(static_cast<uint16_t>(dfile + 2), Zx81DisplayFile::kNewline);
    for (unsigned row = 1; row < 24; ++row)
        machine.bus().write(static_cast<uint16_t>(dfile + 2 + row), Zx81DisplayFile::kNewline);
    machine.bus().write(Zx81PresetContract::d_file_ptr_address, static_cast<uint8_t>(dfile & 0xFF));
    machine.bus().write(static_cast<uint16_t>(Zx81PresetContract::d_file_ptr_address + 1),
                        static_cast<uint8_t>(dfile >> 8));
}

} // namespace

TEST_CASE("ZX-81 video generator constants", "[device][zx81][fast]") {
    REQUIRE(Zx81VideoGenerator::kColumns == 32u);
    REQUIRE(Zx81VideoGenerator::kRows == 24u);
    REQUIRE(Zx81VideoGenerator::kCellSize == 8u);
    REQUIRE(Zx81VideoGenerator::kFramebufferWidth == Zx81PresetContract::video_framebuffer_width);
    REQUIRE(Zx81VideoGenerator::kFramebufferHeight == Zx81PresetContract::video_framebuffer_height);
    REQUIRE(Zx81VideoGenerator::kFrameHz == Zx81PresetContract::video_frame_hz);
    REQUIRE(Zx81VideoGenerator::kCdflagAddress == Zx81PresetContract::cdflag_address);
    REQUIRE(Zx81VideoGenerator::kCdflagSlowDisplayMask == Zx81PresetContract::cdflag_slow_display_mask);
    REQUIRE(Zx81PresetContract::guest_cpu_clock_hz / Zx81VideoGenerator::kFrameHz == 65'000u);
}

TEST_CASE("ZX-81 video generator renders collapsed empty screen black", "[device][zx81][fast]") {
    UnifiedMemory mem;
    const uint16_t dfile = 0x4200;
    Zx81DisplayFile::writeCollapsed(writerFor(mem), dfile);
    Zx81DisplayFile::writePointer(writerFor(mem), dfile);

    Framebuffer fb(Zx81VideoGenerator::kFramebufferWidth, Zx81VideoGenerator::kFramebufferHeight);
    Zx81VideoGenerator::renderToFramebuffer(fb, readerFor(mem));

    REQUIRE(fb.getPixel(0, 0) == kBackground);
    REQUIRE(fb.getPixel(255, 191) == kBackground);
    REQUIRE(fb.isDirty());
}

TEST_CASE("ZX-81 video generator renders digit 1 glyph pixels", "[device][zx81][fast]") {
    UnifiedMemory mem;
    writeDigit1Display(mem, 0x4200);

    Framebuffer fb(Zx81VideoGenerator::kFramebufferWidth, Zx81VideoGenerator::kFramebufferHeight);
    Zx81VideoGenerator::renderToFramebuffer(fb, readerFor(mem));

    REQUIRE(fb.getPixel(0, 0) == kBackground);
    REQUIRE(fb.getPixel(4, 1) == kForeground);
    REQUIRE(fb.getPixel(5, 1) == kForeground);
}

TEST_CASE("ZX-81 CDFLAG bit 7 selects SLOW display vs FAST blank", "[device][zx81][fast]") {
    UnifiedMemory mem;
    writeDigit1Display(mem, 0x4200);

    REQUIRE_FALSE(Zx81VideoGenerator::isDisplayActive(readerFor(mem)));

    mem.ram_bytes[Zx81VideoGenerator::kCdflagAddress - 0x4000] = Zx81VideoGenerator::kCdflagSlowDisplayMask;
    REQUIRE(Zx81VideoGenerator::isDisplayActive(readerFor(mem)));
}

TEST_CASE("ZX-81 FAST mode blanks framebuffer on frame tick", "[device][zx81][fast]") {
    auto machine = createZx81Machine();
    const uint16_t dfile = 0x4200;

    loadDigit1Glyph(*machine);
    writeDigit1Dfile(*machine, dfile);
    machine->bus().write(Zx81PresetContract::cdflag_address, 0x00); // FAST

    Framebuffer* framebuffer = nullptr;
    Zx81VideoGenerator* video = nullptr;
    for (const auto& dev : machine->ownedDevices()) {
        if (auto* fb = dynamic_cast<Framebuffer*>(dev.get()))
            framebuffer = fb;
        if (auto* gen = dynamic_cast<Zx81VideoGenerator*>(dev.get()))
            video = gen;
    }
    REQUIRE(framebuffer != nullptr);
    REQUIRE(video != nullptr);

    video->refreshFramebuffer();
    REQUIRE(framebuffer->getPixel(4, 1) == kBackground);
    REQUIRE(framebuffer->getPixel(5, 1) == kBackground);
}

TEST_CASE("ZX-81 SLOW mode keeps display on frame tick", "[device][zx81][fast]") {
    auto machine = createZx81Machine();
    const uint16_t dfile = 0x4200;

    loadDigit1Glyph(*machine);
    writeDigit1Dfile(*machine, dfile);
    machine->bus().write(Zx81PresetContract::cdflag_address, Zx81PresetContract::cdflag_slow_display_mask);

    Framebuffer* framebuffer = nullptr;
    Zx81VideoGenerator* video = nullptr;
    for (const auto& dev : machine->ownedDevices()) {
        if (auto* fb = dynamic_cast<Framebuffer*>(dev.get()))
            framebuffer = fb;
        if (auto* gen = dynamic_cast<Zx81VideoGenerator*>(dev.get()))
            video = gen;
    }
    REQUIRE(framebuffer != nullptr);
    REQUIRE(video != nullptr);

    video->refreshFramebuffer();
    REQUIRE(framebuffer->getPixel(4, 1) == kForeground);
    REQUIRE(framebuffer->getPixel(5, 1) == kForeground);
}

TEST_CASE("ZX-81 machine owns framebuffer and video generator", "[machine][zx81][fast]") {
    auto machine = createZx81Machine();

    Framebuffer* framebuffer = nullptr;
    Zx81VideoGenerator* video = nullptr;
    for (const auto& dev : machine->ownedDevices()) {
        if (auto* fb = dynamic_cast<Framebuffer*>(dev.get()))
            framebuffer = fb;
        if (auto* gen = dynamic_cast<Zx81VideoGenerator*>(dev.get()))
            video = gen;
    }

    REQUIRE(framebuffer != nullptr);
    REQUIRE(video != nullptr);
    REQUIRE(framebuffer->width() == Zx81VideoGenerator::kFramebufferWidth);
    REQUIRE(framebuffer->height() == Zx81VideoGenerator::kFramebufferHeight);
}

TEST_CASE("ZX-81 video generator ticks at 50 Hz", "[device][zx81][fast]") {
    auto machine = createZx81Machine();

    Zx81VideoGenerator* video = nullptr;
    for (const auto& dev : machine->ownedDevices()) {
        if (auto* gen = dynamic_cast<Zx81VideoGenerator*>(dev.get()))
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
