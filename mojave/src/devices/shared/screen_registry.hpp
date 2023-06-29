#ifndef MOJAVE_SCREEN_REGISTRY_HPP
#define MOJAVE_SCREEN_REGISTRY_HPP

#include <string>
#include <unordered_map>

class Framebuffer;

class ScreenRegistry {
public:
    void registerScreen(const std::string& id, Framebuffer& fb);
    Framebuffer* screen(const std::string& id) const;
    bool hasScreen(const std::string& id) const;
    void removeScreen(const std::string& id);
    void clear();
    size_t count() const { return screens_.size(); }

private:
    std::unordered_map<std::string, Framebuffer*> screens_;
};

#endif
