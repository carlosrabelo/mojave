#include "catch.hpp"
#include "cpus/z80.hpp"
#include "devices/shared/memory.hpp"
#include "machines/shared/machine.hpp"
#include "session/clock.hpp"
#include "session/runner.hpp"

namespace {

uint16_t registerValue(const RegisterSnapshot& snap, const char* name) {
    for (const auto& entry : snap.entries) {
        if (entry.name == name)
            return entry.value;
    }
    return 0xFFFF;
}

std::unique_ptr<Machine> makeHaltedZ80(uint32_t clock_hz) {
    auto cpu = std::make_unique<Z80>();
    auto machine = std::make_unique<Machine>(std::move(cpu));
    machine->setGuestCpuClockHz(clock_hz);

    auto ram1 = std::make_unique<Memory>(0x8000);
    auto ram2 = std::make_unique<Memory>(0x8000);
    ram1->write(0x0000, 0x76); // HALT
    machine->attachDevice(std::move(ram1), 0x0000, 0x8000);
    machine->attachDevice(std::move(ram2), 0x8000, 0x0000); // end 0 = wrap / full top
    machine->reset();
    return machine;
}

} // namespace

TEST_CASE("runner::boot resets the machine", "[session][fast]") {
    auto machine = makeHaltedZ80(3'250'000);
    machine->step();
    REQUIRE(machine->cpu().halted());

    runner::boot(*machine);
    REQUIRE_FALSE(machine->cpu().halted());
    REQUIRE(registerValue(machine->cpu().registers(), "PC") == 0);
}

TEST_CASE("runner::stepForFrame burns full frame budget even while HALTed",
          "[session][fast]") {
    constexpr uint32_t kHz = 3'250'000;
    constexpr uint64_t kFrameHz = 50;
    auto machine = makeHaltedZ80(kHz);
    Clock clock(kHz);

    machine->step();
    REQUIRE(machine->cpu().halted());

    const unsigned cycles = runner::stepForFrame(*machine, clock, kFrameHz);
    REQUIRE(cycles >= clock.cyclesPerFrame(kFrameHz));
    REQUIRE(machine->cpu().halted());
}

TEST_CASE("runner::stepForFrame returns zero when frame budget is zero", "[session][fast]") {
    auto machine = makeHaltedZ80(0);
    Clock clock(0);
    REQUIRE(runner::stepForFrame(*machine, clock, 50) == 0);
}
