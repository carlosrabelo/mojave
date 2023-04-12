#include "catch.hpp"
#include "helpers/guest_loader.hpp"
#include "devices/shared/memory.hpp"

TEST_CASE("Load Z80 halt guest binary", "[guests]") {
    Memory mem(0x1000);
    bool ok = loadGuest("tests/guests/z80/halt.bin", mem, 0x0000);
    REQUIRE(ok);
    REQUIRE(mem.read(0) == 0x76);  // HALT
    REQUIRE(mem.read(1) == 0xC3);  // JP
}

TEST_CASE("Load M6502 halt guest binary", "[guests]") {
    Memory mem(0x1000);
    bool ok = loadGuest("tests/guests/m6502/halt.bin", mem, 0x0600);
    REQUIRE(ok);
    REQUIRE(mem.read(0x0600) == 0x00);  // BRK
    REQUIRE(mem.read(0x0601) == 0x4C);  // JMP
}

TEST_CASE("Guest load at non-zero offset", "[guests]") {
    Memory mem(0x1000);
    bool ok = loadGuest("tests/guests/z80/halt.bin", mem, 0x0100);
    REQUIRE(ok);
    REQUIRE(mem.read(0x0100) == 0x76);
    REQUIRE(mem.read(0x00FF) == 0);
}

TEST_CASE("Guest load file not found returns false", "[guests]") {
    Memory mem(64);
    bool ok = loadGuest("nonexistent.bin", mem, 0);
    REQUIRE_FALSE(ok);
}
