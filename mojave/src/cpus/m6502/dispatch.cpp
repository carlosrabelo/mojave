#include "cpus/m6502/dispatch.hpp"
#include "cpus/m6502.hpp"

namespace m6502 {

#define OP_ROW(high) \
    &M6502::op##high##0, &M6502::op##high##1, &M6502::op##high##2, &M6502::op##high##3, \
    &M6502::op##high##4, &M6502::op##high##5, &M6502::op##high##6, &M6502::op##high##7, \
    &M6502::op##high##8, &M6502::op##high##9, &M6502::op##high##A, &M6502::op##high##B, \
    &M6502::op##high##C, &M6502::op##high##D, &M6502::op##high##E, &M6502::op##high##F

const OpcodeHandler kDispatch[256] = {
    OP_ROW(0), OP_ROW(1), OP_ROW(2), OP_ROW(3),
    OP_ROW(4), OP_ROW(5), OP_ROW(6), OP_ROW(7),
    OP_ROW(8), OP_ROW(9), OP_ROW(A), OP_ROW(B),
    OP_ROW(C), OP_ROW(D), OP_ROW(E), OP_ROW(F)
};

} // namespace m6502
