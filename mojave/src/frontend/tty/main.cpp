#include <cstdio>
#include <memory>
#include <string>
#include "session/cli.hpp"
#include "session/config.hpp"
#include "session/session.hpp"
#include "session/loader.hpp"
#include "machines/shared/machine.hpp"
#include "machines/z80/z80_preset.hpp"
#include "machines/m6502/m6502_preset.hpp"
#include "devices/shared/virtual_tty.hpp"

int main(int argc, char* argv[]) {
    const MojaveConfig config = loadMojaveConfig();
    CLIOptions opts = parseCLI(argc, argv, config);
    if (!opts.ok) {
        std::fprintf(stderr, "Error: %s\n", opts.error.c_str());
        return 1;
    }
    if (opts.help) {
        return 0;
    }

    std::unique_ptr<Machine> machine;
    if (opts.machine == "z80") {
        machine = createZ80Machine();
    } else if (opts.machine == "m6502") {
        machine = createM6502Machine();
    } else {
        std::fprintf(stderr, "Error: machine preset '%s' not supported or implemented yet.\n",
                     opts.machine.c_str());
        return 1;
    }

    VirtualTTY* tty = nullptr;
    for (const auto& dev : machine->ownedDevices()) {
        if (auto t = dynamic_cast<VirtualTTY*>(dev.get())) {
            tty = t;
            break;
        }
    }
    if (!tty) {
        std::fprintf(stderr, "Error: machine preset '%s' does not have a VirtualTTY device.\n",
                     opts.machine.c_str());
        return 1;
    }

    for (const auto& load : opts.loads) {
        if (!loader::loadBinary(load.path.c_str(), machine->bus(), load.address)) {
            std::fprintf(stderr, "Error: failed to load binary file '%s' at address 0x%04X\n",
                         load.path.c_str(), load.address);
            return 1;
        }
    }

    Session session(*machine, *tty);
    session.runUntilHalt();

    if (opts.dumpMem) {
        for (uint32_t addr = opts.memStart; addr <= opts.memEnd; ++addr) {
            if ((addr - opts.memStart) % 16 == 0) {
                std::printf("%04X: ", addr);
            }
            std::printf("%02X", machine->bus().read(addr));
            if ((addr - opts.memStart) % 16 == 15 || addr == opts.memEnd) {
                std::printf("\n");
            } else {
                std::printf(" ");
            }
        }
    }

    if (opts.dumpReg) {
        auto snap = machine->cpu().registers();
        for (const auto& entry : snap.entries) {
            std::printf("%s: %04X\n", entry.name.c_str(), entry.value);
        }
    }

    return 0;
}
