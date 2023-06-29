#include "devices/shared/framebuffer.hpp"
#include <cstdlib>
#include <cstring>

Framebuffer::Framebuffer(uint16_t w, uint16_t h)
    : width_(w), height_(h), pixels_(new uint32_t[static_cast<size_t>(w) * h]) {
    fillSnow();
}

void Framebuffer::setPixel(uint16_t x, uint16_t y, uint32_t rgba) {
    if (x >= width_ || y >= height_) return;
    pixels_[static_cast<size_t>(y) * width_ + x] = rgba;
    expandDirty(x, y, 1, 1);
}

uint32_t Framebuffer::getPixel(uint16_t x, uint16_t y) const {
    if (x >= width_ || y >= height_) return 0;
    return pixels_[static_cast<size_t>(y) * width_ + x];
}

void Framebuffer::fill(uint32_t rgba) {
    for (size_t i = 0; i < static_cast<size_t>(width_) * height_; ++i)
        pixels_[i] = rgba;
    markDirty(0, 0, width_, height_);
}

void Framebuffer::fillSnow() {
    for (size_t i = 0; i < static_cast<size_t>(width_) * height_; ++i) {
        uint8_t g = static_cast<uint8_t>(std::rand() & 0xFF);
        pixels_[i] = 0xFF000000u | (g << 16) | (g << 8) | g;
    }
    markDirty(0, 0, width_, height_);
}

void Framebuffer::markDirty(uint16_t x, uint16_t y, uint16_t w, uint16_t h) {
    expandDirty(x, y, w, h);
}

void Framebuffer::expandDirty(uint16_t x, uint16_t y, uint16_t w, uint16_t h) {
    if (!dirty_) {
        dirty_x_ = x;
        dirty_y_ = y;
        dirty_w_ = w;
        dirty_h_ = h;
        dirty_ = true;
        return;
    }
    uint16_t nx = (x < dirty_x_) ? x : dirty_x_;
    uint16_t ny = (y < dirty_y_) ? y : dirty_y_;
    uint16_t ex1 = dirty_x_ + dirty_w_;
    uint16_t ex2 = x + w;
    uint16_t ey1 = dirty_y_ + dirty_h_;
    uint16_t ey2 = y + h;
    uint16_t nex = (ex1 > ex2) ? ex1 : ex2;
    uint16_t ney = (ey1 > ey2) ? ey1 : ey2;
    dirty_x_ = nx;
    dirty_y_ = ny;
    dirty_w_ = nex - nx;
    dirty_h_ = ney - ny;
}

void Framebuffer::reset() {
    fillSnow();
}

uint8_t Framebuffer::read(uint16_t address) {
    size_t idx = address;
    if (idx >= static_cast<size_t>(width_) * height_ * 4) return 0;
    auto* bytes = reinterpret_cast<const uint8_t*>(pixels_);
    return bytes[idx];
}

void Framebuffer::write(uint16_t address, uint8_t value) {
    size_t idx = address;
    if (idx >= static_cast<size_t>(width_) * height_ * 4) return;
    auto* bytes = reinterpret_cast<uint8_t*>(pixels_);
    bytes[idx] = value;

    size_t pixelIdx = idx / 4;
    expandDirty(static_cast<uint16_t>(pixelIdx % width_),
                static_cast<uint16_t>(pixelIdx / width_), 1, 1);
}
