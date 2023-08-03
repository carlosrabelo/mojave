#include "catch.hpp"
#include "devices/trs80m1l2/expansion_ports.hpp"

TEST_CASE("TRS-80 Model I Level II expansion ports decode serial status at E8", "[devices][trs80m1l2][fast]") {
    Trs80M1L2ExpansionPorts ports;
    ports.reset();

    REQUIRE(ports.readPort(Trs80M1L2ExpansionPorts::kSerialOffset) == Trs80M1L2ExpansionPorts::kSerialStatusMask);
    ports.writePort(Trs80M1L2ExpansionPorts::kSerialOffset, 0x00);
    REQUIRE(ports.readPort(Trs80M1L2ExpansionPorts::kSerialOffset) == Trs80M1L2ExpansionPorts::kSerialStatusMask);
}

TEST_CASE("TRS-80 Model I Level II expansion ports decode baud and UART at E9 and EA",
          "[devices][trs80m1l2][fast]") {
    Trs80M1L2ExpansionPorts ports;

    REQUIRE(ports.readPort(Trs80M1L2ExpansionPorts::kBaudOffset) == Trs80M1L2ExpansionPorts::kDipSwitchReadValue);
    ports.writePort(Trs80M1L2ExpansionPorts::kBaudOffset, 0x77);
    REQUIRE(ports.baudLatch() == 0x77);

    REQUIRE(ports.readPort(Trs80M1L2ExpansionPorts::kUartOffset) == Trs80M1L2ExpansionPorts::kUartTxEmptyMask);
    ports.writePort(Trs80M1L2ExpansionPorts::kUartOffset, 0xA5);
    REQUIRE(ports.uartControl() == 0xA5);
}

TEST_CASE("TRS-80 Model I Level II expansion ports decode serial data at EB", "[devices][trs80m1l2][fast]") {
    Trs80M1L2ExpansionPorts ports;

    ports.writePort(Trs80M1L2ExpansionPorts::kSerialDataOffset, 'Z');
    REQUIRE(ports.lastTxByte() == 'Z');

    ports.queueRxByte('Q');
    REQUIRE(ports.readPort(Trs80M1L2ExpansionPorts::kUartOffset) == 0xC0);
    REQUIRE(ports.readPort(Trs80M1L2ExpansionPorts::kSerialDataOffset) == 'Q');
    REQUIRE_FALSE(ports.rxFull());
    REQUIRE(ports.readPort(Trs80M1L2ExpansionPorts::kSerialDataOffset) == 0x00);
}

TEST_CASE("TRS-80 Model I Level II expansion ports decode printer latch at EC", "[devices][trs80m1l2][fast]") {
    Trs80M1L2ExpansionPorts ports;

    REQUIRE(ports.readPort(Trs80M1L2ExpansionPorts::kPrinterOffset) == Trs80M1L2ExpansionPorts::kPrinterPortStatus);
    ports.writePort(Trs80M1L2ExpansionPorts::kPrinterOffset, 'P');
    REQUIRE(ports.lastPrinterByte() == 'P');
    REQUIRE(ports.readPort(Trs80M1L2ExpansionPorts::kPrinterOffset) == Trs80M1L2ExpansionPorts::kPrinterPortStatus);
}

TEST_CASE("TRS-80 Model I Level II expansion ports return idle values on ED-EF", "[devices][trs80m1l2][fast]") {
    Trs80M1L2ExpansionPorts ports;

    REQUIRE(ports.readPort(5) == 0xFF);
    REQUIRE(ports.readPort(6) == 0xFF);
    REQUIRE(ports.readPort(7) == 0xFF);

    ports.writePort(5, 0x55);
    ports.writePort(6, 0x66);
    ports.writePort(7, 0x77);
    REQUIRE(ports.readPort(5) == 0xFF);
}
