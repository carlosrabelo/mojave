#include "cpus/z80.hpp"
#include "cpus/z80/dispatch.hpp"
#include "bus/bus.hpp"

namespace z80 {

const OpcodeHandler kDispatchED[256] = {
    // 0x00 - 0x0F
    &Z80::opED00, &Z80::opED01, &Z80::opED02, &Z80::opED03,
    &Z80::opED04, &Z80::opED05, &Z80::opED06, &Z80::opED07,
    &Z80::opED08, &Z80::opED09, &Z80::opED0A, &Z80::opED0B,
    &Z80::opED0C, &Z80::opED0D, &Z80::opED0E, &Z80::opED0F,

    // 0x10 - 0x1F
    &Z80::opED10, &Z80::opED11, &Z80::opED12, &Z80::opED13,
    &Z80::opED14, &Z80::opED15, &Z80::opED16, &Z80::opED17,
    &Z80::opED18, &Z80::opED19, &Z80::opED1A, &Z80::opED1B,
    &Z80::opED1C, &Z80::opED1D, &Z80::opED1E, &Z80::opED1F,

    // 0x20 - 0x2F
    &Z80::opED20, &Z80::opED21, &Z80::opED22, &Z80::opED23,
    &Z80::opED24, &Z80::opED25, &Z80::opED26, &Z80::opED27,
    &Z80::opED28, &Z80::opED29, &Z80::opED2A, &Z80::opED2B,
    &Z80::opED2C, &Z80::opED2D, &Z80::opED2E, &Z80::opED2F,

    // 0x30 - 0x3F
    &Z80::opED30, &Z80::opED31, &Z80::opED32, &Z80::opED33,
    &Z80::opED34, &Z80::opED35, &Z80::opED36, &Z80::opED37,
    &Z80::opED38, &Z80::opED39, &Z80::opED3A, &Z80::opED3B,
    &Z80::opED3C, &Z80::opED3D, &Z80::opED3E, &Z80::opED3F,

    // 0x40 - 0x4F
    &Z80::opED40, &Z80::opED41, &Z80::opED42, &Z80::opED43,
    &Z80::opED44, &Z80::opED45, &Z80::opED46, &Z80::opED47,
    &Z80::opED48, &Z80::opED49, &Z80::opED4A, &Z80::opED4B,
    &Z80::opED4C, &Z80::opED4D, &Z80::opED4E, &Z80::opED4F,

    // 0x50 - 0x5F
    &Z80::opED50, &Z80::opED51, &Z80::opED52, &Z80::opED53,
    &Z80::opED54, &Z80::opED55, &Z80::opED56, &Z80::opED57,
    &Z80::opED58, &Z80::opED59, &Z80::opED5A, &Z80::opED5B,
    &Z80::opED5C, &Z80::opED5D, &Z80::opED5E, &Z80::opED5F,

    // 0x60 - 0x6F
    &Z80::opED60, &Z80::opED61, &Z80::opED62, &Z80::opED63,
    &Z80::opED64, &Z80::opED65, &Z80::opED66, &Z80::opED67,
    &Z80::opED68, &Z80::opED69, &Z80::opED6A, &Z80::opED6B,
    &Z80::opED6C, &Z80::opED6D, &Z80::opED6E, &Z80::opED6F,

    // 0x70 - 0x7F
    &Z80::opED70, &Z80::opED71, &Z80::opED72, &Z80::opED73,
    &Z80::opED74, &Z80::opED75, &Z80::opED76, &Z80::opED77,
    &Z80::opED78, &Z80::opED79, &Z80::opED7A, &Z80::opED7B,
    &Z80::opED7C, &Z80::opED7D, &Z80::opED7E, &Z80::opED7F,

    // 0x80 - 0x8F
    &Z80::opED80, &Z80::opED81, &Z80::opED82, &Z80::opED83,
    &Z80::opED84, &Z80::opED85, &Z80::opED86, &Z80::opED87,
    &Z80::opED88, &Z80::opED89, &Z80::opED8A, &Z80::opED8B,
    &Z80::opED8C, &Z80::opED8D, &Z80::opED8E, &Z80::opED8F,

    // 0x90 - 0x9F
    &Z80::opED90, &Z80::opED91, &Z80::opED92, &Z80::opED93,
    &Z80::opED94, &Z80::opED95, &Z80::opED96, &Z80::opED97,
    &Z80::opED98, &Z80::opED99, &Z80::opED9A, &Z80::opED9B,
    &Z80::opED9C, &Z80::opED9D, &Z80::opED9E, &Z80::opED9F,

    // 0xA0 - 0xAF
    &Z80::opEDA0, &Z80::opEDA1, &Z80::opEDA2, &Z80::opEDA3,
    &Z80::opEDA4, &Z80::opEDA5, &Z80::opEDA6, &Z80::opEDA7,
    &Z80::opEDA8, &Z80::opEDA9, &Z80::opEDAA, &Z80::opEDAB,
    &Z80::opEDAC, &Z80::opEDAD, &Z80::opEDAE, &Z80::opEDAF,

    // 0xB0 - 0xBF
    &Z80::opEDB0, &Z80::opEDB1, &Z80::opEDB2, &Z80::opEDB3,
    &Z80::opEDB4, &Z80::opEDB5, &Z80::opEDB6, &Z80::opEDB7,
    &Z80::opEDB8, &Z80::opEDB9, &Z80::opEDBA, &Z80::opEDBB,
    &Z80::opEDBC, &Z80::opEDBD, &Z80::opEDBE, &Z80::opEDBF,

    // 0xC0 - 0xCF
    &Z80::opEDC0, &Z80::opEDC1, &Z80::opEDC2, &Z80::opEDC3,
    &Z80::opEDC4, &Z80::opEDC5, &Z80::opEDC6, &Z80::opEDC7,
    &Z80::opEDC8, &Z80::opEDC9, &Z80::opEDCA, &Z80::opEDCB,
    &Z80::opEDCC, &Z80::opEDCD, &Z80::opEDCE, &Z80::opEDCF,

    // 0xD0 - 0xDF
    &Z80::opEDD0, &Z80::opEDD1, &Z80::opEDD2, &Z80::opEDD3,
    &Z80::opEDD4, &Z80::opEDD5, &Z80::opEDD6, &Z80::opEDD7,
    &Z80::opEDD8, &Z80::opEDD9, &Z80::opEDDA, &Z80::opEDDB,
    &Z80::opEDDC, &Z80::opEDDD, &Z80::opEDDE, &Z80::opEDDF,

    // 0xE0 - 0xEF
    &Z80::opEDE0, &Z80::opEDE1, &Z80::opEDE2, &Z80::opEDE3,
    &Z80::opEDE4, &Z80::opEDE5, &Z80::opEDE6, &Z80::opEDE7,
    &Z80::opEDE8, &Z80::opEDE9, &Z80::opEDEA, &Z80::opEDEB,
    &Z80::opEDEC, &Z80::opEDED, &Z80::opEDEE, &Z80::opEDEF,

    // 0xF0 - 0xFF
    &Z80::opEDF0, &Z80::opEDF1, &Z80::opEDF2, &Z80::opEDF3,
    &Z80::opEDF4, &Z80::opEDF5, &Z80::opEDF6, &Z80::opEDF7,
    &Z80::opEDF8, &Z80::opEDF9, &Z80::opEDFA, &Z80::opEDFB,
    &Z80::opEDFC, &Z80::opEDFD, &Z80::opEDFE, &Z80::opEDFF
};

} // namespace z80

