#include "catch.hpp"
#include "devices/trs80m3/floppy_controller.hpp"
#include "devices/trs80m3/port_decode.hpp"

TEST_CASE("TRS-80 Model III floppy controller returns idle status at F0", "[devices][trs80m3][fast]") {
    Trs80M3PortDecode ports;
    Trs80M3FloppyController floppy(ports);
    floppy.reset();

    REQUIRE(floppy.readPort(Trs80M3FloppyController::kStatusOffset) ==
            Trs80M3FloppyController::kStatusIdle);
}

TEST_CASE("TRS-80 Model III floppy controller accepts restore without claiming media",
          "[devices][trs80m3][fast]") {
    Trs80M3PortDecode ports;
    Trs80M3FloppyController floppy(ports);

    floppy.writePort(Trs80M3FloppyController::kStatusOffset, 0x00);
    REQUIRE(floppy.lastCommand() == 0x00);
    REQUIRE(floppy.track() == 0);
    REQUIRE(floppy.readPort(Trs80M3FloppyController::kStatusOffset) ==
            Trs80M3FloppyController::kStatusIdle);
    REQUIRE(floppy.readPort(Trs80M3FloppyController::kStatusOffset) ==
            Trs80M3FloppyController::kStatusIdle);
}

TEST_CASE("TRS-80 Model III floppy controller reports idle on read sector stub",
          "[devices][trs80m3][fast]") {
    Trs80M3PortDecode ports;
    Trs80M3FloppyController floppy(ports);

    floppy.writePort(Trs80M3FloppyController::kDriveSelectOffset, 0x01);
    floppy.writePort(Trs80M3FloppyController::kStatusOffset, 0x80);
    REQUIRE(floppy.driveSelect() == 0x01);
    REQUIRE(floppy.readPort(Trs80M3FloppyController::kStatusOffset) ==
            Trs80M3FloppyController::kStatusIdle);
}

TEST_CASE("TRS-80 Model III floppy controller latches track sector and data registers",
          "[devices][trs80m3][fast]") {
    Trs80M3PortDecode ports;
    Trs80M3FloppyController floppy(ports);

    floppy.writePort(Trs80M3FloppyController::kTrackOffset, 0x0A);
    floppy.writePort(Trs80M3FloppyController::kSectorOffset, 0x05);
    floppy.writePort(Trs80M3FloppyController::kDataOffset, 0x42);

    REQUIRE(floppy.track() == 0x0A);
    REQUIRE(floppy.sector() == 0x05);
    REQUIRE(floppy.readPort(Trs80M3FloppyController::kDataOffset) == 0x42);
}
