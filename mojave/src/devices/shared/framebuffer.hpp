#ifndef MOJAVE_FRAMEBUFFER_HPP
#define MOJAVE_FRAMEBUFFER_HPP

#include <cstdint>
#include "devices/device.hpp"

class Framebuffer : public Device {
public:
    Framebuffer(uint16_t w = 256, uint16_t h = 192);

    uint16_t width() const { return width_; }
    uint16_t height() const { return height_; }
    uint32_t* pixels() { return pixels_; }
    const uint32_t* pixels() const { return pixels_; }

    void setPixel(uint16_t x, uint16_t y, uint32_t rgba);
    uint32_t getPixel(uint16_t x, uint16_t y) const;
    void fill(uint32_t rgba);
    void fillSnow();

    bool isDirty() const { return dirty_; }
    void markDirty(uint16_t x, uint16_t y, uint16_t w, uint16_t h);
    void clearDirty() { dirty_ = false; dirty_w_ = 0; dirty_h_ = 0; }

    uint16_t dirtyX() const { return dirty_x_; }
    uint16_t dirtyY() const { return dirty_y_; }
    uint16_t dirtyW() const { return dirty_w_; }
    uint16_t dirtyH() const { return dirty_h_; }

    void reset() override;
    uint8_t read(uint16_t address) override;
    void write(uint16_t address, uint8_t value) override;

private:
    uint16_t width_;
    uint16_t height_;
    uint32_t* pixels_;

    bool dirty_ = false;
    uint16_t dirty_x_ = 0, dirty_y_ = 0, dirty_w_ = 0, dirty_h_ = 0;

    void expandDirty(uint16_t x, uint16_t y, uint16_t w, uint16_t h);
};

#endif
