#include <cstdint>
#include "catch.hpp"
#include "devices/shared/framebuffer.hpp"

TEST_CASE("Framebuffer has default dimensions 256x192", "[framebuffer][fast]") {
    Framebuffer fb;
    REQUIRE(fb.width() == 256);
    REQUIRE(fb.height() == 192);
}

TEST_CASE("Framebuffer custom dimensions", "[framebuffer][fast]") {
    Framebuffer fb(320, 240);
    REQUIRE(fb.width() == 320);
    REQUIRE(fb.height() == 240);
}

TEST_CASE("Framebuffer setPixel and getPixel", "[framebuffer][fast]") {
    Framebuffer fb(16, 16);
    fb.fill(0xFF000000);

    fb.setPixel(5, 3, 0xFFFFFFFF);
    REQUIRE(fb.getPixel(5, 3) == 0xFFFFFFFF);
    REQUIRE(fb.getPixel(0, 0) == 0xFF000000);
}

TEST_CASE("Framebuffer setPixel out of bounds is safe", "[framebuffer][fast]") {
    Framebuffer fb(16, 16);
    fb.setPixel(100, 100, 0xFFFFFFFF);
    REQUIRE(fb.getPixel(100, 100) == 0);
}

TEST_CASE("Framebuffer fill sets all pixels", "[framebuffer][fast]") {
    Framebuffer fb(8, 8);
    fb.fill(0xFF0000FF);
    for (uint16_t y = 0; y < 8; ++y)
        for (uint16_t x = 0; x < 8; ++x)
            REQUIRE(fb.getPixel(x, y) == 0xFF0000FF);
}

TEST_CASE("Framebuffer starts dirty after snow init", "[framebuffer][fast]") {
    Framebuffer fb(8, 8);
    REQUIRE(fb.isDirty());
    REQUIRE(fb.dirtyX() == 0);
    REQUIRE(fb.dirtyY() == 0);
    REQUIRE(fb.dirtyW() == 8);
    REQUIRE(fb.dirtyH() == 8);
}

TEST_CASE("Framebuffer clearDirty resets state", "[framebuffer][fast]") {
    Framebuffer fb(8, 8);
    REQUIRE(fb.isDirty());

    fb.clearDirty();
    REQUIRE_FALSE(fb.isDirty());
    REQUIRE(fb.dirtyW() == 0);
    REQUIRE(fb.dirtyH() == 0);
}

TEST_CASE("Framebuffer markDirty expands bounding box", "[framebuffer][fast]") {
    Framebuffer fb(32, 32);
    fb.clearDirty();

    fb.markDirty(10, 10, 1, 1);
    REQUIRE(fb.dirtyX() == 10);
    REQUIRE(fb.dirtyY() == 10);
    REQUIRE(fb.dirtyW() == 1);
    REQUIRE(fb.dirtyH() == 1);

    fb.markDirty(20, 5, 1, 1);
    REQUIRE(fb.dirtyX() == 10);
    REQUIRE(fb.dirtyY() == 5);
    REQUIRE(fb.dirtyW() == 11);
    REQUIRE(fb.dirtyH() == 6);
}

TEST_CASE("Framebuffer memory-mapped write updates pixel", "[framebuffer][fast]") {
    Framebuffer fb(4, 4);
    fb.fill(0xFF000000);

    // Write RGBA bytes for pixel (0,0) in little-endian memory order:
    // addr 0 = LSB, addr 3 = MSB
    // To get 0xFF0000FF: addr0=0xFF, addr1=0x00, addr2=0x00, addr3=0xFF
    fb.write(0, 0xFF);
    fb.write(1, 0x00);
    fb.write(2, 0x00);
    fb.write(3, 0xFF);

    REQUIRE(fb.getPixel(0, 0) == 0xFF0000FF);
    REQUIRE(fb.read(0) == 0xFF);
}

TEST_CASE("Framebuffer reset refills snow", "[framebuffer][fast]") {
    Framebuffer fb(8, 8);
    fb.fill(0xFF000000);
    fb.clearDirty();

    fb.reset();
    REQUIRE(fb.isDirty());
    REQUIRE(fb.dirtyW() == 8);
    REQUIRE(fb.dirtyH() == 8);
}

TEST_CASE("Framebuffer fillSnow produces varied noise", "[framebuffer][fast]") {
    Framebuffer fb(16, 16);
    // After snow, not all pixels should be identical
    uint32_t first = fb.getPixel(0, 0);
    bool varied = false;
    for (uint16_t i = 1; i < 16; ++i) {
        if (fb.getPixel(i, 0) != first) {
            varied = true;
            break;
        }
    }
    REQUIRE(varied);
}
