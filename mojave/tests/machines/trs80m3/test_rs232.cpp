#include "catch.hpp"
#include "machines/trs80m3/trs80m3_preset.hpp"
#include "machines/shared/machine.hpp"
#include "devices/trs80m3/port_decode.hpp"

using Contract = Trs80M3PresetContract;

namespace {

Trs80M3PortDecode* findPortDecode(Machine& machine) {
    for (const auto& dev : machine.ownedDevices()) {
        if (auto* ports = dynamic_cast<Trs80M3PortDecode*>(dev.get()))
            return ports;
    }
    return nullptr;
}

} // namespace

TEST_CASE("TRS-80 Model III machine decodes RS-232 ports E8-EB", "[machine][trs80m3][fast]") {
    auto machine = createTrs80M3Machine();
    Trs80M3PortDecode* ports = findPortDecode(*machine);
    REQUIRE(ports != nullptr);

    REQUIRE(machine->bus().readPort(0xE8) == Trs80M3PortDecode::kRs232IdleStatus);
    REQUIRE(machine->bus().readPort(0xE9) == Trs80M3PortDecode::kDipSwitchReadValue);
    REQUIRE(machine->bus().readPort(0xEA) == Trs80M3PortDecode::kUartTxEmptyMask);

    machine->bus().writePort(0xEB, 'X');
    REQUIRE(ports->lastTxByte() == 'X');

    ports->queueRxByte('Y');
    REQUIRE(machine->bus().readPort(0xEA) == 0xC0);
    REQUIRE(machine->bus().readPort(0xEB) == 'Y');
    REQUIRE_FALSE(ports->rxFull());
}
