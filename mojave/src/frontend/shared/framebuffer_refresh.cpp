#include "frontend/shared/framebuffer_refresh.hpp"
#include "machines/shared/machine.hpp"
#include "devices/shared/framebuffer.hpp"

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
    // Video-device refresh paths land with each display machine.
    if (findFramebuffer(machine) == nullptr)
        fb.fill(0xFF000000u);
}
