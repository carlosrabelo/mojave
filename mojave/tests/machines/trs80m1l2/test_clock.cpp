#include "catch.hpp"
#include "machines/trs80m1l2/trs80m1l2_preset.hpp"
#include "machines/shared/machine.hpp"
#include "cpus/z80.hpp"
#include "session/clock.hpp"
#include "session/runner.hpp"

TEST_CASE("TRS-80 Model I Level II guest CPU clock is 1.774080 MHz", "[machine][trs80m1l2][fast]") {
    using Contract = Trs80M1L2PresetContract;

    REQUIRE(Contract::guest_cpu_clock_hz == 1'774'080u);

    auto machine = createTrs80M1L2Machine();
    REQUIRE(machine != nullptr);
    REQUIRE(machine->guestCpuClockHz() == Contract::guest_cpu_clock_hz);
}

TEST_CASE("TRS-80 Model I Level II clock derives a 60 Hz frame cycle budget", "[machine][trs80m1l2][fast]") {
    Clock clock(Trs80M1L2PresetContract::guest_cpu_clock_hz);

    REQUIRE_FALSE(clock.unlimited());
    REQUIRE(clock.cyclesPerFrame() == 29568u);
}

TEST_CASE("TRS-80 Model I Level II runner stepForFrame honors the guest clock budget",
          "[machine][trs80m1l2][fast]") {
    auto machine = createTrs80M1L2Machine();
    Clock clock(machine->guestCpuClockHz());

    machine->bus().write(0x4000, 0x18); // JR -2
    machine->bus().write(0x4001, 0xFE);
    machine->reset();
    dynamic_cast<Z80&>(machine->cpu()).regs().pc = 0x4000;

    const unsigned budget = static_cast<unsigned>(clock.cyclesPerFrame());
    REQUIRE(budget == 29568u);

    const unsigned cycles = runner::stepForFrame(*machine, clock);
    REQUIRE(cycles == budget);
    REQUIRE_FALSE(machine->cpu().halted());
}
