#include <cstring>
#include "catch.hpp"
#include "devices/shared/memory.hpp"

TEST_CASE("Memory read/write roundtrip", "[memory][fast]") {
    Memory mem(64);
    mem.write(0, 0xAB);
    REQUIRE(mem.read(0) == 0xAB);
}

TEST_CASE("Memory out-of-bounds read returns 0", "[memory][fast]") {
    Memory mem(16);
    REQUIRE(mem.read(0xFF) == 0);
    REQUIRE(mem.read(16) == 0);
    REQUIRE(mem.read(0xFFFF) == 0);
}

TEST_CASE("Memory out-of-bounds write does not crash", "[memory][fast]") {
    Memory mem(16);
    REQUIRE_NOTHROW(mem.write(0xFF, 0xAA));
    REQUIRE_NOTHROW(mem.write(0xFFFF, 0xBB));
}

TEST_CASE("Memory load writes sequential bytes", "[memory][fast]") {
    Memory mem(32);
    uint8_t src[] = {0x10, 0x20, 0x30, 0x40};
    mem.load(8, src, 4);

    REQUIRE(mem.read(8) == 0x10);
    REQUIRE(mem.read(9) == 0x20);
    REQUIRE(mem.read(10) == 0x30);
    REQUIRE(mem.read(11) == 0x40);
    REQUIRE(mem.read(7) == 0);
    REQUIRE(mem.read(12) == 0);
}

TEST_CASE("Memory read-only blocks writes", "[memory][fast]") {
    Memory mem(16, true);
    REQUIRE(mem.isReadOnly());

    mem.write(0, 0xFF);
    REQUIRE(mem.read(0) == 0);
}

TEST_CASE("Memory load bypasses read-only", "[memory][fast]") {
    Memory mem(16, true);
    uint8_t src[] = {0xDE, 0xAD};
    mem.load(0, src, 2);

    REQUIRE(mem.read(0) == 0xDE);
    REQUIRE(mem.read(1) == 0xAD);
}

TEST_CASE("Memory load truncated at boundary", "[memory][fast]") {
    Memory mem(4);
    uint8_t src[] = {1, 2, 3, 4, 5, 6};
    mem.load(2, src, 6);

    REQUIRE(mem.read(2) == 1);
    REQUIRE(mem.read(3) == 2);
    REQUIRE(mem.read(4) == 0);
}
