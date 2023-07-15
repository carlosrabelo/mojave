#include "devices/trs80m1/printer_status.hpp"

uint8_t Trs80M1PrinterStatus::read(uint16_t /*address*/) {
    return ready_ ? kIdleReadValue : kNotReadyReadValue;
}

void Trs80M1PrinterStatus::write(uint16_t /*address*/, uint8_t /*value*/) {}

void Trs80M1PrinterStatus::reset() {
    ready_ = true;
}
