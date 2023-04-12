#include "catch.hpp"
#include "helpers.hpp"

TEST_CASE("createMemory creates zeroed memory", "[helpers][fast]") {
    auto mem = createMemory(32);
    REQUIRE(mem->read(0) == 0);
    REQUIRE(mem->read(31) == 0);
}

TEST_CASE("createMemoryFilled fills with pattern", "[helpers][fast]") {
    auto mem = createMemoryFilled(16, 0x10);
    REQUIRE(mem->read(0) == 0x10);
    REQUIRE(mem->read(5) == 0x15);
    REQUIRE(mem->read(15) == 0x1F);
}

TEST_CASE("createBusWithRam creates bus with attached RAM", "[helpers][fast]") {
    auto [bus, ram] = createBusWithRam(0x8000, 0x8100);
    REQUIRE(bus->read(0x7FFF) == 0xFF);
    REQUIRE(bus->read(0x8000) == 0);
    bus->write(0x8000, 0x42);
    REQUIRE(bus->read(0x8000) == 0x42);
    bus->write(0x80FE, 0x99);
    REQUIRE(bus->read(0x80FE) == 0x99);
    REQUIRE(bus->read(0x8100) == 0xFF);
}

TEST_CASE("MockDevice logs reads and writes", "[helpers][fast]") {
    MockDevice dev;
    dev.read_value = 0xCC;

    dev.write(0x1000, 0xAA);
    dev.write(0x2000, 0xBB);
    auto val = dev.read(0x3000);

    REQUIRE(val == 0xCC);
    REQUIRE(dev.log.size() == 3);
    REQUIRE(dev.log[0].is_read == false);
    REQUIRE(dev.log[0].address == 0x1000);
    REQUIRE(dev.log[0].value == 0xAA);
    REQUIRE(dev.log[1].address == 0x2000);
    REQUIRE(dev.log[2].is_read == true);
    REQUIRE(dev.log[2].address == 0x3000);
    REQUIRE(dev.log[2].value == 0xCC);
}

TEST_CASE("MockDevice reset and tick", "[helpers][fast]") {
    MockDevice dev;
    dev.reset();
    REQUIRE(dev.reset_called);
    dev.tick(4);
    dev.tick(6);
    REQUIRE(dev.tick_count == 2);
    REQUIRE(dev.total_tick_cycles == 10);
}

TEST_CASE("MockCpu step, halt, reset", "[helpers][fast]") {
    MockCpu cpu;
    REQUIRE_FALSE(cpu.halted());

    REQUIRE(cpu.step() == 4);
    REQUIRE(cpu.step_count == 1);

    cpu.is_halted = true;
    REQUIRE(cpu.halted());

    cpu.reset();
    REQUIRE(cpu.reset_called);
    REQUIRE_FALSE(cpu.halted());
    REQUIRE(cpu.step_count == 0);
}
