#ifndef MOJAVE_ZX81_DISPLAY_FILE_HPP
#define MOJAVE_ZX81_DISPLAY_FILE_HPP

#include <array>
#include <cstdint>
#include <functional>
#include <optional>

struct Zx81DisplayFileLine {
    std::array<uint8_t, 32> chars{};
    uint8_t length = 0;
};

struct Zx81DisplayFileGrid {
    std::array<Zx81DisplayFileLine, 24> rows{};
    uint8_t row_count = 0;
    uint16_t byte_length = 0;
};

class Zx81DisplayFile {
public:
    // 0x76 is NEWLINE in the ZX-81 display file (same numeric value as Z80 HALT).
    static constexpr uint8_t kNewline = 0x76;
    static constexpr unsigned kColumns = 32;
    static constexpr unsigned kRows = 24;
    static constexpr unsigned kMinCollapsedBytes = 25;
    static constexpr unsigned kMaxBytes = 793;
    static constexpr uint16_t kDFilePointerAddress = 0x400C;

    using ReadFn = std::function<uint8_t(uint16_t address)>;
    using WriteFn = std::function<void(uint16_t address, uint8_t value)>;

    static std::optional<Zx81DisplayFileGrid> parse(ReadFn read, uint16_t dfile_start);
    static uint16_t readPointer(ReadFn read, uint16_t pointer_address = kDFilePointerAddress);
    static void writePointer(WriteFn write, uint16_t pointer, uint16_t pointer_address = kDFilePointerAddress);
    static uint16_t writeCollapsed(WriteFn write, uint16_t dfile_start);
};

#endif
