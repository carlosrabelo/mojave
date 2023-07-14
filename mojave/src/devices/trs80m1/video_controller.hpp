#ifndef MOJAVE_TRS80M1_VIDEO_CONTROLLER_HPP
#define MOJAVE_TRS80M1_VIDEO_CONTROLLER_HPP

#include <array>
#include <cstdint>
#include "devices/device.hpp"
#include "devices/shared/framebuffer.hpp"

class Trs80M1VideoController : public Device {
public:
    static constexpr uint16_t kColumns = 64;
    static constexpr uint16_t kRows = 16;
    static constexpr uint16_t kVramSize = kColumns * kRows;
    static constexpr uint16_t kCellWidth = 6;
    static constexpr uint16_t kCellHeight = 12;
    static constexpr uint16_t kFramebufferWidth = kCellWidth * kColumns;
    static constexpr uint16_t kFramebufferHeight = kCellHeight * kRows;

    explicit Trs80M1VideoController(Framebuffer& fb);

    uint8_t read(uint16_t address) override;
    void write(uint16_t address, uint8_t value) override;
    void reset() override;

    void setWideMode(bool wide);
    bool wideMode() const { return wide_; }

private:
    Framebuffer& fb_;
    std::array<uint8_t, kVramSize> vram_{};
    bool wide_ = false;

    uint8_t cellPixelWidth() const { return wide_ ? static_cast<uint8_t>(kCellWidth * 2) : kCellWidth; }
    uint8_t visibleColumns() const { return wide_ ? static_cast<uint8_t>(kColumns / 2) : kColumns; }

    void renderAll();
    void renderCell(uint8_t column, uint8_t row);
    void clearCell(uint8_t column, uint8_t row);
    void drawTextCell(uint8_t column, uint8_t row, uint8_t ch);
    void drawBlockGraphicsCell(uint8_t column, uint8_t row, uint8_t ch);
    void plotPixel(uint16_t x, uint16_t y);
};

#endif
