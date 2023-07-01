#include "frontend/sdl/sdl_screen.hpp"
#include "devices/shared/framebuffer.hpp"
#include "frontend/shared/machine_keyboard_bridge.hpp"
#include <SDL2/SDL.h>

SdlScreen::SdlScreen(Framebuffer& fb, int scale)
    : fb_(fb), scale_(scale) {}

SdlScreen::~SdlScreen() {
    close();
}

bool SdlScreen::init(bool text_input) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) return false;

    int w = fb_.width() * scale_;
    int h = fb_.height() * scale_;

    window_ = SDL_CreateWindow(
        "mojave",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        w, h,
        SDL_WINDOW_SHOWN
    );
    if (!window_) return false;

    renderer_ = SDL_CreateRenderer(window_, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer_) renderer_ = SDL_CreateRenderer(window_, -1, SDL_RENDERER_SOFTWARE);
    if (!renderer_) return false;

    texture_ = SDL_CreateTexture(
        renderer_,
        SDL_PIXELFORMAT_RGBA32,
        SDL_TEXTUREACCESS_STREAMING,
        fb_.width(), fb_.height()
    );
    if (!texture_) return false;

    if (text_input)
        SDL_StartTextInput();
    return true;
}

void SdlScreen::render() {
    if (!renderer_ || !texture_) return;

    void* pixels = nullptr;
    int pitch = 0;
    if (SDL_LockTexture(texture_, nullptr, &pixels, &pitch) == 0) {
        memcpy(pixels, fb_.pixels(),
               static_cast<size_t>(fb_.width()) * fb_.height() * 4);
        SDL_UnlockTexture(texture_);
    }

    SDL_RenderClear(renderer_);
    SDL_RenderCopy(renderer_, texture_, nullptr, nullptr);
    SDL_RenderPresent(renderer_);
}

bool SdlScreen::pollEvents(MachineKeyboardBridge* keyboard_bridge, Machine* machine) {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) return false;
        if (keyboard_bridge)
            keyboard_bridge->handleSdlKeyboardEvent(&e, machine);
    }
    return true;
}

void SdlScreen::close() {
    if (texture_) { SDL_DestroyTexture(texture_); texture_ = nullptr; }
    if (renderer_) { SDL_DestroyRenderer(renderer_); renderer_ = nullptr; }
    if (window_) { SDL_DestroyWindow(window_); window_ = nullptr; }
    SDL_Quit();
}
