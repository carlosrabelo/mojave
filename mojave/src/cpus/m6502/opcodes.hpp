#ifndef MOJAVE_M6502_OPCODES_HPP
#define MOJAVE_M6502_OPCODES_HPP

#include <cstdint>

class M6502;

namespace m6502 {

using OpcodeHandler = unsigned (M6502::*)();

} // namespace m6502

#endif
