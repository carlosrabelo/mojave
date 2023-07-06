#include "machines/z80/z80_preset.hpp"
#include "machines/shared/machine.hpp"
#include "cpus/z80.hpp"
#include "devices/shared/memory.hpp"
#include "devices/shared/virtual_tty.hpp"

std::unique_ptr<Machine> createZ80Machine() {
    auto cpu = std::make_unique<Z80>();
    auto machine = std::make_unique<Machine>(std::move(cpu));

    // Attach 64 KiB RAM using two 32 KiB memory blocks to avoid size overflow
    auto ram1 = std::make_unique<Memory>(0x8000); // 32 KiB
    auto ram2 = std::make_unique<Memory>(0x8000); // 32 KiB
    machine->attachDevice(std::move(ram1), 0x0000, 0x8000);
    machine->attachDevice(std::move(ram2), 0x8000, 0x0000); // end_exclusive = 0 (covers up to 65536 / 0xFFFF)

    // Attach VirtualTTY to Port I/O space at ports 0x0000 - 0x0002
    auto tty = std::make_unique<VirtualTTY>();
    machine->bus().attachPort(*tty, 0x0000, 0x0002);

    // Maintain ownership of the tty device inside the machine
    machine->addOwnedDevice(std::move(tty));

    return machine;
}

bool isZ80PresetId(const std::string& id) {
    return id == "z80";
}
