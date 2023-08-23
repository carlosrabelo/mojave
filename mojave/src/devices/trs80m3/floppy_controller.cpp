#include "devices/trs80m3/floppy_controller.hpp"
#include "devices/trs80m3/port_decode.hpp"
#include "cpus/z80.hpp"

Trs80M3FloppyController::Trs80M3FloppyController(Trs80M3PortDecode& ports) : ports_(ports) {}

uint8_t Trs80M3FloppyController::read(uint16_t address) {
    return readPort(address);
}

void Trs80M3FloppyController::write(uint16_t address, uint8_t value) {
    writePort(address, value);
}

uint8_t Trs80M3FloppyController::readPort(uint16_t port) {
    switch (port) {
    case kStatusOffset:
        // Stub: always look like an absent FDC. Model III ROM then takes the
        // Cass? / Memory Size / READY path. Real disk media is not emulated.
        if (status_pending_) {
            status_pending_ = false;
            return pending_status_;
        }
        return kStatusIdle;
    case kTrackOffset:
        return track_;
    case kSectorOffset:
        return sector_;
    case kDataOffset:
        return data_;
    case kDriveSelectOffset:
        return drive_select_;
    default:
        return 0xFF;
    }
}

void Trs80M3FloppyController::writePort(uint16_t port, uint8_t value) {
    switch (port) {
    case kStatusOffset:
        issueCommand(value);
        break;
    case kTrackOffset:
        track_ = value;
        break;
    case kSectorOffset:
        sector_ = value;
        break;
    case kDataOffset:
        data_ = value;
        break;
    case kDriveSelectOffset:
        // Latch the select byte only. Do not raise disk-index NMI yet: early
        // ROM init writes F4 and a premature NMI would derail Cass?/READY boot.
        drive_select_ = value;
        break;
    default:
        break;
    }
}

void Trs80M3FloppyController::issueCommand(uint8_t command) {
    last_command_ = command;
    status_pending_ = true;

    switch (command) {
    case 0x00: // RESTORE
        track_ = 0;
        pending_status_ = kStatusIdle;
        break;
    case 0xD0: // FORCE INTERRUPT
        track_ = 0;
        pending_status_ = kStatusIdle;
        break;
    default:
        pending_status_ = kStatusIdle;
        break;
    }
}

void Trs80M3FloppyController::pulseIndexNmi() {
    ports_.setDiskIndexPending(true);
    if (cpu_ != nullptr)
        cpu_->requestNmi();
}

void Trs80M3FloppyController::reset() {
    track_ = 0;
    sector_ = 0;
    data_ = 0;
    drive_select_ = 0;
    last_command_ = 0;
    pending_status_ = 0;
    status_pending_ = false;
}
