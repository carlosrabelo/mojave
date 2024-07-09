#include "devices/zx80/video_generator.hpp"
#include "devices/zx80/character_rom.hpp"

namespace {

constexpr uint32_t kForeground = 0xFFFFFFFFu;
constexpr uint32_t kBackground = 0xFF000000u;

void drawGlyphRow(Framebuffer& fb, uint16_t origin_x, uint16_t y, uint8_t row_bits) {
    for (uint8_t bit = 0; bit < Zx80CharacterRom::kGlyphRows; ++bit) {
        if ((row_bits & (1u << (7u - bit))) == 0)
            continue;
        fb.setPixel(static_cast<uint16_t>(origin_x + bit), y, kForeground);
    }
}

void drawCell(Framebuffer& fb, uint8_t column, uint8_t row, uint8_t code, Zx80DisplayFile::ReadFn read) {
    const uint16_t origin_x = static_cast<uint16_t>(column) * Zx80VideoGenerator::kCellSize;
    const uint16_t origin_y = static_cast<uint16_t>(row) * Zx80VideoGenerator::kCellSize;

    for (uint8_t glyph_row = 0; glyph_row < Zx80CharacterRom::kGlyphRows; ++glyph_row) {
        const uint8_t bits = Zx80CharacterRom::readRow(read, code, glyph_row);
        drawGlyphRow(fb, origin_x, static_cast<uint16_t>(origin_y + glyph_row), bits);
    }
}

} // namespace

Zx80VideoGenerator::Zx80VideoGenerator(Framebuffer& fb, Bus& bus, uint32_t guest_cpu_clock_hz)
    : fb_(fb), bus_(bus), cycles_per_frame_(guest_cpu_clock_hz / kFrameHz) {}

uint8_t Zx80VideoGenerator::read(uint16_t /*address*/) {
    return 0xFF;
}

void Zx80VideoGenerator::write(uint16_t /*address*/, uint8_t /*value*/) {}

void Zx80VideoGenerator::reset() {
    cycle_accumulator_ = 0;
    frames_rendered_ = 0;
    fb_.fill(kBackground);
}

void Zx80VideoGenerator::tick(unsigned cycles) {
    cycle_accumulator_ += cycles;
    while (cycle_accumulator_ >= cycles_per_frame_) {
        cycle_accumulator_ -= cycles_per_frame_;
        renderFrame();
    }
}

void Zx80VideoGenerator::refreshFramebuffer() {
    renderFrame();
}

void Zx80VideoGenerator::renderFrame() {
    const auto read = [this](uint16_t address) { return bus_.read(address); };
    renderToFramebuffer(fb_, read);
    ++frames_rendered_;
}

void Zx80VideoGenerator::renderToFramebuffer(Framebuffer& fb, Zx80DisplayFile::ReadFn read) {
    fb.fill(kBackground);
    if (!read)
        return;

    const uint16_t dfile = Zx80DisplayFile::readPointer(read);
    const auto grid = Zx80DisplayFile::parse(read, dfile);
    if (!grid.has_value())
        return;

    for (uint8_t row = 0; row < grid->row_count; ++row) {
        const Zx80DisplayFileLine& line = grid->rows[row];
        for (uint8_t column = 0; column < kColumns; ++column) {
            const uint8_t code = column < line.length ? line.chars[column] : 0;
            drawCell(fb, column, row, code, read);
        }
    }
}
