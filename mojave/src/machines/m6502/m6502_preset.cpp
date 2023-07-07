#include "machines/m6502/m6502_preset.hpp"
#include "machines/shared/machine.hpp"
#include "cpus/m6502.hpp"
#include "devices/shared/memory.hpp"
#include "devices/shared/virtual_tty.hpp"

std::unique_ptr<Machine> createM6502Machine() {
    auto cpu = std::make_unique<M6502>();
    auto machine = std::make_unique<Machine>(std::move(cpu));

    // 64 KiB space with VirtualTTY window at 0xFFE0-0xFFE1 (pForth / FIG layout).
    // ram1: 0x0000 .. 0xFFE0
    // tty:  0xFFE0 .. 0xFFE2
    // ram2: 0xFFE2 .. 0x10000
    auto ram1 = std::make_unique<Memory>(0xFFE0);
    auto ram2 = std::make_unique<Memory>(0x001E);
    machine->attachDevice(std::move(ram1), 0x0000, 0xFFE0);
    machine->attachDevice(std::move(ram2), 0xFFE2, 0x0000);

    auto tty = std::make_unique<VirtualTTY>();
    machine->attachDevice(std::move(tty), 0xFFE0, 0xFFE2);

    return machine;
}

bool isM6502PresetId(const std::string& id) {
    return id == "m6502";
}
