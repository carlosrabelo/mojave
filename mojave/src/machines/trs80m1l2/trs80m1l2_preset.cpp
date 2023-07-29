#include "machines/trs80m1l2/trs80m1l2_preset.hpp"
#include "machines/shared/machine.hpp"
#include "cpus/z80.hpp"
#include "devices/shared/memory.hpp"
#include "devices/shared/framebuffer.hpp"
#include "devices/trs80m1/video_controller.hpp"

namespace {

using Contract = Trs80M1L2PresetContract;

} // namespace

std::unique_ptr<Machine> createTrs80M1L2Machine() {
    auto cpu = std::make_unique<Z80>();
    auto machine = std::make_unique<Machine>(std::move(cpu));

    auto rom = std::make_unique<Memory>(Contract::rom_end_exclusive - Contract::rom_start, true);
    machine->attachDevice(std::move(rom), Contract::rom_start, Contract::rom_end_exclusive);

    auto ram = std::make_unique<Memory>(Contract::ram_end_exclusive - Contract::ram_start);
    machine->attachDevice(std::move(ram), Contract::ram_start, Contract::ram_end_exclusive);

    auto fb = std::make_unique<Framebuffer>(Trs80M1VideoController::kFramebufferWidth,
                                            Trs80M1VideoController::kFramebufferHeight);
    Framebuffer* fb_ptr = fb.get();
    machine->addOwnedDevice(std::move(fb));

    auto video = std::make_unique<Trs80M1VideoController>(*fb_ptr);
    machine->attachDevice(std::move(video), Contract::vram_start, Contract::vram_end_exclusive);

    return machine;
}

bool isTrs80M1L2PresetId(const std::string& id) {
    return id == "trs80m1l2";
}
