#include "devices/zx81/display_file.hpp"

std::optional<Zx81DisplayFileGrid> Zx81DisplayFile::parse(ReadFn read, uint16_t dfile_start) {
    if (!read)
        return std::nullopt;

    uint16_t pc = dfile_start;
    if (read(pc) != kNewline)
        return std::nullopt;
    pc++;

    Zx81DisplayFileGrid grid{};
    for (unsigned row = 0; row < kRows; ++row) {
        Zx81DisplayFileLine& line = grid.rows[row];
        line.length = 0;

        while (line.length < kColumns) {
            const uint8_t value = read(pc);
            if (value == kNewline)
                break;
            line.chars[line.length++] = value;
            pc++;
        }

        if (read(pc) != kNewline)
            return std::nullopt;
        pc++;

        grid.row_count = static_cast<uint8_t>(row + 1);
    }

    grid.byte_length = static_cast<uint16_t>(pc - dfile_start);
    return grid;
}

uint16_t Zx81DisplayFile::readPointer(ReadFn read, uint16_t pointer_address) {
    if (!read)
        return 0;
    return static_cast<uint16_t>(read(pointer_address))
           | static_cast<uint16_t>(read(static_cast<uint16_t>(pointer_address + 1)) << 8);
}

void Zx81DisplayFile::writePointer(WriteFn write, uint16_t pointer, uint16_t pointer_address) {
    if (!write)
        return;
    write(pointer_address, static_cast<uint8_t>(pointer & 0xFF));
    write(static_cast<uint16_t>(pointer_address + 1), static_cast<uint8_t>(pointer >> 8));
}

uint16_t Zx81DisplayFile::writeCollapsed(WriteFn write, uint16_t dfile_start) {
    for (unsigned i = 0; i < kMinCollapsedBytes; ++i)
        write(static_cast<uint16_t>(dfile_start + i), kNewline);
    return static_cast<uint16_t>(dfile_start + kMinCollapsedBytes);
}
