#include "catch.hpp"
#include "devices/trs80m3/rtc_timer.hpp"
#include "devices/trs80m3/port_decode.hpp"
#include "cpus/z80.hpp"

TEST_CASE("TRS-80 Model III RTC timer fires every 67584 guest cycles", "[devices][trs80m3][fast]") {
    Trs80M3PortDecode ports;
    Z80 cpu;
    ports.bindCpu(&cpu);
    Trs80M3RtcTimer timer(ports, cpu, 2'027'520);

    REQUIRE(timer.cyclesPerInterrupt() == 67584u);

    timer.tick(67583);
    REQUIRE(ports.interruptStatus() == 0xFF);
    REQUIRE_FALSE(cpu.regs().iff1);

    timer.tick(1);
    REQUIRE(ports.interruptStatus() ==
            static_cast<uint8_t>(~Trs80M3PortDecode::kRtcInterruptMask));
}

TEST_CASE("TRS-80 Model III RTC timer asserts INT when RTC is masked in", "[devices][trs80m3][fast]") {
    Trs80M3PortDecode ports;
    Z80 cpu;
    ports.bindCpu(&cpu);
    Trs80M3RtcTimer timer(ports, cpu, 2'027'520);

    ports.writePort(0xE0, Trs80M3PortDecode::kRtcInterruptMask);
    timer.tick(67584);
    REQUIRE(ports.maskableInterruptActive());

    ports.writePort(0xE0, 0x00);
    REQUIRE_FALSE(ports.maskableInterruptActive());
}

TEST_CASE("TRS-80 Model III RTC timer reset clears cycle accumulator", "[devices][trs80m3][fast]") {
    Trs80M3PortDecode ports;
    Z80 cpu;
    Trs80M3RtcTimer timer(ports, cpu, 2'027'520);

    timer.tick(1000);
    timer.reset();
    REQUIRE(timer.cycleAccumulator() == 0);
}
