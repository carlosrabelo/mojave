#include "devices/trs80m3/rtc_timer.hpp"
#include "devices/trs80m3/port_decode.hpp"
#include "cpus/z80.hpp"

Trs80M3RtcTimer::Trs80M3RtcTimer(Trs80M3PortDecode& ports, Z80& cpu, uint32_t cpu_hz)
    : ports_(ports), cpu_(cpu), cycles_per_interrupt_(cpu_hz / kInterruptHz) {}

uint8_t Trs80M3RtcTimer::read(uint16_t /*address*/) {
    return 0xFF;
}

void Trs80M3RtcTimer::write(uint16_t /*address*/, uint8_t /*value*/) {}

void Trs80M3RtcTimer::reset() {
    cycle_accumulator_ = 0;
}

void Trs80M3RtcTimer::tick(unsigned cycles) {
    cycle_accumulator_ += cycles;
    while (cycle_accumulator_ >= cycles_per_interrupt_) {
        cycle_accumulator_ -= cycles_per_interrupt_;
        ports_.setRtcPending(true);
    }
}
