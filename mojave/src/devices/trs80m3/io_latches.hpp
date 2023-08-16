#ifndef MOJAVE_TRS80M3_IO_LATCHES_HPP
#define MOJAVE_TRS80M3_IO_LATCHES_HPP

#include <array>
#include <cstdint>
#include "devices/device.hpp"

// Memory-mapped I/O window at 0x37E0-0x37EF.
//
// On the Model III this range overlays the top of the 14 KiB ROM. Only a few
// addresses are real I/O (printer, optional latches). Everything else must
// still return the underlying ROM bytes — BASIC jumps into 0x37EB after the
// Memory Size prompt, so swallowing those fetches with 0xFF prevents READY.
class Trs80M3IoLatches : public Device {
public:
    static constexpr uint16_t kBaseAddress = 0x37E0;
    static constexpr uint16_t kSize = 16;

    static constexpr uint8_t kOffsetInterruptLatch = 0x00;
    static constexpr uint8_t kOffsetDiskDrive0Select = 0x01;
    static constexpr uint8_t kOffsetCassetteLatch = 0x02;
    static constexpr uint8_t kOffsetDiskDrive1Select = 0x03;
    static constexpr uint8_t kOffsetCassetteSelect = 0x04;
    static constexpr uint8_t kOffsetDiskDrive2Select = 0x05;
    static constexpr uint8_t kOffsetDiskDrive3Select = 0x07;
    static constexpr uint8_t kOffsetPrinter = 0x08;
    static constexpr uint8_t kOffsetPrinterOut = 0x09;
    static constexpr uint8_t kOffsetDiskStatus = 0x0C;
    static constexpr uint8_t kOffsetDiskTrack = 0x0D;
    static constexpr uint8_t kOffsetDiskSector = 0x0E;
    static constexpr uint8_t kOffsetDiskData = 0x0F;

    static constexpr uint8_t kPrinterReadyBit = 0x01;
    static constexpr uint8_t kPrinterIdleReadValue = 0xFE;
    static constexpr uint8_t kPrinterNotReadyReadValue = 0xFF;
    static constexpr uint8_t kDiskIdleStatus = 0xFF;

    uint8_t read(uint16_t address) override;
    void write(uint16_t address, uint8_t value) override;
    void reset() override;

    void setPrinterReady(bool ready) { printer_ready_ = ready; }
    bool printerReady() const { return printer_ready_; }

    uint8_t interruptMask() const { return interrupt_mask_; }
    uint8_t cassetteLatch() const { return cassette_latch_; }
    uint8_t cassetteSelect() const { return cassette_select_; }
    uint8_t diskDriveSelect(uint8_t drive) const;
    uint8_t lastPrinterByte() const { return last_printer_byte_; }
    uint8_t diskCommand() const { return disk_command_; }
    uint8_t diskTrack() const { return disk_track_; }
    uint8_t diskSector() const { return disk_sector_; }
    uint8_t diskData() const { return disk_data_; }
    uint8_t romImage(uint8_t offset) const;

private:
    std::array<uint8_t, kSize> rom_image_{};
    uint8_t interrupt_mask_ = 0;
    uint8_t cassette_latch_ = 0;
    uint8_t cassette_select_ = 0;
    std::array<uint8_t, 4> disk_drive_select_{};
    bool printer_ready_ = true;
    uint8_t last_printer_byte_ = 0;
    uint8_t disk_command_ = 0;
    uint8_t disk_track_ = 0;
    uint8_t disk_sector_ = 0;
    uint8_t disk_data_ = 0;
};

#endif
