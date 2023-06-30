#ifndef MOJAVE_FRAMEBUFFER_REFRESH_HPP
#define MOJAVE_FRAMEBUFFER_REFRESH_HPP

class Machine;
class Framebuffer;

// Re-rasterize the active machine video device into its framebuffer. Frontends
// call this once per rendered frame so the display stays current even while the
// guest CPU is halted (e.g. the ZX-80 display-refresh loop). Machines without a
// video device leave the supplied placeholder framebuffer blank.
void refreshMachineFramebuffer(Machine& machine, Framebuffer& fb);

#endif
