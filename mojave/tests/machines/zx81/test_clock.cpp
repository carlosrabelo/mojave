#include "catch.hpp"
#include "machines/zx81/zx81_preset.hpp"
#include "machines/shared/machine.hpp"
#include "cpus/z80.hpp"
#include "devices/zx81/video_generator.hpp"
#include "devices/zx81/port_decode.hpp"
#include "session/clock.hpp"
#include "session/runner.hpp"

using Contract = Zx81PresetContract;

TEST_CASE("Sinclair ZX-81 guest CPU clock is 3.25 MHz", "[machine][zx81][fast]") {
    REQUIRE(Contract::guest_cpu_clock_hz == 3'250'000u);
    REQUIRE(Contract::video_frame_hz == 50u);
    REQUIRE(Contract::video_cycles_per_frame == 65'000u);
    REQUIRE(Contract::nmi_hz == 50u);
    REQUIRE(Contract::nmi_cycles_per_interrupt == Contract::video_cycles_per_frame);

    auto machine = createZx81Machine();
    REQUIRE(machine != nullptr);
    REQUIRE(machine->guestCpuClockHz() == Contract::guest_cpu_clock_hz);
}

TEST_CASE("Sinclair ZX-81 clock derives a 50 Hz frame cycle budget", "[machine][zx81][fast]") {
    Clock clock(Contract::guest_cpu_clock_hz);

    REQUIRE_FALSE(clock.unlimited());
    REQUIRE(clock.cyclesPerFrame(Contract::video_frame_hz) == Contract::video_cycles_per_frame);
}

TEST_CASE("Sinclair ZX-81 runner stepForFrame honors the guest clock budget", "[machine][zx81][fast]") {
    auto machine = createZx81Machine();
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

TEST_CASE("Sinclair ZX-81 video generator ticks once per frame cycle budget", "[machine][zx81][fast]") {
    auto machine = createZx81Machine();

    Zx81VideoGenerator* video = nullptr;
    for (const auto& dev : machine->ownedDevices()) {
        if (auto* gen = dynamic_cast<Zx81VideoGenerator*>(dev.get()))
            video = gen;
    }
    REQUIRE(video != nullptr);
    REQUIRE(video->cyclesPerFrame() == Contract::video_cycles_per_frame);

    machine->reset();
    REQUIRE(video->framesRendered() == 0);

    video->tick(Contract::video_cycles_per_frame - 1);
    REQUIRE(video->framesRendered() == 0);

    video->tick(1);
    REQUIRE(video->framesRendered() == 1);
}

TEST_CASE("Sinclair ZX-81 OUT FE enables line-rate NMI generator", "[machine][zx81][fast]") {
    auto machine = createZx81Machine();
    auto& z80 = dynamic_cast<Z80&>(machine->cpu());

    Zx81VideoGenerator* video = nullptr;
    Zx81PortDecode* ports = nullptr;
    for (const auto& dev : machine->ownedDevices()) {
        if (auto* gen = dynamic_cast<Zx81VideoGenerator*>(dev.get()))
            video = gen;
        if (auto* io = dynamic_cast<Zx81PortDecode*>(dev.get()))
            ports = io;
    }
    REQUIRE(video != nullptr);
    REQUIRE(ports != nullptr);

    machine->bus().write(0x0066, 0x76); // HALT at NMI vector
    z80.regs().pc = 0x4000;
    machine->bus().write(0x4000, 0x00); // NOP

    REQUIRE_FALSE(ports->nmiGeneratorOn());
    machine->bus().writePort(0x00FE, 0x00);
    REQUIRE(ports->nmiGeneratorOn());

    REQUIRE(video->nmisRaised() == 0);
    video->tick(Zx81VideoGenerator::kNmiPeriodCycles);
    REQUIRE(video->nmisRaised() == 1);

    machine->step();
    REQUIRE(z80.regs().pc == 0x0066);
}

TEST_CASE("Sinclair ZX-81 OUT FD disables NMI generator", "[machine][zx81][fast]") {
    auto machine = createZx81Machine();

    Zx81VideoGenerator* video = nullptr;
    Zx81PortDecode* ports = nullptr;
    for (const auto& dev : machine->ownedDevices()) {
        if (auto* gen = dynamic_cast<Zx81VideoGenerator*>(dev.get()))
            video = gen;
        if (auto* io = dynamic_cast<Zx81PortDecode*>(dev.get()))
            ports = io;
    }
    REQUIRE(video != nullptr);
    REQUIRE(ports != nullptr);

    machine->bus().writePort(0x00FE, 0x00);
    REQUIRE(ports->nmiGeneratorOn());
    machine->bus().writePort(0x00FD, 0x00);
    REQUIRE_FALSE(ports->nmiGeneratorOn());

    video->tick(Zx81VideoGenerator::kNmiPeriodCycles * 4);
    REQUIRE(video->nmisRaised() == 0);
}
