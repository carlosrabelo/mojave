#include <cstdint>
#include "catch.hpp"
#include "machines/shared/machine.hpp"
#include "devices/shared/memory.hpp"

namespace {

class TickCpu : public Cpu {
public:
    bool is_halted = false;
    unsigned cycle_count = 4;
    unsigned step_count = 0;

    void reset() override { step_count = 0; is_halted = false; }
    unsigned step() override { step_count++; return cycle_count; }
    bool halted() const override { return is_halted; }
};

class TickDevice : public Device {
public:
    bool was_reset = false;
    unsigned total_cycles = 0;
    unsigned tick_count = 0;

    uint8_t read(uint16_t) override { return 0; }
    void write(uint16_t, uint8_t) override {}

    void reset() override { was_reset = true; }

    void tick(unsigned cycles) override {
        total_cycles += cycles;
        tick_count++;
    }
};

}

TEST_CASE("Machine step with stub CPU and Memory does not crash", "[machine]") {
    auto cpu = std::make_unique<TickCpu>();
    auto mem = std::make_unique<Memory>(64);
    Machine m(std::move(cpu));
    m.attachDevice(std::move(mem), 0x0000, 0x0040);
    REQUIRE_NOTHROW(m.step());
}

TEST_CASE("Machine reset calls Cpu::reset and Device::reset", "[machine]") {
    auto cpu = std::make_unique<TickCpu>();
    auto dev = std::make_unique<TickDevice>();
    TickDevice* dev_ptr = dev.get();
    Machine m(std::move(cpu));
    m.attachDevice(std::move(dev), 0x0000, 0x0010);

    m.step();
    m.step();
    m.reset();

    REQUIRE(static_cast<TickCpu&>(m.cpu()).step_count == 0);
    REQUIRE(dev_ptr->was_reset);
}

TEST_CASE("Machine step calls tick on devices with correct cycle count", "[machine]") {
    auto cpu = std::make_unique<TickCpu>();
    auto dev = std::make_unique<TickDevice>();
    TickCpu* cpu_ptr = static_cast<TickCpu*>(cpu.get());
    TickDevice* dev_ptr = dev.get();

    Machine m(std::move(cpu));
    m.attachDevice(std::move(dev), 0x0000, 0x0010);

    cpu_ptr->cycle_count = 7;
    unsigned cycles = m.step();
    REQUIRE(cycles == 7);
    REQUIRE(dev_ptr->tick_count == 1);
    REQUIRE(dev_ptr->total_cycles == 7);
}

TEST_CASE("Machine integration: multi-step with halted CPU", "[machine][integration]") {
    auto cpu = std::make_unique<TickCpu>();
    auto dev = std::make_unique<TickDevice>();
    auto mem = std::make_unique<Memory>(64);

    TickCpu* cpu_ptr = static_cast<TickCpu*>(cpu.get());
    TickDevice* dev_ptr = dev.get();

    Machine m(std::move(cpu));
    m.attachDevice(std::move(dev), 0x0000, 0x0010);
    m.attachDevice(std::move(mem), 0x1000, 0x1040);

    for (int i = 0; i < 5; i++)
        m.step();

    REQUIRE(cpu_ptr->step_count == 5);
    REQUIRE(dev_ptr->tick_count == 5);
    REQUIRE(dev_ptr->total_cycles == 5 * 4);

    REQUIRE_FALSE(m.cpu().halted());
    cpu_ptr->is_halted = true;
    REQUIRE(m.cpu().halted());
}
