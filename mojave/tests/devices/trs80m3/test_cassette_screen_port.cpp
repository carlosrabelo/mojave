#include "catch.hpp"
#include "devices/trs80m3/cassette_screen_port.hpp"
#include "devices/trs80m3/video_controller.hpp"
#include "devices/shared/framebuffer.hpp"

TEST_CASE("TRS-80 Model III cassette screen port decodes cassette and screen mode writes",
          "[devices][trs80m3][fast]") {
    Framebuffer fb(Trs80M3VideoController::kFramebufferWidth, Trs80M3VideoController::kFramebufferHeight);
    Trs80M3VideoController video(fb);
    Trs80M3CassetteScreenPort port(video);

    port.writePort(Trs80M3CassetteScreenPort::kPortStart, 0x07);
    REQUIRE(port.cassetteOut() == 0x03);
    REQUIRE(port.motorOn());
    REQUIRE_FALSE(port.wideScreen());
    REQUIRE_FALSE(video.wideMode());

    port.writePort(Trs80M3CassetteScreenPort::kPortStart, 0x0F);
    REQUIRE(port.wideScreen());
    REQUIRE(video.wideMode());
}

TEST_CASE("TRS-80 Model III cassette screen port returns cassette input on read",
          "[devices][trs80m3][fast]") {
    Framebuffer fb(Trs80M3VideoController::kFramebufferWidth, Trs80M3VideoController::kFramebufferHeight);
    Trs80M3VideoController video(fb);
    Trs80M3CassetteScreenPort port(video);

    REQUIRE(port.readPort(Trs80M3CassetteScreenPort::kPortStart) ==
            Trs80M3CassetteScreenPort::kIdleCassetteRead);

    port.setCassetteIn(0x00);
    REQUIRE(port.readPort(Trs80M3CassetteScreenPort::kPortStart) == 0x00);
}

TEST_CASE("TRS-80 Model III cassette screen port reset clears latch and wide mode",
          "[devices][trs80m3][fast]") {
    Framebuffer fb(Trs80M3VideoController::kFramebufferWidth, Trs80M3VideoController::kFramebufferHeight);
    Trs80M3VideoController video(fb);
    Trs80M3CassetteScreenPort port(video);

    port.writePort(Trs80M3CassetteScreenPort::kPortStart, 0x0F);
    port.reset();

    REQUIRE(port.cassetteOut() == 0);
    REQUIRE_FALSE(port.motorOn());
    REQUIRE_FALSE(port.wideScreen());
    REQUIRE_FALSE(video.wideMode());
}

TEST_CASE("TRS-80 Model III cassette screen port toggles 32-column rendering",
          "[devices][trs80m3][fast]") {
    Framebuffer fb(Trs80M3VideoController::kFramebufferWidth, Trs80M3VideoController::kFramebufferHeight);
    Trs80M3VideoController video(fb);
    Trs80M3CassetteScreenPort port(video);
    video.reset();

    video.write(0, 'A');
    REQUIRE(fb.getPixel(3, 2) == 0xFFFFFFFFu);

    port.writePort(Trs80M3CassetteScreenPort::kPortStart, Trs80M3CassetteScreenPort::kWideScreenMask);
    video.write(0, 'A');
    REQUIRE(fb.getPixel(4, 2) == 0xFFFFFFFFu);
    REQUIRE(fb.getPixel(5, 2) == 0xFFFFFFFFu);
}
