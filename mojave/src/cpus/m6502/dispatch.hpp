#ifndef MOJAVE_M6502_DISPATCH_HPP
#define MOJAVE_M6502_DISPATCH_HPP

#include "cpus/m6502/opcodes.hpp"

namespace m6502 {

extern const OpcodeHandler kDispatch[256];

} // namespace m6502

#endif