unsigned Z80::executeED(uint8_t op) {
    switch (op) {
        // IN r, (C) - 12 cycles
        case 0x40: case 0x48: case 0x50: case 0x58:
        case 0x60: case 0x68: case 0x70: case 0x78: {
            uint8_t val = bus_ ? bus_->readPort(regs_.bc) : 0;
            setFlagS((val & 0x80) != 0);
            setFlagZ(val == 0);
            setFlagH(false);
            setFlagPV(parity(val));
            setFlagN(false);
            if (op == 0x40) setB(val);
            else if (op == 0x48) setC(val);
            else if (op == 0x50) setD(val);
            else if (op == 0x58) setE(val);
            else if (op == 0x60) setH(val);
            else if (op == 0x68) setL(val);
            else if (op == 0x78) setA(val);
            return 12;
        }

        // OUT (C), r - 12 cycles
        case 0x41: if (bus_) bus_->writePort(regs_.bc, getB()); return 12;
        case 0x49: if (bus_) bus_->writePort(regs_.bc, getC()); return 12;
        case 0x51: if (bus_) bus_->writePort(regs_.bc, getD()); return 12;
        case 0x59: if (bus_) bus_->writePort(regs_.bc, getE()); return 12;
        case 0x61: if (bus_) bus_->writePort(regs_.bc, getH()); return 12;
        case 0x69: if (bus_) bus_->writePort(regs_.bc, getL()); return 12;
        case 0x71: if (bus_) bus_->writePort(regs_.bc, 0); return 12;
        case 0x79: if (bus_) bus_->writePort(regs_.bc, getA()); return 12;

        // SBC HL, ss - 15 cycles
        case 0x42: edSBC_HL(regs_.bc); return 15;
        case 0x52: edSBC_HL(regs_.de); return 15;
        case 0x62: edSBC_HL(regs_.hl); return 15;
        case 0x72: edSBC_HL(regs_.sp); return 15;

        // ADC HL, ss - 15 cycles
        case 0x4A: edADC_HL(regs_.bc); return 15;
        case 0x5A: edADC_HL(regs_.de); return 15;
        case 0x6A: edADC_HL(regs_.hl); return 15;
        case 0x7A: edADC_HL(regs_.sp); return 15;

        // LD (nn), dd - 20 cycles
        case 0x43: {
            uint16_t addr = fetchWord();
            writeByte(addr, regs_.bc & 0xFF);
            writeByte(addr + 1, regs_.bc >> 8);
            return 20;
        }
        case 0x53: {
            uint16_t addr = fetchWord();
            writeByte(addr, regs_.de & 0xFF);
            writeByte(addr + 1, regs_.de >> 8);
            return 20;
        }
        case 0x63: {
            uint16_t addr = fetchWord();
            writeByte(addr, regs_.hl & 0xFF);
            writeByte(addr + 1, regs_.hl >> 8);
            return 20;
        }
        case 0x73: {
            uint16_t addr = fetchWord();
            writeByte(addr, regs_.sp & 0xFF);
            writeByte(addr + 1, regs_.sp >> 8);
            return 20;
        }

        // LD dd, (nn) - 20 cycles
        case 0x4B: {
            uint16_t addr = fetchWord();
            regs_.bc = readByte(addr) | (readByte(addr + 1) << 8);
            return 20;
        }
        case 0x5B: {
            uint16_t addr = fetchWord();
            regs_.de = readByte(addr) | (readByte(addr + 1) << 8);
            return 20;
        }
        case 0x6B: {
            uint16_t addr = fetchWord();
            regs_.hl = readByte(addr) | (readByte(addr + 1) << 8);
            return 20;
        }
        case 0x7B: {
            uint16_t addr = fetchWord();
            regs_.sp = readByte(addr) | (readByte(addr + 1) << 8);
            return 20;
        }

        // NEG - 8 cycles
        case 0x44: case 0x54: case 0x64: case 0x74:
        case 0x4C: case 0x5C: case 0x6C: case 0x7C:
            edNEG();
            return 8;

        // RETN / RETI - 14 cycles
        case 0x45: case 0x55: case 0x65: case 0x75:
        case 0x4D: case 0x5D: case 0x6D: case 0x7D:
            return edRETN();

        // IM - 8 cycles
        case 0x46: case 0x4E: regs_.im = 0; return 8;
        case 0x56: case 0x5E: regs_.im = 1; return 8;
        case 0x66: case 0x6E: regs_.im = 2; return 8;

        // LD I, A (47) and LD R, A (4F) - 9 cycles
        case 0x47: regs_.i = getA(); return 9;
        case 0x4F: regs_.r = getA(); return 9;

        // LD A, I (57) and LD A, R (5F) - 9 cycles
        case 0x57: {
            setA(regs_.i);
            setFlagS((regs_.i & 0x80) != 0);
            setFlagZ(regs_.i == 0);
            setFlagH(false);
            setFlagPV(regs_.iff2);
            setFlagN(false);
            setF35(regs_.i);
            return 9;
        }
        case 0x5F: {
            setA(regs_.r);
            setFlagS((regs_.r & 0x80) != 0);
            setFlagZ(regs_.r == 0);
            setFlagH(false);
            setFlagPV(regs_.iff2);
            setFlagN(false);
            setF35(regs_.r);
            return 9;
        }

        // RRD (67)
        case 0x67: {
            uint8_t a = getA();
            uint8_t mem = readByte(regs_.hl);
            uint8_t new_a = (a & 0xF0) | (mem & 0x0F);
            uint8_t new_mem = ((a & 0x0F) << 4) | (mem >> 4);
            setA(new_a);
            writeByte(regs_.hl, new_mem);
            setFlagS((new_a & 0x80) != 0);
            setFlagZ(new_a == 0);
            setFlagH(false);
            setFlagPV(parity(new_a));
            setFlagN(false);
            setF35(new_a);
            return 18;
        }
        // RLD (6F)
        case 0x6F: {
            uint8_t a = getA();
            uint8_t mem = readByte(regs_.hl);
            uint8_t new_a = (a & 0xF0) | (mem >> 4);
            uint8_t new_mem = ((mem & 0x0F) << 4) | (a & 0x0F);
            setA(new_a);
            writeByte(regs_.hl, new_mem);
            setFlagS((new_a & 0x80) != 0);
            setFlagZ(new_a == 0);
            setFlagH(false);
            setFlagPV(parity(new_a));
            setFlagN(false);
            setF35(new_a);
            return 18;
        }

        // Block transfers - 16 cycles
        case 0xA0: edLDI(); return 16;
        case 0xA8: edLDD(); return 16;
        case 0xB0: { // LDIR
            edLDI();
            if (regs_.bc != 0) {
                regs_.pc -= 2;
                return 21;
            }
            return 16;
        }
        case 0xB8: { // LDDR
            edLDD();
            if (regs_.bc != 0) {
                regs_.pc -= 2;
                return 21;
            }
            return 16;
        }

        // Block compares - 16 cycles
        case 0xA1: edCPI(); return 16;
        case 0xA9: edCPD(); return 16;
        case 0xB1: { // CPIR
            edCPI();
            if (regs_.bc != 0 && !getFlagZ()) {
                regs_.pc -= 2;
                return 21;
            }
            return 16;
        }
        case 0xB9: { // CPDR
            edCPD();
            if (regs_.bc != 0 && !getFlagZ()) {
                regs_.pc -= 2;
                return 21;
            }
            return 16;
        }

        // Block inputs - 16 cycles
        case 0xA2: edINI(); return 16;
        case 0xAA: edIND(); return 16;
        case 0xB2: { // INIR
            edINI();
            if (getB() != 0) {
                regs_.pc -= 2;
                return 21;
            }
            return 16;
        }
        case 0xBA: { // INDR
            edIND();
            if (getB() != 0) {
                regs_.pc -= 2;
                return 21;
            }
            return 16;
        }

        // Block outputs - 16 cycles
        case 0xA3: edOUTI(); return 16;
        case 0xAB: edOUTD(); return 16;
        case 0xB3: { // OTIR
            edOUTI();
            if (getB() != 0) {
                regs_.pc -= 2;
                return 21;
            }
            return 16;
        }
        case 0xBB: { // OTDR
            edOUTD();
            if (getB() != 0) {
                regs_.pc -= 2;
                return 21;
            }
            return 16;
        }

        default:
            return 4;
    }
}

