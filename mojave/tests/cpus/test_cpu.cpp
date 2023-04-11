#include <cstdint>
#include "catch.hpp"
#include "cpus/cpu.hpp"

namespace {

class TestCpu : public Cpu {
public:
    unsigned step_count = 0;
    bool is_halted = false;
    unsigned cycles_per_step = 4;

    void reset() override {
        step_count = 0;
        is_halted = false;
    }

    unsigned step() override {
        step_count++;
        return cycles_per_step;
    }

    bool halted() const override {
        return is_halted;
    }

    RegisterSnapshot registers() const override {
        RegisterSnapshot snap;
        snap.entries.push_back({"PC", 0x0100});
        snap.entries.push_back({"SP", 0xFFFE});
        return snap;
    }
};

}

TEST_CASE("Cpu can be subclassed and stepped", "[cpu][fast]") {
    TestCpu cpu;
    REQUIRE(cpu.step_count == 0);

    unsigned cycles = cpu.step();
    REQUIRE(cycles == 4);
    REQUIRE(cpu.step_count == 1);
}

TEST_CASE("Cpu reset clears state", "[cpu][fast]") {
    TestCpu cpu;
    cpu.step();
    cpu.step();
    REQUIRE(cpu.step_count == 2);

    cpu.reset();
    REQUIRE(cpu.step_count == 0);
}

TEST_CASE("Cpu halted returns correct state", "[cpu][fast]") {
    TestCpu cpu;
    REQUIRE_FALSE(cpu.halted());

    cpu.is_halted = true;
    REQUIRE(cpu.halted());
}

TEST_CASE("Cpu registers returns snapshot", "[cpu][fast]") {
    TestCpu cpu;
    auto snap = cpu.registers();

    REQUIRE(snap.entries.size() == 2);
    REQUIRE(snap.entries[0].name == "PC");
    REQUIRE(snap.entries[0].value == 0x0100);
    REQUIRE(snap.entries[1].name == "SP");
    REQUIRE(snap.entries[1].value == 0xFFFE);
}
