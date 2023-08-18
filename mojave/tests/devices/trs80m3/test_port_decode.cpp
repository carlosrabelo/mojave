#include "catch.hpp"
#include "devices/trs80m3/port_decode.hpp"

TEST_CASE("TRS-80 Model III port decode maps interrupt latch at E0-E3", "[devices][trs80m3][fast]") {
    Trs80M3PortDecode ports;
    ports.reset();

    REQUIRE(ports.readPort(0x00) == 0xFF);
    REQUIRE_FALSE(ports.rtcEnabled());

    ports.writePort(0x00, Trs80M3PortDecode::kRtcInterruptMask);
    REQUIRE(ports.interruptMask() == Trs80M3PortDecode::kRtcInterruptMask);
    REQUIRE(ports.rtcEnabled());

    ports.setRtcPending(true);
    REQUIRE(ports.readPort(0x01) == static_cast<uint8_t>(~Trs80M3PortDecode::kRtcInterruptMask));
    REQUIRE(ports.readPort(0x03) == static_cast<uint8_t>(~Trs80M3PortDecode::kRtcInterruptMask));
}

TEST_CASE("TRS-80 Model III port decode acknowledges RTC on hardware control read",
          "[devices][trs80m3][fast]") {
    Trs80M3PortDecode ports;
    ports.setRtcPending(true);
    REQUIRE(ports.interruptStatus() == static_cast<uint8_t>(~Trs80M3PortDecode::kRtcInterruptMask));

    REQUIRE(ports.readPort(Trs80M3PortDecode::kHardwareControlOffset) == 0xFF);
    REQUIRE(ports.interruptStatus() == 0xFF);
}

TEST_CASE("TRS-80 Model III port decode maps NMI options at E4-E7", "[devices][trs80m3][fast]") {
    Trs80M3PortDecode ports;

    REQUIRE(ports.readPort(Trs80M3PortDecode::kNmiOffset) == Trs80M3PortDecode::kNmiIdleStatus);
    ports.setDiskIndexPending(true);
    REQUIRE(ports.readPort(Trs80M3PortDecode::kNmiOffset) == Trs80M3PortDecode::kDiskIndexStatus);
    REQUIRE(ports.readPort(Trs80M3PortDecode::kNmiOffset) == Trs80M3PortDecode::kNmiIdleStatus);
    ports.writePort(Trs80M3PortDecode::kNmiOffset, 0xC0);
    REQUIRE(ports.nmiMask() == 0xC0);
    REQUIRE(ports.readPort(0x07) == Trs80M3PortDecode::kNmiIdleStatus);
}

TEST_CASE("TRS-80 Model III port decode returns idle modem status at E8", "[devices][trs80m3][fast]") {
    Trs80M3PortDecode ports;

    REQUIRE(ports.readPort(Trs80M3PortDecode::kRs232StatusOffset) ==
            Trs80M3PortDecode::kRs232IdleStatus);
    ports.writePort(Trs80M3PortDecode::kRs232StatusOffset, 0x55);
    REQUIRE(ports.readPort(Trs80M3PortDecode::kRs232StatusOffset) ==
            Trs80M3PortDecode::kRs232IdleStatus);
}

TEST_CASE("TRS-80 Model III port decode maps RS-232 baud and UART at E9 and EA",
          "[devices][trs80m3][fast]") {
    Trs80M3PortDecode ports;

    REQUIRE(ports.readPort(0x09) == Trs80M3PortDecode::kDipSwitchReadValue);
    ports.writePort(0x09, 0x77);
    REQUIRE(ports.baudLatch() == 0x77);

    REQUIRE(ports.readPort(0x0A) == Trs80M3PortDecode::kUartTxEmptyMask);
    ports.writePort(0x0A, 0xA5);
    REQUIRE(ports.uartControl() == 0xA5);
}

TEST_CASE("TRS-80 Model III port decode maps RS-232 data at EB", "[devices][trs80m3][fast]") {
    Trs80M3PortDecode ports;

    ports.writePort(0x0B, 'Z');
    REQUIRE(ports.lastTxByte() == 'Z');

    ports.queueRxByte('Q');
    REQUIRE(ports.readPort(0x0A) == 0xC0);
    REQUIRE(ports.readPort(0x0B) == 'Q');
    REQUIRE_FALSE(ports.rxFull());
    REQUIRE(ports.readPort(0x0B) == 0x00);
}

TEST_CASE("TRS-80 Model III port decode reset clears RS-232 latches", "[devices][trs80m3][fast]") {
    Trs80M3PortDecode ports;

    ports.writePort(0x09, 0x77);
    ports.writePort(0x0B, 'Z');
    ports.queueRxByte('Q');
    ports.reset();

    REQUIRE(ports.baudLatch() == 0);
    REQUIRE(ports.lastTxByte() == 0);
    REQUIRE_FALSE(ports.rxFull());
}

TEST_CASE("TRS-80 Model III port decode latches hardware control at EC-EF",
          "[devices][trs80m3][fast]") {
    Trs80M3PortDecode ports;
    ports.reset();

    REQUIRE_FALSE(ports.cassetteMotorOn());
    REQUIRE_FALSE(ports.doubleWidth());

    const uint8_t value = static_cast<uint8_t>(Trs80M3PortDecode::kDoubleWidthMask |
                                               Trs80M3PortDecode::kVideoWaitMask);
    ports.writePort(Trs80M3PortDecode::kHardwareControlOffset, value);
    REQUIRE(ports.hardwareControl() == value);
    REQUIRE(ports.doubleWidth());
    REQUIRE(ports.videoWaitsEnabled());
    REQUIRE(ports.cassetteMotorOn());

    ports.writePort(0x0F, Trs80M3PortDecode::kCassetteMotorOffMask);
    REQUIRE_FALSE(ports.cassetteMotorOn());
    REQUIRE_FALSE(ports.doubleWidth());
}
