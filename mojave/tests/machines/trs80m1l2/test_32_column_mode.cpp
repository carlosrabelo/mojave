#include "catch.hpp"
#include "machines/trs80m1l2/trs80m1l2_preset.hpp"
#include "machines/shared/machine.hpp"
#include "devices/shared/framebuffer.hpp"
#include "devices/trs80m1/system_port.hpp"
#include "devices/trs80m1/video_controller.hpp"

using Contract = Trs80M1L2PresetContract;

namespace {

Framebuffer* findMachineFramebuffer(Machine& machine) {
    for (const auto& dev : machine.ownedDevices()) {
        if (auto* fb = dynamic_cast<Framebuffer*>(dev.get()))
            return fb;
    }
    return nullptr;
}

Trs80M1SystemPort* findSystemPort(Machine& machine) {
    for (const auto& dev : machine.ownedDevices()) {
        if (auto* port = dynamic_cast<Trs80M1SystemPort*>(dev.get()))
            return port;
    }
    return nullptr;
}

} // namespace

TEST_CASE("TRS-80 Model I Level II 32-column mode doubles cell width", "[machine][trs80m1l2][fast]") {
    auto machine = createTrs80M1L2Machine();
    Framebuffer* fb = findMachineFramebuffer(*machine);
    REQUIRE(fb != nullptr);

    machine->bus().writePort(Trs80M1SystemPort::kPort, Trs80M1SystemPort::kWideScreenMask);
    machine->bus().write(Contract::vram_start, 'A');
    machine->bus().write(static_cast<uint16_t>(Contract::vram_start + 1), 'B');

    REQUIRE(fb->getPixel(3, 2) == 0xFFFFFFFFu);
    REQUIRE(fb->getPixel(4, 2) == 0xFFFFFFFFu);
    REQUIRE(fb->getPixel(14, 2) == 0xFFFFFFFFu);
    REQUIRE(fb->getPixel(15, 2) == 0xFFFFFFFFu);
}

TEST_CASE("TRS-80 Model I Level II 32-column mode hides columns 32-63", "[machine][trs80m1l2][fast]") {
    auto machine = createTrs80M1L2Machine();
    Framebuffer* fb = findMachineFramebuffer(*machine);
    REQUIRE(fb != nullptr);

    machine->bus().writePort(Trs80M1SystemPort::kPort, Trs80M1SystemPort::kWideScreenMask);

    const uint16_t hidden_column = static_cast<uint16_t>(Contract::vram_start + 32);
    machine->bus().write(hidden_column, 'Z');
    REQUIRE(machine->bus().read(hidden_column) == 'Z');
    REQUIRE(fb->getPixel(Trs80M1VideoController::kFramebufferWidth - 1, 0) == 0xFF000000u);
}

TEST_CASE("TRS-80 Model I Level II 32-column mode clears when port FF wide bit is off",
          "[machine][trs80m1l2][fast]") {
    auto machine = createTrs80M1L2Machine();
    Framebuffer* fb = findMachineFramebuffer(*machine);
    Trs80M1SystemPort* port = findSystemPort(*machine);
    REQUIRE(fb != nullptr);
    REQUIRE(port != nullptr);

    machine->bus().writePort(Trs80M1SystemPort::kPort, Trs80M1SystemPort::kWideScreenMask);
    machine->bus().write(Contract::vram_start, 'A');
    REQUIRE(fb->getPixel(3, 2) == 0xFFFFFFFFu);

    machine->bus().writePort(Trs80M1SystemPort::kPort, 0x00);
    REQUIRE_FALSE(port->wideScreen());
    machine->bus().write(Contract::vram_start, 'A');
    REQUIRE(fb->getPixel(2, 2) == 0xFFFFFFFFu);
    REQUIRE(fb->getPixel(3, 2) == 0xFF000000u);
}
