#include <cstdint>
#include "catch.hpp"
#include "devices/shared/screen_registry.hpp"
#include "devices/shared/framebuffer.hpp"

TEST_CASE("ScreenRegistry starts empty", "[screen][fast]") {
    ScreenRegistry reg;
    REQUIRE(reg.count() == 0);
    REQUIRE_FALSE(reg.hasScreen("main"));
    REQUIRE(reg.screen("main") == nullptr);
}

TEST_CASE("ScreenRegistry registerScreen and screen", "[screen][fast]") {
    ScreenRegistry reg;
    Framebuffer fb(256, 192);

    reg.registerScreen("main", fb);
    REQUIRE(reg.count() == 1);
    REQUIRE(reg.hasScreen("main"));

    Framebuffer* p = reg.screen("main");
    REQUIRE(p != nullptr);
    REQUIRE(p->width() == 256);
    REQUIRE(p->height() == 192);
}

TEST_CASE("ScreenRegistry unknown id returns nullptr", "[screen][fast]") {
    ScreenRegistry reg;
    Framebuffer fb(16, 16);
    reg.registerScreen("main", fb);

    REQUIRE(reg.screen("nonexistent") == nullptr);
    REQUIRE_FALSE(reg.hasScreen("nope"));
}

TEST_CASE("ScreenRegistry multiple screens", "[screen][fast]") {
    ScreenRegistry reg;
    Framebuffer fb1(256, 192);
    Framebuffer fb2(320, 240);

    reg.registerScreen("main", fb1);
    reg.registerScreen("debug", fb2);

    REQUIRE(reg.count() == 2);
    REQUIRE(reg.screen("main")->width() == 256);
    REQUIRE(reg.screen("debug")->width() == 320);
}

TEST_CASE("ScreenRegistry removeScreen", "[screen][fast]") {
    ScreenRegistry reg;
    Framebuffer fb(16, 16);
    reg.registerScreen("main", fb);

    reg.removeScreen("main");
    REQUIRE(reg.count() == 0);
    REQUIRE_FALSE(reg.hasScreen("main"));
}

TEST_CASE("ScreenRegistry clear removes all", "[screen][fast]") {
    ScreenRegistry reg;
    Framebuffer fb1(16, 16);
    Framebuffer fb2(32, 32);
    reg.registerScreen("a", fb1);
    reg.registerScreen("b", fb2);

    reg.clear();
    REQUIRE(reg.count() == 0);
}

TEST_CASE("ScreenRegistry re-register overwrites", "[screen][fast]") {
    ScreenRegistry reg;
    Framebuffer fb1(16, 16);
    Framebuffer fb2(32, 32);

    reg.registerScreen("main", fb1);
    reg.registerScreen("main", fb2);

    REQUIRE(reg.count() == 1);
    REQUIRE(reg.screen("main")->width() == 32);
}
