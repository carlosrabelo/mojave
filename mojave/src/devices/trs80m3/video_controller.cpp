#include "devices/trs80m3/video_controller.hpp"
#include "devices/trs80m3/trs80m3_font.hpp"
#include <cstddef>

namespace {
constexpr uint32_t kForeground = 0xFFFFFFFFu;
constexpr uint32_t kBackground = 0xFF000000u;
} // namespace

Trs80M3VideoController::Trs80M3VideoController(Framebuffer& fb) : fb_(fb) {}

uint8_t Trs80M3VideoController::read(uint16_t address) {
    if (address >= kVramSize)
        return 0;
    return vram_[address];
}

void Trs80M3VideoController::write(uint16_t address, uint8_t value) {
    if (address >= kVramSize)
        return;
    vram_[address] = value;
    const uint8_t row = static_cast<uint8_t>(address / kColumns);
    const uint8_t column = static_cast<uint8_t>(address % kColumns);
    if (wide_ && column >= visibleColumns())
        return;
    renderCell(column, row);
}

void Trs80M3VideoController::reset() {
    wide_ = false;
    vram_.fill(0x20);
    fb_.fill(kBackground);
    renderAll();
}

void Trs80M3VideoController::setWideMode(bool wide) {
    if (wide_ == wide)
        return;
    wide_ = wide;
    fb_.fill(kBackground);
    renderAll();
}

void Trs80M3VideoController::refreshFramebuffer() {
    renderAll();
}

void Trs80M3VideoController::renderAll() {
    fb_.fill(kBackground);
    for (uint8_t row = 0; row < kRows; ++row) {
        for (uint8_t column = 0; column < visibleColumns(); ++column)
            renderCell(column, row);
    }
}

void Trs80M3VideoController::renderCell(uint8_t column, uint8_t row) {
    clearCell(column, row);
    const uint8_t ch = vram_[static_cast<size_t>(row) * kColumns + column];
    if (ch >= 128 && ch <= 191) {
        drawBlockGraphicsCell(column, row, ch);
        return;
    }
    drawTextCell(column, row, ch);
}

void Trs80M3VideoController::clearCell(uint8_t column, uint8_t row) {
    const uint16_t origin_x = static_cast<uint16_t>(column) * cellPixelWidth();
    const uint16_t origin_y = static_cast<uint16_t>(row) * kCellHeight;
    for (uint8_t y = 0; y < kCellHeight; ++y) {
        for (uint8_t x = 0; x < cellPixelWidth(); ++x)
            fb_.setPixel(origin_x + x, origin_y + y, kBackground);
    }
}

void Trs80M3VideoController::plotPixel(uint16_t x, uint16_t y) {
    fb_.setPixel(x, y, kForeground);
    if (wide_)
        fb_.setPixel(x + 1, y, kForeground);
}

void Trs80M3VideoController::drawTextCell(uint8_t column, uint8_t row, uint8_t ch) {
    const uint16_t origin_x = static_cast<uint16_t>(column) * cellPixelWidth();
    const uint16_t origin_y = static_cast<uint16_t>(row) * kCellHeight;
    for (uint8_t glyph_row = 0; glyph_row < trs80m3::kGlyphRows; ++glyph_row) {
        const uint8_t bits = trs80m3::textGlyphRow(ch, glyph_row);
        for (uint8_t bit = 0; bit < trs80m3::kGlyphWidth; ++bit) {
            if ((bits & (1u << (7u - bit))) == 0)
                continue;
            const uint16_t x = origin_x + static_cast<uint16_t>(wide_ ? bit * 2u : bit);
            plotPixel(x, origin_y + glyph_row);
        }
    }
}

void Trs80M3VideoController::drawBlockGraphicsCell(uint8_t column, uint8_t row, uint8_t ch) {
    const uint8_t pattern = static_cast<uint8_t>(ch - 128);
    const uint16_t origin_x = static_cast<uint16_t>(column) * cellPixelWidth();
    const uint16_t origin_y = static_cast<uint16_t>(row) * kCellHeight;
    const uint16_t block_width = wide_ ? 4u : 2u;

    for (uint8_t block = 0; block < 8; ++block) {
        if ((pattern & (1u << block)) == 0)
            continue;
        const uint8_t bx = static_cast<uint8_t>(block % 4);
        const uint8_t by = static_cast<uint8_t>(block / 4);
        const uint16_t px = origin_x + static_cast<uint16_t>(bx) * block_width;
        const uint16_t py = origin_y + static_cast<uint16_t>(by) * 6u;
        for (uint8_t y = 0; y < 6; ++y) {
            for (uint8_t x = 0; x < block_width; ++x)
                fb_.setPixel(px + x, py + y, kForeground);
        }
    }
}
