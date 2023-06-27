#include "session/runner.hpp"
#include "machines/shared/machine.hpp"

namespace runner {

void boot(Machine& machine) {
    machine.reset();
}

unsigned stepForFrame(Machine& machine, const Clock& clock, uint64_t frameHz) {
    const uint64_t budget = clock.cyclesPerFrame(frameHz);
    if (budget == 0)
        return 0;

    // Always burn the frame budget. ZX-80/81 use HALT inside the display path;
    // stopping early would freeze the ULA INT/NMI cascade.
    unsigned cycles = 0;
    while (cycles < budget)
        cycles += machine.step();
    return cycles;
}

void stepAndPaceFrame(Machine& machine, Clock& clock, uint64_t frameHz) {
    const unsigned cycles = stepForFrame(machine, clock, frameHz);
    clock.pace(cycles);
}

} // namespace runner
