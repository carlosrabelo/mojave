#ifndef MOJAVE_RUNNER_HPP
#define MOJAVE_RUNNER_HPP

#include <cstdint>
#include "session/clock.hpp"

class Machine;

namespace runner {

void boot(Machine& machine);

unsigned stepForFrame(Machine& machine, const Clock& clock,
                      uint64_t frameHz = Clock::kDefaultFrameHz);

void stepAndPaceFrame(Machine& machine, Clock& clock,
                      uint64_t frameHz = Clock::kDefaultFrameHz);

} // namespace runner

#endif
