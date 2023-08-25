#include "catch.hpp"
#include "machines/trs80m3/trs80m3_preset.hpp"
#include "machines/shared/machine.hpp"
#include "devices/shared/framebuffer.hpp"
#include "devices/trs80m3/cassette_screen_port.hpp"
#include "devices/trs80m3/video_controller.hpp"

using Contract = Trs80M3PresetContract;

namespace {

Framebuffer* findMachineFramebuffer(Machine& machine) {
    for (const auto& dev : machine.ownedDevices()) {
        if (auto* fb = dynamic_cast<Framebuffer*>(dev.get()))
            return fb;
    }
    return nullptr;
}

Trs80M3CassetteScreenPort* findCassetteScreenPort(Machine& machine) {
    for (const auto& dev : machine.ownedDevices()) {
        if (auto* port = dynamic_cast<Trs80M3CassetteScreenPort*>(dev.get()))
            return port;
    }
    return nullptr;
}

} // namespace

TEST_CASE("TRS-80 Model III 32-column mode doubles cell width", "[machine][trs80m3][fast]") {
    auto machine = createTrs80M3Machine();
    Framebuffer* fb = findMachineFramebuffer(*machine);
    REQUIRE(fb != nullptr);

    machine->bus().writePort(Contract::cassette_screen_port_start,
                             Trs80M3CassetteScreenPort::kWideScreenMask);
    machine->bus().write(Contract::vram_start, 'A');
    machine->bus().write(static_cast<uint16_t>(Contract::vram_start + 1), 'B');

    REQUIRE(fb->getPixel(4, 2) == 0xFFFFFFFFu);
    REQUIRE(fb->getPixel(5, 2) == 0xFFFFFFFFu);
    REQUIRE(fb->getPixel(20, 2) == 0xFFFFFFFFu);
    REQUIRE(fb->getPixel(21, 2) == 0xFFFFFFFFu);
}

TEST_CASE("TRS-80 Model III 32-column mode hides columns 32-63", "[machine][trs80m3][fast]") {
    auto machine = createTrs80M3Machine();
    Framebuffer* fb = findMachineFramebuffer(*machine);
    REQUIRE(fb != nullptr);

    machine->bus().writePort(Contract::cassette_screen_port_start,
                             Trs80M3CassetteScreenPort::kWideScreenMask);

    const uint16_t hidden_column = static_cast<uint16_t>(Contract::vram_start + 32);
    machine->bus().write(hidden_column, 'Z');
    REQUIRE(machine->bus().read(hidden_column) == 'Z');
    REQUIRE(fb->getPixel(Trs80M3VideoController::kFramebufferWidth - 1, 0) == 0xFF000000u);
}

TEST_CASE("TRS-80 Model III 32-column mode clears when port FC wide bit is off",
          "[machine][trs80m3][fast]") {
    auto machine = createTrs80M3Machine();
    Framebuffer* fb = findMachineFramebuffer(*machine);
    Trs80M3CassetteScreenPort* port = findCassetteScreenPort(*machine);
    REQUIRE(fb != nullptr);
    REQUIRE(port != nullptr);

    machine->bus().writePort(Contract::cassette_screen_port_start,
                             Trs80M3CassetteScreenPort::kWideScreenMask);
    machine->bus().write(Contract::vram_start, 'A');
    REQUIRE(fb->getPixel(4, 2) == 0xFFFFFFFFu);

    machine->bus().writePort(Contract::cassette_screen_port_start, 0x00);
    REQUIRE_FALSE(port->wideScreen());
    machine->bus().write(Contract::vram_start, 'A');
    REQUIRE(fb->getPixel(3, 2) == 0xFFFFFFFFu);
    REQUIRE(fb->getPixel(4, 2) == 0xFF000000u);
}

TEST_CASE("TRS-80 Model III 32-column mode doubles block graphics cell width",
          "[machine][trs80m3][fast]") {
    auto machine = createTrs80M3Machine();
    Framebuffer* fb = findMachineFramebuffer(*machine);
    REQUIRE(fb != nullptr);

    machine->bus().writePort(Contract::cassette_screen_port_start,
                             Trs80M3CassetteScreenPort::kWideScreenMask);
    machine->bus().write(Contract::vram_start, 161);

    REQUIRE(fb->getPixel(2, 1) == 0xFFFFFFFFu);
    REQUIRE(fb->getPixel(3, 1) == 0xFFFFFFFFu);
    REQUIRE(fb->getPixel(4, 7) == 0xFFFFFFFFu);
    REQUIRE(fb->getPixel(5, 7) == 0xFFFFFFFFu);
}
