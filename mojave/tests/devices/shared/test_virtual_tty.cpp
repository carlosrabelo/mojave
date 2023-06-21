#include <cstdint>
#include "catch.hpp"
#include "devices/shared/virtual_tty.hpp"

TEST_CASE("VirtualTTY starts empty", "[tty][fast]") {
    VirtualTTY tty;
    REQUIRE_FALSE(tty.rxHasData());
    REQUIRE(tty.txHasSpace());
    REQUIRE(tty.readChar() == -1);
}

TEST_CASE("VirtualTTY injectChar and readPort data", "[tty][fast]") {
    VirtualTTY tty;

    tty.injectChar('A');
    tty.injectChar('B');
    REQUIRE(tty.rxHasData());

    REQUIRE(tty.readPort(0) == 'A');
    REQUIRE(tty.readPort(0) == 'B');
    REQUIRE_FALSE(tty.rxHasData());
}

TEST_CASE("VirtualTTY writePort and readChar", "[tty][fast]") {
    VirtualTTY tty;

    tty.writePort(0, 'H');
    tty.writePort(0, 'i');

    REQUIRE(tty.readChar() == 'H');
    REQUIRE(tty.readChar() == 'i');
    REQUIRE(tty.readChar() == -1);
}

TEST_CASE("VirtualTTY status port reflects buffer state", "[tty][fast]") {
    VirtualTTY tty;

    // Empty: no RX data, TX ready
    uint8_t st = tty.readPort(1);
    REQUIRE((st & 0x01) == 0);
    REQUIRE((st & 0x02) == 0x02);

    tty.injectChar('X');
    st = tty.readPort(1);
    REQUIRE((st & 0x01) == 0x01);
    REQUIRE((st & 0x02) == 0x02);
}

TEST_CASE("VirtualTTY write to status port does nothing", "[tty][fast]") {
    VirtualTTY tty;
    tty.writePort(1, 0xFF);
    REQUIRE(tty.readChar() == -1);
}

TEST_CASE("VirtualTTY memory-mapped interface mirrors port I/O", "[tty][fast]") {
    VirtualTTY tty;

    tty.injectChar('M');
    REQUIRE(tty.read(1) == 0x03); // RX available + TX ready
    REQUIRE(tty.read(0) == 'M');

    tty.write(0, 'O');
    REQUIRE(tty.readChar() == 'O');
}

TEST_CASE("VirtualTTY reset clears all buffers", "[tty][fast]") {
    VirtualTTY tty;

    tty.injectChar('A');
    tty.writePort(0, 'B');
    REQUIRE(tty.rxHasData());
    REQUIRE(tty.readChar() != -1);

    tty.reset();

    REQUIRE_FALSE(tty.rxHasData());
    REQUIRE(tty.txHasSpace());
    REQUIRE(tty.readChar() == -1);
}

TEST_CASE("VirtualTTY read empty data port returns 0", "[tty][fast]") {
    VirtualTTY tty;
    REQUIRE(tty.readPort(0) == 0);
}

TEST_CASE("VirtualTTY write to full TX buffer is silently discarded", "[tty][fast]") {
    VirtualTTY tty;

    for (int i = 0; i < 256; ++i)
        tty.writePort(0, static_cast<uint8_t>(i & 0xFF));

    // Buffer is full, next write should be discarded
    tty.writePort(0, 0xFF);

    // Read all 256 chars
    for (int i = 0; i < 256; ++i) {
        int c = tty.readChar();
        REQUIRE(c != -1);
        REQUIRE(c == (i & 0xFF));
    }

    // 257th read fails
    REQUIRE(tty.readChar() == -1);
}
