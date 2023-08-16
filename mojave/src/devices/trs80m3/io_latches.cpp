#include "devices/trs80m3/io_latches.hpp"

uint8_t Trs80M3IoLatches::diskDriveSelect(uint8_t drive) const {
    if (drive >= disk_drive_select_.size())
        return 0;
    return disk_drive_select_[drive];
}

uint8_t Trs80M3IoLatches::romImage(uint8_t offset) const {
    if (offset >= kSize)
        return 0xFF;
    return rom_image_[offset];
}

uint8_t Trs80M3IoLatches::read(uint16_t address) {
    if (address >= kSize)
        return 0xFF;

    // Only the printer status port replaces ROM on read. Every other byte in
    // 0x37E0-0x37EF must stay as the ROM image: BASIC jumps to 0x37EB after
    // Memory Size, and CALL 021B spans 0x37EB-0x37ED.
    if (address == kOffsetPrinter)
        return printer_ready_ ? kPrinterIdleReadValue : kPrinterNotReadyReadValue;

    return rom_image_[address];
}

void Trs80M3IoLatches::write(uint16_t address, uint8_t value) {
    if (address >= kSize)
        return;

    // Keep the ROM shadow so loadBinary can populate the overlay window.
    // Printer status is read-only hardware, so do not overwrite its ROM image
    // with output bytes (ROM has 0xFF here anyway).
    if (address != kOffsetPrinter && address != kOffsetPrinterOut)
        rom_image_[address] = value;

    switch (address) {
    case kOffsetInterruptLatch:
        interrupt_mask_ = value;
        break;
    case kOffsetDiskDrive0Select:
        disk_drive_select_[0] = value;
        break;
    case kOffsetCassetteLatch:
        cassette_latch_ = value;
        break;
    case kOffsetDiskDrive1Select:
        disk_drive_select_[1] = value;
        break;
    case kOffsetCassetteSelect:
        cassette_select_ = value;
        break;
    case kOffsetDiskDrive2Select:
        disk_drive_select_[2] = value;
        break;
    case kOffsetDiskDrive3Select:
        disk_drive_select_[3] = value;
        break;
    case kOffsetPrinter:
    case kOffsetPrinterOut:
        last_printer_byte_ = value;
        break;
    case kOffsetDiskStatus:
        disk_command_ = value;
        break;
    case kOffsetDiskTrack:
        disk_track_ = value;
        break;
    case kOffsetDiskSector:
        disk_sector_ = value;
        break;
    case kOffsetDiskData:
        disk_data_ = value;
        break;
    default:
        break;
    }
}

void Trs80M3IoLatches::reset() {
    // Keep rom_image_: it is firmware loaded into the 0x37E0 overlay, not
    // runtime latch state. Clearing it on reset would wipe CALL 021B at 37EB
    // and prevent BASIC from reaching READY after Memory Size.
    interrupt_mask_ = 0;
    cassette_latch_ = 0;
    cassette_select_ = 0;
    disk_drive_select_.fill(0);
    printer_ready_ = true;
    last_printer_byte_ = 0;
    disk_command_ = 0;
    disk_track_ = 0;
    disk_sector_ = 0;
    disk_data_ = 0;
}
