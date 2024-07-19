#include "catch.hpp"
#include "devices/sinclair/host_keymap.hpp"

TEST_CASE("Sinclair host keymap maps letters and digits", "[device][sinclair][fast]") {
    SinclairKeyboard::Key key = SinclairKeyboard::Key::Space;

    REQUIRE(sinclairHostKeyFromLetter('p', key));
    REQUIRE(key == SinclairKeyboard::Key::P);

    REQUIRE(sinclairHostKeyFromLetter('Z', key));
    REQUIRE(key == SinclairKeyboard::Key::Z);

    REQUIRE(sinclairHostKeyFromDigit('0', key));
    REQUIRE(key == SinclairKeyboard::Key::Digit0);

    REQUIRE_FALSE(sinclairHostKeyFromLetter('!', key));
    REQUIRE_FALSE(sinclairHostKeyFromDigit('a', key));
}
