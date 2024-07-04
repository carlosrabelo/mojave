#include <array>
#include <cstdint>
#include <vector>
#include "catch.hpp"
#include "devices/zx80/display_file.hpp"
#include "machines/zx80/zx80_preset.hpp"
#include "machines/shared/machine.hpp"

namespace {

std::vector<uint8_t> makeMemory(uint16_t size) {
    return std::vector<uint8_t>(size, 0xFF);
}

Zx80DisplayFile::ReadFn readerFor(std::vector<uint8_t>& ram, uint16_t base) {
    return [&ram, base](uint16_t address) -> uint8_t {
        if (address < base)
            return 0xFF;
        const uint16_t offset = static_cast<uint16_t>(address - base);
        if (offset >= ram.size())
            return 0xFF;
        return ram[offset];
    };
}

Zx80DisplayFile::WriteFn writerFor(std::vector<uint8_t>& ram, uint16_t base) {
    return [&ram, base](uint16_t address, uint8_t value) {
        if (address < base)
            return;
        const uint16_t offset = static_cast<uint16_t>(address - base);
        if (offset >= ram.size())
            return;
        ram[offset] = value;
    };
}

} // namespace

TEST_CASE("ZX-80 display file constants", "[device][zx80][fast]") {
    REQUIRE(Zx80DisplayFile::kHaltOpcode == 0x76);
    REQUIRE(Zx80DisplayFile::kColumns == 32u);
    REQUIRE(Zx80DisplayFile::kRows == 24u);
    REQUIRE(Zx80DisplayFile::kMinCollapsedBytes == 25u);
    REQUIRE(Zx80DisplayFile::kMaxBytes == 793u);
    REQUIRE(Zx80DisplayFile::kDFilePointerAddress == Zx80PresetContract::d_file_ptr_address);
}

TEST_CASE("ZX-80 display file parses collapsed 25-byte empty screen", "[device][zx80][fast]") {
    auto ram = makeMemory(0x400);
    const uint16_t dfile = 0x4200;
    Zx80DisplayFile::writeCollapsed(writerFor(ram, 0x4000), dfile);

    const auto grid = Zx80DisplayFile::parse(readerFor(ram, 0x4000), dfile);
    REQUIRE(grid.has_value());
    REQUIRE(grid->row_count == 24);
    REQUIRE(grid->byte_length == 25);

    for (unsigned row = 0; row < 24; ++row) {
        REQUIRE(grid->rows[row].length == 0);
        for (uint8_t col = 0; col < 32; ++col)
            REQUIRE(grid->rows[row].chars[col] == 0);
    }
}

TEST_CASE("ZX-80 display file parses single-character line", "[device][zx80][fast]") {
    auto ram = makeMemory(0x400);
    const uint16_t dfile = 0x4200;
    const uint8_t letter_a = 29;

    ram[dfile - 0x4000] = Zx80DisplayFile::kHaltOpcode;
    ram[dfile - 0x4000 + 1] = letter_a;
    ram[dfile - 0x4000 + 2] = Zx80DisplayFile::kHaltOpcode;
    for (unsigned i = 3; i < 26; ++i)
        ram[dfile - 0x4000 + i] = Zx80DisplayFile::kHaltOpcode;

    const auto grid = Zx80DisplayFile::parse(readerFor(ram, 0x4000), dfile);
    REQUIRE(grid.has_value());
    REQUIRE(grid->rows[0].length == 1);
    REQUIRE(grid->rows[0].chars[0] == letter_a);
    REQUIRE(grid->rows[1].length == 0);
}

TEST_CASE("ZX-80 display file parses 32-character line", "[device][zx80][fast]") {
    auto ram = makeMemory(0x400);
    const uint16_t dfile = 0x4200;

    uint16_t offset = 0;
    ram[dfile - 0x4000 + offset++] = Zx80DisplayFile::kHaltOpcode;
    for (unsigned col = 0; col < 32; ++col)
        ram[dfile - 0x4000 + offset++] = static_cast<uint8_t>(col + 1);
    ram[dfile - 0x4000 + offset++] = Zx80DisplayFile::kHaltOpcode;
    while (offset < 58)
        ram[dfile - 0x4000 + offset++] = Zx80DisplayFile::kHaltOpcode;

    const auto grid = Zx80DisplayFile::parse(readerFor(ram, 0x4000), dfile);
    REQUIRE(grid.has_value());
    REQUIRE(grid->rows[0].length == 32);
    REQUIRE(grid->rows[0].chars[0] == 1);
    REQUIRE(grid->rows[0].chars[31] == 32);
}

TEST_CASE("ZX-80 display file rejects missing leading HALT", "[device][zx80][fast]") {
    auto ram = makeMemory(0x400);
    const uint16_t dfile = 0x4200;
    ram[dfile - 0x4000] = 'X';

    REQUIRE_FALSE(Zx80DisplayFile::parse(readerFor(ram, 0x4000), dfile).has_value());
}

TEST_CASE("ZX-80 display file rejects line without terminating HALT", "[device][zx80][fast]") {
    auto ram = makeMemory(0x400);
    const uint16_t dfile = 0x4200;

    ram[dfile - 0x4000] = Zx80DisplayFile::kHaltOpcode;
    ram[dfile - 0x4000 + 1] = 29;

    REQUIRE_FALSE(Zx80DisplayFile::parse(readerFor(ram, 0x4000), dfile).has_value());
}

TEST_CASE("ZX-80 D_FILE pointer is little-endian at 0x400C", "[device][zx80][fast]") {
    auto ram = makeMemory(0x400);
    const auto read = readerFor(ram, 0x4000);
    const auto write = writerFor(ram, 0x4000);

    Zx80DisplayFile::writePointer(write, 0x4200);
    REQUIRE(Zx80DisplayFile::readPointer(read) == 0x4200);
    REQUIRE(ram[0x400C - 0x4000] == 0x00);
    REQUIRE(ram[0x400D - 0x4000] == 0x42);
}

TEST_CASE("ZX-80 machine RAM holds D_FILE pointer to parsed display file", "[machine][zx80][fast]") {
    auto machine = createZx80Machine();
    const uint16_t dfile = 0x4300;

    for (unsigned i = 0; i < Zx80DisplayFile::kMinCollapsedBytes; ++i)
        machine->bus().write(static_cast<uint16_t>(dfile + i), Zx80DisplayFile::kHaltOpcode);

    machine->bus().write(Zx80PresetContract::d_file_ptr_address, static_cast<uint8_t>(dfile & 0xFF));
    machine->bus().write(static_cast<uint16_t>(Zx80PresetContract::d_file_ptr_address + 1),
                         static_cast<uint8_t>(dfile >> 8));

    const auto read = [&machine](uint16_t address) { return machine->bus().read(address); };
    const uint16_t pointer = Zx80DisplayFile::readPointer(read);
    const auto grid = Zx80DisplayFile::parse(read, pointer);

    REQUIRE(pointer == dfile);
    REQUIRE(grid.has_value());
    REQUIRE(grid->byte_length == 25);
}