#define IMPL_ED_ROW(high) \
    unsigned Z80::opED##high##0() { return executeED(0x##high##0); } \
    unsigned Z80::opED##high##1() { return executeED(0x##high##1); } \
    unsigned Z80::opED##high##2() { return executeED(0x##high##2); } \
    unsigned Z80::opED##high##3() { return executeED(0x##high##3); } \
    unsigned Z80::opED##high##4() { return executeED(0x##high##4); } \
    unsigned Z80::opED##high##5() { return executeED(0x##high##5); } \
    unsigned Z80::opED##high##6() { return executeED(0x##high##6); } \
    unsigned Z80::opED##high##7() { return executeED(0x##high##7); } \
    unsigned Z80::opED##high##8() { return executeED(0x##high##8); } \
    unsigned Z80::opED##high##9() { return executeED(0x##high##9); } \
    unsigned Z80::opED##high##A() { return executeED(0x##high##A); } \
    unsigned Z80::opED##high##B() { return executeED(0x##high##B); } \
    unsigned Z80::opED##high##C() { return executeED(0x##high##C); } \
    unsigned Z80::opED##high##D() { return executeED(0x##high##D); } \
    unsigned Z80::opED##high##E() { return executeED(0x##high##E); } \
    unsigned Z80::opED##high##F() { return executeED(0x##high##F); }

IMPL_ED_ROW(0) IMPL_ED_ROW(1) IMPL_ED_ROW(2) IMPL_ED_ROW(3)
IMPL_ED_ROW(4) IMPL_ED_ROW(5) IMPL_ED_ROW(6) IMPL_ED_ROW(7)
IMPL_ED_ROW(8) IMPL_ED_ROW(9) IMPL_ED_ROW(A) IMPL_ED_ROW(B)
IMPL_ED_ROW(C) IMPL_ED_ROW(D) IMPL_ED_ROW(E) IMPL_ED_ROW(F)

#undef IMPL_ED_ROW
