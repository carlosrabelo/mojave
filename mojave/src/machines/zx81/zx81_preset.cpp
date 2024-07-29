#include "machines/zx81/zx81_preset.hpp"
#include "machines/shared/machine.hpp"
#include "cpus/z80.hpp"
#include "devices/shared/memory.hpp"

namespace {

using Contract = Zx81PresetContract;

} // namespace

std::unique_ptr<Machine> createZx81Machine() {
    auto cpu = std::make_unique<Z80>();
    auto machine = std::make_unique<Machine>(std::move(cpu));
    machine->setGuestCpuClockHz(Contract::guest_cpu_clock_hz);

    auto rom = std::make_unique<Memory>(Contract::rom_end_exclusive - Contract::rom_start, true);
    machine->attachDevice(std::move(rom), Contract::rom_start, Contract::rom_end_exclusive);

    auto ram = std::make_unique<Memory>(Contract::ram_end_exclusive - Contract::ram_start);
    machine->attachDevice(std::move(ram), Contract::ram_start, Contract::ram_end_exclusive);

    return machine;
}

bool isZx81PresetId(const std::string& id) {
    return id == "zx81";
}
