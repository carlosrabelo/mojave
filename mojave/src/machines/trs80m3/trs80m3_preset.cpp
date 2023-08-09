#include "machines/trs80m3/trs80m3_preset.hpp"
#include "machines/shared/machine.hpp"
#include "cpus/z80.hpp"
#include "devices/shared/memory.hpp"

namespace {

using Contract = Trs80M3PresetContract;

} // namespace

std::unique_ptr<Machine> createTrs80M3Machine() {
    auto cpu = std::make_unique<Z80>();
    auto machine = std::make_unique<Machine>(std::move(cpu));
    machine->setGuestCpuClockHz(Contract::guest_cpu_clock_hz);

    auto rom_low = std::make_unique<Memory>(Contract::io_latch_start - Contract::rom_start, true);
    machine->attachDevice(std::move(rom_low), Contract::rom_start, Contract::io_latch_start);

    auto rom_tail =
        std::make_unique<Memory>(Contract::rom_end_exclusive - Contract::rom_tail_start, true);
    machine->attachDevice(std::move(rom_tail), Contract::rom_tail_start, Contract::rom_end_exclusive);

    auto ram = std::make_unique<Memory>(Contract::ram_end_exclusive - Contract::ram_start);
    machine->attachDevice(std::move(ram), Contract::ram_start, Contract::ram_bus_end);

    return machine;
}

bool isTrs80M3PresetId(const std::string& id) {
    return id == "trs80m3";
}
