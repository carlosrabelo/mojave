#include "catch.hpp"
#include "bus/bus.hpp"
#include "devices/shared/memory.hpp"

TEST_CASE("Bus routes read to correct device", "[bus][fast]") {
    Bus bus;
    Memory ram_a(16);
    Memory ram_b(16);

    bus.attach(ram_a, 0x0000, 0x0010);
    bus.attach(ram_b, 0x0010, 0x0020);

    ram_a.write(0, 0xAA);
    ram_b.write(0, 0xBB);

    REQUIRE(bus.read(0x0000) == 0xAA);
    REQUIRE(bus.read(0x0010) == 0xBB);
}

TEST_CASE("Bus routes write to correct device", "[bus][fast]") {
    Bus bus;
    Memory ram_a(16);
    Memory ram_b(16);

    bus.attach(ram_a, 0x0000, 0x0010);
    bus.attach(ram_b, 0x0010, 0x0020);

    bus.write(0x0005, 0x11);
    bus.write(0x0015, 0x22);

    REQUIRE(ram_a.read(5) == 0x11);
    REQUIRE(ram_b.read(5) == 0x22);
}

TEST_CASE("Bus overlapping attach throws", "[bus][fast]") {
    Bus bus;
    Memory ram(16);

    bus.attach(ram, 0x0000, 0x0010);
    REQUIRE_THROWS(bus.attach(ram, 0x0008, 0x0018));  // overlaps end
    REQUIRE_THROWS(bus.attach(ram, 0x0000, 0x0010));  // exact duplicate
    REQUIRE_THROWS(bus.attach(ram, 0x0000, 0x0020));  // contains first
}

TEST_CASE("Bus unmapped address returns floating bus 0xFF", "[bus][fast]") {
    Bus bus;
    Memory ram(16);
    bus.attach(ram, 0x0000, 0x0010);

    REQUIRE(bus.read(0x0010) == 0xFF);
    REQUIRE(bus.read(0xFFFF) == 0xFF);
}

TEST_CASE("Bus unmapped write does nothing", "[bus][fast]") {
    Bus bus;
    Memory ram(16);
    bus.attach(ram, 0x0000, 0x0010);

    REQUIRE_NOTHROW(bus.write(0xFFFF, 0xFF));
}

TEST_CASE("Bus device at non-zero start offset", "[bus][fast]") {
    Bus bus;
    Memory ram(32);
    bus.attach(ram, 0x0100, 0x0120);

    bus.write(0x0100, 0xCA);
    bus.write(0x011F, 0xFE);

    REQUIRE(bus.read(0x0100) == 0xCA);
    REQUIRE(bus.read(0x011F) == 0xFE);
    REQUIRE(ram.read(0) == 0xCA);
    REQUIRE(ram.read(0x1F) == 0xFE);
}
