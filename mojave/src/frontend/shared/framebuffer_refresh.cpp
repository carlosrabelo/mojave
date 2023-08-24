#include "frontend/shared/framebuffer_refresh.hpp"
#include "machines/shared/machine.hpp"
#include "devices/shared/framebuffer.hpp"
#include "devices/trs80m3/video_controller.hpp"

namespace {

Framebuffer* findFramebuffer(Machine& machine) {
    for (const auto& dev : machine.ownedDevices()) {
        if (auto* fb = dynamic_cast<Framebuffer*>(dev.get()))
            return fb;
    }
    return nullptr;
}

} // namespace

void refreshMachineFramebuffer(Machine& machine, Framebuffer& fb) {
    for (const auto& dev : machine.ownedDevices()) {
        if (auto* video = dynamic_cast<Trs80M3VideoController*>(dev.get())) {
            video->refreshFramebuffer();
            return;
        }
    }
    if (findFramebuffer(machine) == nullptr)
        fb.fill(0xFF000000u);
}
