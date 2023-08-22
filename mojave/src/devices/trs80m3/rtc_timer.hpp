#ifndef MOJAVE_TRS80M3_RTC_TIMER_HPP
#define MOJAVE_TRS80M3_RTC_TIMER_HPP

#include <cstdint>
#include "devices/device.hpp"

class Trs80M3PortDecode;
class Z80;

class Trs80M3RtcTimer : public Device {
public:
    static constexpr uint32_t kInterruptHz = 30;

    Trs80M3RtcTimer(Trs80M3PortDecode& ports, Z80& cpu, uint32_t cpu_hz);

    uint8_t read(uint16_t address) override;
    void write(uint16_t address, uint8_t value) override;
    void reset() override;
    void tick(unsigned cycles) override;

    uint32_t cyclesPerInterrupt() const { return cycles_per_interrupt_; }
    uint32_t cycleAccumulator() const { return cycle_accumulator_; }

private:
    Trs80M3PortDecode& ports_;
    Z80& cpu_;
    uint32_t cycles_per_interrupt_;
    uint32_t cycle_accumulator_ = 0;
};

#endif
