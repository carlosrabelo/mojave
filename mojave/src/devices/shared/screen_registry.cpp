#include "devices/shared/screen_registry.hpp"
#include "devices/shared/framebuffer.hpp"

void ScreenRegistry::registerScreen(const std::string& id, Framebuffer& fb) {
    screens_[id] = &fb;
}

Framebuffer* ScreenRegistry::screen(const std::string& id) const {
    auto it = screens_.find(id);
    if (it == screens_.end()) return nullptr;
    return it->second;
}

bool ScreenRegistry::hasScreen(const std::string& id) const {
    return screens_.find(id) != screens_.end();
}

void ScreenRegistry::removeScreen(const std::string& id) {
    screens_.erase(id);
}

void ScreenRegistry::clear() {
    screens_.clear();
}
