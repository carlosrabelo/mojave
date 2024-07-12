#include "catch.hpp"
#include "machines/zx80/zx80_preset.hpp"
#include "machines/shared/machine.hpp"
#include "cpus/z80.hpp"
#include "devices/zx80/video_generator.hpp"
#include "session/clock.hpp"
#include "session/runner.hpp"

using Contract = Zx80PresetContract;

TEST_CASE("Sinclair ZX-80 guest CPU clock is 3.25 MHz", "[machine][zx80][fast]") {
    REQUIRE(Contract::guest_cpu_clock_hz == 3'250'000u);
    REQUIRE(Contract::video_frame_hz == 50u);
    REQUIRE(Contract::video_cycles_per_frame == 65'000u);

    auto machine = createZx80Machine();
    REQUIRE(machine != nullptr);
    REQUIRE(machine->guestCpuClockHz() == Contract::guest_cpu_clock_hz);
}

TEST_CASE("Sinclair ZX-80 clock derives a 50 Hz frame cycle budget", "[machine][zx80][fast]") {
    Clock clock(Contract::guest_cpu_clock_hz);

    REQUIRE_FALSE(clock.unlimited());
    REQUIRE(clock.cyclesPerFrame(Contract::video_frame_hz) == Contract::video_cycles_per_frame);
}

TEST_CASE("Sinclair ZX-80 runner stepForFrame honors the guest clock budget", "[machine][zx80][fast]") {
    auto machine = createZx80Machine();
    Clock clock(machine->guestCpuClockHz());

    machine->bus().write(0x4000, 0x18); // JR -2
    machine->bus().write(0x4001, 0xFE);
    machine->reset();
    dynamic_cast<Z80&>(machine->cpu()).regs().pc = 0x4000;

    const unsigned budget = static_cast<unsigned>(clock.cyclesPerFrame(Contract::video_frame_hz));
    REQUIRE(budget == Contract::video_cycles_per_frame);

    const unsigned cycles = runner::stepForFrame(*machine, clock, Contract::video_frame_hz);
    REQUIRE(cycles >= budget);
    REQUIRE(cycles - budget <= 12u);
    REQUIRE_FALSE(machine->cpu().halted());
}

TEST_CASE("Sinclair ZX-80 video generator ticks once per frame cycle budget", "[machine][zx80][fast]") {
    auto machine = createZx80Machine();

    Zx80VideoGenerator* video = nullptr;
    for (const auto& dev : machine->ownedDevices()) {
        if (auto* gen = dynamic_cast<Zx80VideoGenerator*>(dev.get()))
            video = gen;
    }
    REQUIRE(video != nullptr);

    machine->reset();
    REQUIRE(video->framesRendered() == 0);

    video->tick(Contract::video_cycles_per_frame - 1);
    REQUIRE(video->framesRendered() == 0);

    video->tick(1);
    REQUIRE(video->framesRendered() == 1);
}
