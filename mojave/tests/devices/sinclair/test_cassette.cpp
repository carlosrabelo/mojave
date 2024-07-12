#include <cstdint>
#include "catch.hpp"
#include "devices/sinclair/cassette.hpp"

TEST_CASE("Sinclair cassette stub constants", "[device][sinclair][fast]") {
    REQUIRE(SinclairCassette::kBaud == 250);
    REQUIRE(SinclairCassette::kEarBit == 0x80);
    REQUIRE(SinclairCassette::kEarIdleMask == 0x80);
}

TEST_CASE("Sinclair cassette stub idle EAR and MIC levels", "[device][sinclair][fast]") {
    SinclairCassette cassette;
    cassette.reset();

    REQUIRE(cassette.micHigh());
    REQUIRE(cassette.earBits() == SinclairCassette::kEarIdleMask);

    cassette.setEarHigh(false);
    REQUIRE(cassette.earBits() == 0);

    cassette.onFeRead();
    REQUIRE_FALSE(cassette.micHigh());

    cassette.onAnyOut();
    REQUIRE(cassette.micHigh());
}
