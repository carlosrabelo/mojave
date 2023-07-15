#include "catch.hpp"
#include "devices/trs80m1/printer_status.hpp"

TEST_CASE("TRS-80 Model I Level I printer status is active low on bit 0", "[devices][trs80m1l1][fast]") {
    Trs80M1PrinterStatus printer;

    REQUIRE(printer.read(0) == Trs80M1PrinterStatus::kIdleReadValue);
    REQUIRE(printer.ready());

    printer.setReady(false);
    REQUIRE(printer.read(0) == Trs80M1PrinterStatus::kNotReadyReadValue);
    REQUIRE_FALSE(printer.ready());

    printer.reset();
    REQUIRE(printer.read(0) == Trs80M1PrinterStatus::kIdleReadValue);
}

TEST_CASE("TRS-80 Model I Level I printer status ignores writes", "[devices][trs80m1l1][fast]") {
    Trs80M1PrinterStatus printer;

    printer.write(0, 0x00);
    REQUIRE(printer.read(0) == Trs80M1PrinterStatus::kIdleReadValue);

    printer.setReady(false);
    printer.write(0, 0xAA);
    REQUIRE(printer.read(0) == Trs80M1PrinterStatus::kNotReadyReadValue);
}
