#include <cstdint>
#include "catch.hpp"
#include "bus/bus.hpp"
#include "devices/shared/memory.hpp"
#include "devices/shared/port_device.hpp"

namespace {

class TestPortDevice : public PortDevice {
public:
    uint16_t last_read_port = 0;
    uint16_t last_write_port = 0;
    uint8_t last_write_val = 0;

    uint8_t readPort(uint16_t port) override {
        last_read_port = port;
        return 0xCC;
    }

    void writePort(uint16_t port, uint8_t value) override {
        last_write_port = port;
        last_write_val = value;
    }
};

}

TEST_CASE("Bus port read/write reaches device", "[bus][fast][port]") {
    Bus bus;
    TestPortDevice dev;

    bus.attachPort(dev, 0x00, 0x10);
    bus.writePort(0x03, 0xAA);
    REQUIRE(dev.last_write_port == 0x03);
    REQUIRE(dev.last_write_val == 0xAA);

    uint8_t val = bus.readPort(0x07);
    REQUIRE(val == 0xCC);
    REQUIRE(dev.last_read_port == 0x07);
}

TEST_CASE("Bus unmapped port returns floating bus 0xFF", "[bus][fast][port]") {
    Bus bus;
    TestPortDevice dev;

    bus.attachPort(dev, 0x00, 0x10);
    REQUIRE(bus.readPort(0x10) == 0xFF);
    REQUIRE(bus.readPort(0xFF) == 0xFF);
}

TEST_CASE("Bus overlapping port attach throws", "[bus][fast][port]") {
    Bus bus;
    TestPortDevice dev;

    bus.attachPort(dev, 0x00, 0x10);
    REQUIRE_THROWS(bus.attachPort(dev, 0x08, 0x18));
    REQUIRE_THROWS(bus.attachPort(dev, 0x00, 0x10));
}

TEST_CASE("Bus memory and port spaces are independent", "[bus][fast][port]") {
    Bus bus;
    Memory mem(16);
    TestPortDevice port_dev;

    bus.attach(mem, 0x00, 0x10);
    bus.attachPort(port_dev, 0x00, 0x10);

    bus.write(0x05, 0x11);
    bus.writePort(0x05, 0x22);

    REQUIRE(mem.read(5) == 0x11);
    REQUIRE(port_dev.last_write_port == 0x05);
    REQUIRE(port_dev.last_write_val == 0x22);
}

TEST_CASE("Bus low-byte port alias reaches device for high-byte Z80 addresses", "[bus][fast][port]") {
    Bus bus;
    TestPortDevice dev;

    bus.attachPort(dev, 0xF0, 0xF5, true);

    bus.writePort(0xC1F4, 0xAA);
    REQUIRE(dev.last_write_port == 0x04);
    REQUIRE(dev.last_write_val == 0xAA);

    bus.writePort(0x00F3, 0x55);
    REQUIRE(dev.last_write_port == 0x03);
    REQUIRE(dev.last_write_val == 0x55);

    REQUIRE(bus.readPort(0x08FF) == 0xFF);
}
