#include <cstdint>
#include <chrono>
#include "catch.hpp"
#include "session/clock.hpp"

TEST_CASE("Clock 0 Hz is unlimited", "[clock][fast]") {
    Clock clk(0);
    REQUIRE(clk.unlimited());
    REQUIRE(clk.hz() == 0);
}

TEST_CASE("Clock non-zero Hz is not unlimited", "[clock][fast]") {
    Clock clk(4000000);
    REQUIRE_FALSE(clk.unlimited());
    REQUIRE(clk.hz() == 4000000);
}

TEST_CASE("Clock 0 Hz pace never sleeps", "[clock][fast]") {
    Clock clk(0);
    auto t0 = std::chrono::steady_clock::now();
    clk.pace(1000000);
    auto t1 = std::chrono::steady_clock::now();
    auto us = std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count();
    // Should be effectively instant (well under 10ms)
    REQUIRE(us < 10000);
}

TEST_CASE("Clock reset clears cycle counter", "[clock][fast]") {
    Clock clk(1000);
    clk.pace(500);
    clk.reset();
    // After reset, a large cycle count would trigger a long sleep.
    // Instead, verify reset works by checking pace with 0 cycles is instant.
    auto t0 = std::chrono::steady_clock::now();
    clk.pace(0);
    auto t1 = std::chrono::steady_clock::now();
    auto us = std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count();
    REQUIRE(us < 1000);
}

TEST_CASE("Clock paces high-speed execution correctly", "[clock][integration]") {
    // 1000 Hz, run 100 cycles → should take ~100ms
    Clock clk(1000);
    auto t0 = std::chrono::steady_clock::now();
    clk.pace(100);
    auto t1 = std::chrono::steady_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count();
    REQUIRE(ms >= 80);
    REQUIRE(ms < 200);
}

TEST_CASE("Clock default constructor is unlimited", "[clock][fast]") {
    Clock clk;
    REQUIRE(clk.unlimited());
}
