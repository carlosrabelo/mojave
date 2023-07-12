#include "machines/trs80m1l1/trs80m1l1_preset.hpp"
#include "machines/shared/machine.hpp"
#include "cpus/z80.hpp"
#include "devices/shared/memory.hpp"

namespace {

using Contract = Trs80M1L1PresetContract;

} // namespace

std::unique_ptr<Machine> createTrs80M1L1Machine() {
    auto cpu = std::make_unique<Z80>();
    auto machine = std::make_unique<Machine>(std::move(cpu));

    auto rom = std::make_unique<Memory>(Contract::rom_end_exclusive - Contract::rom_start, true);
    machine->attachDevice(std::move(rom), Contract::rom_start, Contract::rom_end_exclusive);

    auto ram = std::make_unique<Memory>(Contract::ram_end_exclusive - Contract::ram_start);
    machine->attachDevice(std::move(ram), Contract::ram_start, Contract::ram_end_exclusive);

    return machine;
}

bool isTrs80M1L1PresetId(const std::string& id) {
    return id == "trs80m1l1";
}
