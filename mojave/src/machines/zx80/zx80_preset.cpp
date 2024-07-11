#include "machines/zx80/zx80_preset.hpp"
#include "machines/shared/machine.hpp"
#include "cpus/z80.hpp"
#include "devices/shared/memory.hpp"
#include "devices/shared/framebuffer.hpp"
#include "devices/zx80/video_generator.hpp"
#include "devices/sinclair/keyboard.hpp"
#include "devices/zx80/port_decode.hpp"

namespace {

using Contract = Zx80PresetContract;

} // namespace

std::unique_ptr<Machine> createZx80Machine() {
    auto cpu = std::make_unique<Z80>();
    auto machine = std::make_unique<Machine>(std::move(cpu));
    machine->setGuestCpuClockHz(Contract::guest_cpu_clock_hz);

    auto rom = std::make_unique<Memory>(Contract::rom_end_exclusive - Contract::rom_start, true);
    machine->attachDevice(std::move(rom), Contract::rom_start, Contract::rom_end_exclusive);

    auto ram = std::make_unique<Memory>(Contract::ram_end_exclusive - Contract::ram_start);
    machine->attachDevice(std::move(ram), Contract::ram_start, Contract::ram_end_exclusive);

    auto fb = std::make_unique<Framebuffer>(Zx80VideoGenerator::kFramebufferWidth,
                                            Zx80VideoGenerator::kFramebufferHeight);
    Framebuffer* fb_ptr = fb.get();
    machine->addOwnedDevice(std::move(fb));

    auto video = std::make_unique<Zx80VideoGenerator>(*fb_ptr, machine->bus(), Contract::guest_cpu_clock_hz);
    machine->addOwnedDevice(std::move(video));

    auto keyboard = std::make_unique<SinclairKeyboard>();
    SinclairKeyboard* keyboard_ptr = keyboard.get();
    machine->addOwnedDevice(std::move(keyboard));

    auto ports = std::make_unique<Zx80PortDecode>(*keyboard_ptr);
    machine->bus().attachPort(*ports, Contract::io_port_attach_start, Contract::io_port_attach_end_exclusive);
    machine->addOwnedDevice(std::move(ports));

    return machine;
}

bool isZx80PresetId(const std::string& id) {
    return id == "zx80";
}
