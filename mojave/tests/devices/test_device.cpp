#include <cstdint>
#include "catch.hpp"
#include "devices/device.hpp"

namespace {

class TestDevice : public Device {
public:
    uint16_t last_read_addr = 0;
    uint16_t last_write_addr = 0;
    uint8_t last_write_val = 0;
    bool was_reset = false;
    unsigned total_tick_cycles = 0;
    unsigned tick_call_count = 0;

    uint8_t read(uint16_t address) override {
        last_read_addr = address;
        return 0xAA;
    }

    void write(uint16_t address, uint8_t value) override {
        last_write_addr = address;
        last_write_val = value;
    }

    void reset() override {
        was_reset = true;
    }

    void tick(unsigned cycles_elapsed) override {
        total_tick_cycles += cycles_elapsed;
        tick_call_count++;
    }
};

}

TEST_CASE("Device can be subclassed", "[device][fast]") {
    TestDevice dev;
    REQUIRE(dev.read(0x1234) == 0xAA);
}

TEST_CASE("Device write reaches subclass", "[device][fast]") {
    TestDevice dev;
    dev.write(0x4321, 0xBB);
    REQUIRE(dev.last_write_addr == 0x4321);
    REQUIRE(dev.last_write_val == 0xBB);
}

TEST_CASE("Device tick accumulates cycles", "[device][fast]") {
    TestDevice dev;
    dev.tick(4);
    dev.tick(7);
    REQUIRE(dev.tick_call_count == 2);
    REQUIRE(dev.total_tick_cycles == 11);
}

TEST_CASE("Device reset is called", "[device][fast]") {
    TestDevice dev;
    dev.reset();
    REQUIRE(dev.was_reset);
}
