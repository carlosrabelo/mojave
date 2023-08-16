#include "catch.hpp"
#include "devices/trs80m3/io_latches.hpp"

TEST_CASE("TRS-80 Model III I/O latches expose printer idle status at 37E8", "[devices][trs80m3][fast]") {
    Trs80M3IoLatches latches;

    REQUIRE(latches.read(Trs80M3IoLatches::kOffsetPrinter) == Trs80M3IoLatches::kPrinterIdleReadValue);
    REQUIRE(latches.printerReady());

    latches.setPrinterReady(false);
    REQUIRE(latches.read(Trs80M3IoLatches::kOffsetPrinter) == Trs80M3IoLatches::kPrinterNotReadyReadValue);

    latches.reset();
    REQUIRE(latches.read(Trs80M3IoLatches::kOffsetPrinter) == Trs80M3IoLatches::kPrinterIdleReadValue);
}

TEST_CASE("TRS-80 Model III I/O latches decode cassette latches", "[devices][trs80m3][fast]") {
    Trs80M3IoLatches latches;

    latches.write(Trs80M3IoLatches::kOffsetCassetteLatch, 0x04);
    REQUIRE(latches.cassetteLatch() == 0x04);
    REQUIRE(latches.read(Trs80M3IoLatches::kOffsetCassetteLatch) == 0x04);

    latches.write(Trs80M3IoLatches::kOffsetCassetteSelect, 0x01);
    REQUIRE(latches.cassetteSelect() == 0x01);
    REQUIRE(latches.read(Trs80M3IoLatches::kOffsetCassetteSelect) == 0x01);
}

TEST_CASE("TRS-80 Model III I/O latches decode disk select and register latches", "[devices][trs80m3][fast]") {
    Trs80M3IoLatches latches;

    latches.write(Trs80M3IoLatches::kOffsetDiskDrive0Select, 0x01);
    REQUIRE(latches.diskDriveSelect(0) == 0x01);
    latches.write(Trs80M3IoLatches::kOffsetDiskDrive0Select, 0x02);
    latches.write(Trs80M3IoLatches::kOffsetDiskDrive2Select, 0x04);
    REQUIRE(latches.diskDriveSelect(0) == 0x02);
    REQUIRE(latches.diskDriveSelect(2) == 0x04);

    latches.write(Trs80M3IoLatches::kOffsetDiskStatus, 0x88);
    latches.write(Trs80M3IoLatches::kOffsetDiskTrack, 0x0A);
    latches.write(Trs80M3IoLatches::kOffsetDiskSector, 0x05);
    latches.write(Trs80M3IoLatches::kOffsetDiskData, 0x42);
    // Reads return the ROM shadow so instruction fetch through 0x37EC works.
    REQUIRE(latches.read(Trs80M3IoLatches::kOffsetDiskStatus) == 0x88);
    REQUIRE(latches.diskCommand() == 0x88);
    REQUIRE(latches.diskTrack() == 0x0A);
    REQUIRE(latches.diskSector() == 0x05);
    REQUIRE(latches.diskData() == 0x42);
}

TEST_CASE("TRS-80 Model III I/O latches preserve ROM image for non-printer reads",
          "[devices][trs80m3][fast]") {
    Trs80M3IoLatches latches;

    // Simulate loadBinary writing the Model III ROM overlay (CALL 021B at 37EB).
    latches.write(0x0B, 0xCD);
    latches.write(0x0C, 0x1B);
    latches.write(0x0D, 0x02);
    latches.write(0x0E, 0x21);
    latches.write(0x0F, 0x02);

    REQUIRE(latches.read(0x0B) == 0xCD);
    REQUIRE(latches.read(0x0C) == 0x1B);
    REQUIRE(latches.read(0x0D) == 0x02);
    REQUIRE(latches.read(0x0E) == 0x21);
    REQUIRE(latches.read(0x0F) == 0x02);
    REQUIRE(latches.romImage(0x0B) == 0xCD);

    latches.reset();
    REQUIRE(latches.read(0x0B) == 0xCD);
    REQUIRE(latches.cassetteLatch() == 0);
}

TEST_CASE("TRS-80 Model III I/O latches capture printer output bytes", "[devices][trs80m3][fast]") {
    Trs80M3IoLatches latches;

    latches.write(Trs80M3IoLatches::kOffsetPrinter, 'X');
    REQUIRE(latches.lastPrinterByte() == 'X');

    latches.write(Trs80M3IoLatches::kOffsetPrinterOut, 'Y');
    REQUIRE(latches.lastPrinterByte() == 'Y');
    REQUIRE(latches.read(Trs80M3IoLatches::kOffsetPrinter) == Trs80M3IoLatches::kPrinterIdleReadValue);
}

TEST_CASE("TRS-80 Model III I/O latches ignore out-of-range access", "[devices][trs80m3][fast]") {
    Trs80M3IoLatches latches;

    REQUIRE(latches.read(Trs80M3IoLatches::kSize) == 0xFF);
    latches.write(Trs80M3IoLatches::kSize, 0x55);
    REQUIRE(latches.cassetteLatch() == 0);
}
