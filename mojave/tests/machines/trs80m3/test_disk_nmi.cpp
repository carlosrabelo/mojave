#include "catch.hpp"
#include "bus/bus.hpp"
#include "cpus/z80.hpp"
#include "devices/trs80m3/floppy_controller.hpp"
#include "devices/trs80m3/port_decode.hpp"

TEST_CASE("TRS-80 Model III floppy drive select latches without auto NMI", "[machine][trs80m3][fast]") {
    Trs80M3PortDecode ports;
    Trs80M3FloppyController floppy(ports);
    Z80 cpu;
    ports.bindCpu(&cpu);
    floppy.bindCpu(&cpu);

    Bus bus;
    bus.attachPort(floppy, Trs80M3FloppyController::kPortStart,
                   Trs80M3FloppyController::kPortEndExclusive, true);

    cpu.regs().pc = 0x34FB;
    cpu.setBus(&bus);

    bus.writePort(0xC1F4, 0xC1);
    REQUIRE(floppy.driveSelect() == 0xC1);
    REQUIRE(ports.readPort(Trs80M3PortDecode::kNmiOffset) == Trs80M3PortDecode::kNmiIdleStatus);
}

TEST_CASE("TRS-80 Model III floppy can request disk index NMI explicitly", "[machine][trs80m3][fast]") {
    Trs80M3PortDecode ports;
    Trs80M3FloppyController floppy(ports);
    Z80 cpu;
    ports.bindCpu(&cpu);
    floppy.bindCpu(&cpu);

    Bus bus;
    bus.attachPort(floppy, Trs80M3FloppyController::kPortStart,
                   Trs80M3FloppyController::kPortEndExclusive, true);

    cpu.regs().pc = 0x34FB;
    cpu.setBus(&bus);

    floppy.requestDiskIndexNmi();
    cpu.step();

    REQUIRE(cpu.regs().pc == 0x0066);
    REQUIRE(ports.readPort(Trs80M3PortDecode::kNmiOffset) == Trs80M3PortDecode::kDiskIndexStatus);
}
