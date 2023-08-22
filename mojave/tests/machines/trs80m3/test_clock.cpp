#include "catch.hpp"
#include "machines/trs80m3/trs80m3_preset.hpp"
#include "machines/shared/machine.hpp"
#include "cpus/z80.hpp"
#include "devices/trs80m3/port_decode.hpp"
#include "devices/trs80m3/rtc_timer.hpp"
#include "session/clock.hpp"
#include "session/runner.hpp"

using Contract = Trs80M3PresetContract;

TEST_CASE("TRS-80 Model III guest CPU clock is 2.027520 MHz", "[machine][trs80m3][fast]") {
    REQUIRE(Contract::guest_cpu_clock_hz == 2'027'520u);
    REQUIRE(Contract::rtc_interrupt_hz == 30u);
    REQUIRE(Contract::rtc_cycles_per_interrupt == 67584u);

    auto machine = createTrs80M3Machine();
    REQUIRE(machine != nullptr);
    REQUIRE(machine->guestCpuClockHz() == Contract::guest_cpu_clock_hz);
}

TEST_CASE("TRS-80 Model III clock derives a 60 Hz frame cycle budget", "[machine][trs80m3][fast]") {
    Clock clock(Contract::guest_cpu_clock_hz);

    REQUIRE_FALSE(clock.unlimited());
    REQUIRE(clock.cyclesPerFrame() == 33792u);
}

TEST_CASE("TRS-80 Model III runner stepForFrame honors the guest clock budget", "[machine][trs80m3][fast]") {
    auto machine = createTrs80M3Machine();
    Clock clock(machine->guestCpuClockHz());

    machine->bus().write(0x4000, 0x18); // JR -2
    machine->bus().write(0x4001, 0xFE);
    machine->reset();
    dynamic_cast<Z80&>(machine->cpu()).regs().pc = 0x4000;

    const unsigned budget = static_cast<unsigned>(clock.cyclesPerFrame());
    REQUIRE(budget == 33792u);

    const unsigned cycles = runner::stepForFrame(*machine, clock);
    REQUIRE(cycles == budget);
    REQUIRE_FALSE(machine->cpu().halted());
}

TEST_CASE("TRS-80 Model III RTC timer delivers maskable interrupt when enabled", "[machine][trs80m3][fast]") {
    auto machine = createTrs80M3Machine();
    auto& z80 = dynamic_cast<Z80&>(machine->cpu());
    z80.regs().im = 1;
    z80.regs().iff1 = true;

    machine->bus().write(0x0038, 0x76); // HALT in IM1 vector
    machine->bus().writePort(Contract::interrupt_latch_port_start, Trs80M3PortDecode::kRtcInterruptMask);
    z80.regs().pc = 0x4000;
    machine->bus().write(0x4000, 0x00); // NOP

    Trs80M3RtcTimer* timer = nullptr;
    for (const auto& dev : machine->ownedDevices()) {
        if (auto* found = dynamic_cast<Trs80M3RtcTimer*>(dev.get())) {
            timer = found;
            break;
        }
    }
    REQUIRE(timer != nullptr);

    timer->tick(Contract::rtc_cycles_per_interrupt);
    machine->step();

    REQUIRE(z80.regs().pc == 0x0038);
    REQUIRE_FALSE(z80.regs().iff1);
}
