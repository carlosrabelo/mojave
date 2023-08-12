#include "devices/trs80m3/video_controller.hpp"
#include "devices/shared/framebuffer.hpp"
#include "catch.hpp"

TEST_CASE("TRS-80 Model III video controller dimensions", "[devices][trs80m3][fast]") {
    REQUIRE(Trs80M3VideoController::kColumns == 64u);
    REQUIRE(Trs80M3VideoController::kRows == 16u);
    REQUIRE(Trs80M3VideoController::kVramSize == 1024u);
    REQUIRE(Trs80M3VideoController::kFramebufferWidth == 512u);
    REQUIRE(Trs80M3VideoController::kFramebufferHeight == 192u);
}

TEST_CASE("TRS-80 Model III video controller renders uppercase text", "[devices][trs80m3][fast]") {
    Framebuffer fb(Trs80M3VideoController::kFramebufferWidth, Trs80M3VideoController::kFramebufferHeight);
    Trs80M3VideoController video(fb);

    video.reset();
    REQUIRE(fb.getPixel(0, 0) == 0xFF000000u);

    video.write(0, 'A');
    REQUIRE(video.read(0) == 'A');
    REQUIRE(fb.getPixel(3, 2) == 0xFFFFFFFFu);
    REQUIRE(fb.getPixel(4, 5) == 0xFFFFFFFFu);
    REQUIRE(fb.getPixel(0, 11) == 0xFF000000u);
}

TEST_CASE("TRS-80 Model III video controller renders lowercase text", "[devices][trs80m3][fast]") {
    Framebuffer fb(Trs80M3VideoController::kFramebufferWidth, Trs80M3VideoController::kFramebufferHeight);
    Trs80M3VideoController video(fb);
    video.reset();

    video.write(0, 'A');
    const uint32_t upper_pixel = fb.getPixel(3, 4);

    video.write(0, 'a');
    const uint32_t lower_pixel = fb.getPixel(3, 4);
    REQUIRE(lower_pixel != upper_pixel);
    REQUIRE(fb.getPixel(3, 4) == 0xFFFFFFFFu);
}

TEST_CASE("TRS-80 Model III video controller renders block graphics", "[devices][trs80m3][fast]") {
    Framebuffer fb(Trs80M3VideoController::kFramebufferWidth, Trs80M3VideoController::kFramebufferHeight);
    Trs80M3VideoController video(fb);
    video.reset();

    video.write(0, 161);
    REQUIRE(fb.getPixel(1, 1) == 0xFFFFFFFFu);
    REQUIRE(fb.getPixel(2, 7) == 0xFFFFFFFFu);
    REQUIRE(fb.getPixel(6, 0) == 0xFF000000u);
}

TEST_CASE("TRS-80 Model III video controller maps 64x16 VRAM", "[devices][trs80m3][fast]") {
    Framebuffer fb(Trs80M3VideoController::kFramebufferWidth, Trs80M3VideoController::kFramebufferHeight);
    Trs80M3VideoController video(fb);
    video.reset();

    const uint16_t last = Trs80M3VideoController::kVramSize - 1;
    video.write(last, '@');
    REQUIRE(video.read(last) == '@');

    const uint16_t origin_x =
        static_cast<uint16_t>((last % Trs80M3VideoController::kColumns) * Trs80M3VideoController::kCellWidth);
    const uint16_t origin_y =
        static_cast<uint16_t>((last / Trs80M3VideoController::kColumns) * Trs80M3VideoController::kCellHeight);
    REQUIRE(fb.getPixel(origin_x + 2, origin_y + 2) == 0xFFFFFFFFu);
}

TEST_CASE("TRS-80 Model III video controller reset fills VRAM with spaces", "[devices][trs80m3][fast]") {
    Framebuffer fb(Trs80M3VideoController::kFramebufferWidth, Trs80M3VideoController::kFramebufferHeight);
    Trs80M3VideoController video(fb);

    video.write(0, 'Z');
    video.reset();
    REQUIRE(video.read(0) == 0x20);
}

TEST_CASE("TRS-80 Model III video controller doubles cell width in 32-column mode",
          "[devices][trs80m3][fast]") {
    Framebuffer fb(Trs80M3VideoController::kFramebufferWidth, Trs80M3VideoController::kFramebufferHeight);
    Trs80M3VideoController video(fb);
    video.reset();

    video.write(0, 'A');
    REQUIRE(fb.getPixel(3, 2) == 0xFFFFFFFFu);

    video.setWideMode(true);
    video.write(0, 'A');
    REQUIRE(fb.getPixel(4, 2) == 0xFFFFFFFFu);
    REQUIRE(fb.getPixel(5, 2) == 0xFFFFFFFFu);
    REQUIRE(fb.getPixel(6, 2) == 0xFFFFFFFFu);
    REQUIRE(fb.getPixel(7, 2) == 0xFFFFFFFFu);

    video.write(1, 'A');
    REQUIRE(fb.getPixel(20, 2) == 0xFFFFFFFFu);
    REQUIRE(fb.getPixel(21, 2) == 0xFFFFFFFFu);
    REQUIRE(fb.getPixel(22, 2) == 0xFFFFFFFFu);
    REQUIRE(fb.getPixel(23, 2) == 0xFFFFFFFFu);
}

TEST_CASE("TRS-80 Model III video controller doubles block graphics in 32-column mode",
          "[devices][trs80m3][fast]") {
    Framebuffer fb(Trs80M3VideoController::kFramebufferWidth, Trs80M3VideoController::kFramebufferHeight);
    Trs80M3VideoController video(fb);
    video.reset();

    video.write(0, 161);
    REQUIRE(fb.getPixel(1, 1) == 0xFFFFFFFFu);
    REQUIRE(fb.getPixel(2, 7) == 0xFFFFFFFFu);

    video.setWideMode(true);
    video.write(0, 161);
    REQUIRE(fb.getPixel(2, 1) == 0xFFFFFFFFu);
    REQUIRE(fb.getPixel(3, 1) == 0xFFFFFFFFu);
    REQUIRE(fb.getPixel(4, 7) == 0xFFFFFFFFu);
    REQUIRE(fb.getPixel(5, 7) == 0xFFFFFFFFu);
}
