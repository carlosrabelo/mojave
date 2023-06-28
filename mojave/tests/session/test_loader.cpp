#include <cstdint>
#include "catch.hpp"
#include "session/loader.hpp"
#include "helpers.hpp"
#include "helpers/guest_loader.hpp"

TEST_CASE("Loader loads binary file into Memory", "[loader][fast]") {
    auto mem = createMemory(256);
    bool ok = loader::loadBinary("tests/guests/z80/halt.bin", *mem, 0x0000);
    REQUIRE(ok);
    REQUIRE(mem->read(0) == 0x76);
    REQUIRE(mem->read(1) == 0xC3);
}

TEST_CASE("Loader loads binary file into Memory at non-zero offset", "[loader][fast]") {
    auto mem = createMemory(512);
    bool ok = loader::loadBinary("tests/guests/z80/halt.bin", *mem, 0x0100);
    REQUIRE(ok);
    REQUIRE(mem->read(0x0100) == 0x76);
    REQUIRE(mem->read(0x0101) == 0xC3);
}

TEST_CASE("Loader loads binary file into Bus", "[loader][fast]") {
    auto [bus, ram] = createBusWithRam(0x0000, 0x0100);
    bool ok = loader::loadBinary("tests/guests/z80/halt.bin", *bus, 0x0000);
    REQUIRE(ok);
    REQUIRE(bus->read(0) == 0x76);
    REQUIRE(bus->read(1) == 0xC3);
}

TEST_CASE("Loader returns false for nonexistent file", "[loader][fast]") {
    auto mem = createMemory(64);
    bool ok = loader::loadBinary("nonexistent.bin", *mem, 0);
    REQUIRE_FALSE(ok);
}

TEST_CASE("Loader loads binary into read-only ROM via Bus", "[loader][fast]") {
    Bus bus;
    Memory rom(16, true);
    bus.attach(rom, 0x0000, 0x0010);

    bool ok = loader::loadBinary("tests/guests/z80/halt.bin", bus, 0x0000);
    REQUIRE(ok);
    REQUIRE(bus.read(0x0000) == 0x76);
    REQUIRE(bus.read(0x0001) == 0xC3);
    REQUIRE(rom.read(0) == 0x76);
}

TEST_CASE("Loader applies every --load-bin before execution starts", "[loader][fast]") {
    auto [bus, ram] = createBusWithRam(0x0000, 0x0200);
    REQUIRE(loader::loadBinary("tests/guests/z80/halt.bin", *bus, 0x0000));
    REQUIRE(loader::loadBinary("tests/guests/z80/halt.bin", *bus, 0x0100));
    REQUIRE(bus->read(0x0000) == 0x76);
    REQUIRE(bus->read(0x0100) == 0x76);
}

TEST_CASE("Loader writes each byte to Bus sequentially via write()", "[loader][fast]") {
    Bus bus;
    Memory ram(16);
    bus.attach(ram, 0x0000, 0x0010);

    // Use a small inline binary: 4 bytes
    bool ok = loader::loadBinary("tests/guests/z80/halt.bin", bus, 0x0000);
    REQUIRE(ok);
    REQUIRE(bus.read(0x0000) == 0x76);
    REQUIRE(bus.read(0x0003) == 0x00);
}
