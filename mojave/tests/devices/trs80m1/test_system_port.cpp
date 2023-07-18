#include "catch.hpp"
#include "devices/trs80m1/system_port.hpp"
#include "devices/trs80m1/video_controller.hpp"
#include "devices/shared/framebuffer.hpp"

TEST_CASE("TRS-80 Model I Level I system port decodes cassette and screen mode writes",
          "[devices][trs80m1l1][fast]") {
    Framebuffer fb(Trs80M1VideoController::kFramebufferWidth, Trs80M1VideoController::kFramebufferHeight);
    Trs80M1VideoController video(fb);
    Trs80M1SystemPort port(video);

    port.writePort(Trs80M1SystemPort::kPort, 0x07);
    REQUIRE(port.cassetteOut() == 0x03);
    REQUIRE(port.motorOn());
    REQUIRE_FALSE(port.wideScreen());
    REQUIRE_FALSE(video.wideMode());

    port.writePort(Trs80M1SystemPort::kPort, 0x0F);
    REQUIRE(port.wideScreen());
    REQUIRE(video.wideMode());
}

TEST_CASE("TRS-80 Model I Level I system port returns cassette input on read", "[devices][trs80m1l1][fast]") {
    Framebuffer fb(Trs80M1VideoController::kFramebufferWidth, Trs80M1VideoController::kFramebufferHeight);
    Trs80M1VideoController video(fb);
    Trs80M1SystemPort port(video);

    REQUIRE(port.readPort(Trs80M1SystemPort::kPort) == Trs80M1SystemPort::kIdleCassetteRead);

    port.setCassetteIn(0x05);
    REQUIRE(port.readPort(Trs80M1SystemPort::kPort) == 0xA0);
}

TEST_CASE("TRS-80 Model I Level I system port reset clears latch and wide mode", "[devices][trs80m1l1][fast]") {
    Framebuffer fb(Trs80M1VideoController::kFramebufferWidth, Trs80M1VideoController::kFramebufferHeight);
    Trs80M1VideoController video(fb);
    Trs80M1SystemPort port(video);

    port.writePort(Trs80M1SystemPort::kPort, 0x0F);
    port.reset();

    REQUIRE(port.cassetteOut() == 0);
    REQUIRE_FALSE(port.motorOn());
    REQUIRE_FALSE(port.wideScreen());
    REQUIRE_FALSE(video.wideMode());
}

TEST_CASE("TRS-80 Model I Level I system port toggles 32-column rendering", "[devices][trs80m1l1][fast]") {
    Framebuffer fb(Trs80M1VideoController::kFramebufferWidth, Trs80M1VideoController::kFramebufferHeight);
    Trs80M1VideoController video(fb);
    Trs80M1SystemPort port(video);
    video.reset();

    video.write(0, 'A');
    REQUIRE(fb.getPixel(3, 2) == 0xFF000000u);

    port.writePort(Trs80M1SystemPort::kPort, Trs80M1SystemPort::kWideScreenMask);
    video.write(0, 'A');
    REQUIRE(fb.getPixel(3, 2) == 0xFFFFFFFFu);
}
