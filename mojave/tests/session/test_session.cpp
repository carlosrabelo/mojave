#include <cstdint>
#include "catch.hpp"
#include "session/session.hpp"
#include "helpers.hpp"

namespace {

class HaltAfterCpu : public Cpu {
public:
    unsigned cycles_per_step = 7;
    unsigned steps_before_halt = 5;
    unsigned step_count = 0;
    bool reset_called = false;

    void reset() override {
        step_count = 0;
        reset_called = true;
    }

    unsigned step() override {
        step_count++;
        return cycles_per_step;
    }

    bool halted() const override {
        return step_count >= steps_before_halt;
    }
};

} // anonymous namespace

TEST_CASE("Session runs until CPU halts and counts steps/cycles", "[session][fast]") {
    auto cpu = std::make_unique<HaltAfterCpu>();
    cpu->steps_before_halt = 5;
    cpu->cycles_per_step = 7;

    Machine machine(std::move(cpu));
    VirtualTTY tty;

    Session session(machine, tty);
    REQUIRE(session.stepCount() == 0);
    REQUIRE(session.totalCycles() == 0);

    session.runUntilHalt();

    REQUIRE(session.stepCount() == 5);
    REQUIRE(session.totalCycles() == 5 * 7);
}

TEST_CASE("Session runUntilHalt stops immediately if CPU already halted", "[session][fast]") {
    auto cpu = std::make_unique<HaltAfterCpu>();
    cpu->steps_before_halt = 0; // already halted
    Machine machine(std::move(cpu));
    VirtualTTY tty;
    Session session(machine, tty);

    session.runUntilHalt();
    REQUIRE(session.stepCount() == 0);
}

TEST_CASE("Session reset clears counters", "[session][fast]") {
    auto cpu = std::make_unique<HaltAfterCpu>();
    cpu->steps_before_halt = 3;
    Machine machine(std::move(cpu));
    VirtualTTY tty;
    Session session(machine, tty);

    session.runUntilHalt();
    REQUIRE(session.stepCount() == 3);

    session.reset();
    REQUIRE(session.stepCount() == 0);
    REQUIRE(session.totalCycles() == 0);
}

TEST_CASE("Session pollTTY drains TTY output to host", "[session][fast]") {
    // Write to TTY, then run session and verify chars are drained
    auto cpu = std::make_unique<HaltAfterCpu>();
    cpu->steps_before_halt = 3;
    Machine machine(std::move(cpu));
    VirtualTTY tty;

    tty.writePort(0, 'H');
    tty.writePort(0, 'i');

    Session session(machine, tty);
    session.runUntilHalt();

    // After runUntilHalt, TTY TX buffer should be drained
    REQUIRE(tty.readChar() == -1);
}
