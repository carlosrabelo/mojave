#include "machines/m6507/m6507_preset.hpp"
#include "machines/shared/machine.hpp"
#include "cpus/m6507.hpp"
#include "devices/shared/memory.hpp"
#include "devices/shared/virtual_tty.hpp"

std::unique_ptr<Machine> createM6507Machine() {
    auto cpu = std::make_unique<M6507>();
    auto machine = std::make_unique<Machine>(std::move(cpu));

    // M6507 has 8 KiB addressable space. Map RAM around TTY at 0x1E00-0x1E02:
    // ram1: 0x0000 to 0x1E00 (size 0x1E00 = 7680 bytes)
    // ram2: 0x1E02 to 0x2000 (size 0x01FE = 510 bytes)
    auto ram1 = std::make_unique<Memory>(0x1E00);
    auto ram2 = std::make_unique<Memory>(0x01FE);
    machine->attachDevice(std::move(ram1), 0x0000, 0x1E00);
    machine->attachDevice(std::move(ram2), 0x1E02, 0x2000);

    // Memory-mapped VirtualTTY at 0x1E00-0x1E02 (corresponds to physical 0xFE00 under 13-bit mask)
    auto tty = std::make_unique<VirtualTTY>();
    machine->attachDevice(std::move(tty), 0x1E00, 0x1E02);

    return machine;
}

bool isM6507PresetId(const std::string& id) {
    return id == "m6507";
}
