#ifndef MOJAVE_SDL_SCREEN_HPP
#define MOJAVE_SDL_SCREEN_HPP

#include <cstdint>

struct SDL_Window;
struct SDL_Renderer;
struct SDL_Texture;
class Framebuffer;
class MachineKeyboardBridge;
class Machine;

class SdlScreen {
public:
    explicit SdlScreen(Framebuffer& fb, int scale = 2);
    ~SdlScreen();

    SdlScreen(const SdlScreen&) = delete;
    SdlScreen& operator=(const SdlScreen&) = delete;

    bool init(bool text_input = false);
    void render();
    bool pollEvents(MachineKeyboardBridge* keyboard_bridge = nullptr, Machine* machine = nullptr);
    void close();

private:
    Framebuffer& fb_;
    int scale_;
    SDL_Window* window_ = nullptr;
    SDL_Renderer* renderer_ = nullptr;
    SDL_Texture* texture_ = nullptr;
};

#endif
