#include "machines/trs80m3/trs80m3_preset.hpp"
#include "machines/shared/machine.hpp"
#include "cpus/z80.hpp"
#include "devices/shared/memory.hpp"
#include "devices/shared/framebuffer.hpp"
#include "devices/trs80m3/video_controller.hpp"
#include "devices/trs80m3/io_latches.hpp"
#include "devices/trs80m3/port_decode.hpp"
#include "devices/trs80m3/keyboard.hpp"

namespace {

using Contract = Trs80M3PresetContract;

} // namespace

std::unique_ptr<Machine> createTrs80M3Machine() {
    auto cpu = std::make_unique<Z80>();
    auto machine = std::make_unique<Machine>(std::move(cpu));
    machine->setGuestCpuClockHz(Contract::guest_cpu_clock_hz);

    auto rom_low = std::make_unique<Memory>(Contract::io_latch_start - Contract::rom_start, true);
    machine->attachDevice(std::move(rom_low), Contract::rom_start, Contract::io_latch_start);

    auto io_latches = std::make_unique<Trs80M3IoLatches>();
    machine->attachDevice(std::move(io_latches), Contract::io_latch_start, Contract::io_latch_end_exclusive);

    auto rom_tail =
        std::make_unique<Memory>(Contract::rom_end_exclusive - Contract::rom_tail_start, true);
    machine->attachDevice(std::move(rom_tail), Contract::rom_tail_start, Contract::rom_end_exclusive);

    auto keyboard = std::make_unique<Trs80M3Keyboard>();
    machine->attachDevice(std::move(keyboard), Contract::keyboard_start, Contract::keyboard_end_exclusive);

    auto ram = std::make_unique<Memory>(Contract::ram_end_exclusive - Contract::ram_start);
    machine->attachDevice(std::move(ram), Contract::ram_start, Contract::ram_bus_end);

    auto fb = std::make_unique<Framebuffer>(Trs80M3VideoController::kFramebufferWidth,
                                            Trs80M3VideoController::kFramebufferHeight);
    Framebuffer* fb_ptr = fb.get();
    machine->addOwnedDevice(std::move(fb));

    auto video = std::make_unique<Trs80M3VideoController>(*fb_ptr);
    machine->attachDevice(std::move(video), Contract::vram_start, Contract::vram_end_exclusive);

    auto port_decode = std::make_unique<Trs80M3PortDecode>();
    Trs80M3PortDecode* port_ptr = port_decode.get();
    machine->bus().attachPort(*port_decode, Contract::port_decode_start,
                              Contract::port_decode_end_exclusive, true);
    machine->addOwnedDevice(std::move(port_decode));

    if (auto* z80 = dynamic_cast<Z80*>(&machine->cpu()))
        port_ptr->bindCpu(z80);

    return machine;
}

bool isTrs80M3PresetId(const std::string& id) {
    return id == "trs80m3";
}
