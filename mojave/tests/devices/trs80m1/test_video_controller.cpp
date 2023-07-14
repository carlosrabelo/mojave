#include "devices/trs80m1/video_controller.hpp"
#include "devices/shared/framebuffer.hpp"
#include "catch.hpp"

TEST_CASE("TRS-80 Model I Level I video controller dimensions", "[devices][trs80m1l1][fast]") {
    REQUIRE(Trs80M1VideoController::kColumns == 64u);
    REQUIRE(Trs80M1VideoController::kRows == 16u);
    REQUIRE(Trs80M1VideoController::kVramSize == 1024u);
    REQUIRE(Trs80M1VideoController::kFramebufferWidth == 384u);
    REQUIRE(Trs80M1VideoController::kFramebufferHeight == 192u);
}

TEST_CASE("TRS-80 Model I Level I video controller renders uppercase text", "[devices][trs80m1l1][fast]") {
    Framebuffer fb(Trs80M1VideoController::kFramebufferWidth, Trs80M1VideoController::kFramebufferHeight);
    Trs80M1VideoController video(fb);

    video.reset();
    REQUIRE(fb.getPixel(0, 0) == 0xFF000000u);

    video.write(0, 'A');
    REQUIRE(video.read(0) == 'A');
    REQUIRE(fb.getPixel(2, 2) == 0xFFFFFFFFu);
    REQUIRE(fb.getPixel(3, 5) == 0xFFFFFFFFu);
    REQUIRE(fb.getPixel(0, 11) == 0xFF000000u);

    video.write(1, 'a');
    REQUIRE(fb.getPixel(8, 2) == 0xFFFFFFFFu);
}

TEST_CASE("TRS-80 Model I Level I video controller renders block graphics", "[devices][trs80m1l1][fast]") {
    Framebuffer fb(Trs80M1VideoController::kFramebufferWidth, Trs80M1VideoController::kFramebufferHeight);
    Trs80M1VideoController video(fb);
    video.reset();

    video.write(0, 129);
    REQUIRE(fb.getPixel(1, 1) == 0xFFFFFFFFu);
    REQUIRE(fb.getPixel(2, 3) == 0xFFFFFFFFu);
    REQUIRE(fb.getPixel(4, 0) == 0xFF000000u);
}

TEST_CASE("TRS-80 Model I Level I video controller maps 64x16 VRAM", "[devices][trs80m1l1][fast]") {
    Framebuffer fb(Trs80M1VideoController::kFramebufferWidth, Trs80M1VideoController::kFramebufferHeight);
    Trs80M1VideoController video(fb);
    video.reset();

    const uint16_t last = Trs80M1VideoController::kVramSize - 1;
    video.write(last, '@');
    REQUIRE(video.read(last) == '@');

    const uint16_t origin_x =
        static_cast<uint16_t>((last % Trs80M1VideoController::kColumns) * Trs80M1VideoController::kCellWidth);
    const uint16_t origin_y =
        static_cast<uint16_t>((last / Trs80M1VideoController::kColumns) * Trs80M1VideoController::kCellHeight);
    REQUIRE(fb.getPixel(origin_x + 1, origin_y + 2) == 0xFFFFFFFFu);
}

TEST_CASE("TRS-80 Model I Level I video controller reset fills VRAM with spaces", "[devices][trs80m1l1][fast]") {
    Framebuffer fb(Trs80M1VideoController::kFramebufferWidth, Trs80M1VideoController::kFramebufferHeight);
    Trs80M1VideoController video(fb);

    video.write(0, 'Z');
    video.reset();
    REQUIRE(video.read(0) == 0x20);
}

TEST_CASE("TRS-80 Model I Level I video controller doubles cell width in 32-column mode",
          "[devices][trs80m1l1][fast]") {
    Framebuffer fb(Trs80M1VideoController::kFramebufferWidth, Trs80M1VideoController::kFramebufferHeight);
    Trs80M1VideoController video(fb);
    video.reset();

    video.write(0, 'A');
    REQUIRE(fb.getPixel(2, 2) == 0xFFFFFFFFu);

    video.setWideMode(true);
    video.write(0, 'A');
    REQUIRE(fb.getPixel(3, 2) == 0xFFFFFFFFu);
    REQUIRE(fb.getPixel(4, 2) == 0xFFFFFFFFu);
    REQUIRE(fb.getPixel(5, 2) == 0xFFFFFFFFu);

    video.write(1, 'A');
    REQUIRE(fb.getPixel(14, 2) == 0xFFFFFFFFu);
    REQUIRE(fb.getPixel(15, 2) == 0xFFFFFFFFu);
}
