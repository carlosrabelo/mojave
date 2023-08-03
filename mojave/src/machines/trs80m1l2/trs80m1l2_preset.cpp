#include "machines/trs80m1l2/trs80m1l2_preset.hpp"
#include "machines/shared/machine.hpp"
#include "cpus/z80.hpp"
#include "devices/shared/memory.hpp"
#include "devices/shared/framebuffer.hpp"
#include "devices/trs80m1/video_controller.hpp"
#include "devices/trs80m1/printer_status.hpp"
#include "devices/trs80m1/system_port.hpp"
#include "devices/trs80m1/keyboard.hpp"
#include "devices/trs80m1l2/expansion_ports.hpp"

namespace {

using Contract = Trs80M1L2PresetContract;

} // namespace

std::unique_ptr<Machine> createTrs80M1L2Machine() {
    auto cpu = std::make_unique<Z80>();
    auto machine = std::make_unique<Machine>(std::move(cpu));
    machine->setGuestCpuClockHz(Contract::guest_cpu_clock_hz);

    auto rom = std::make_unique<Memory>(Contract::rom_end_exclusive - Contract::rom_start, true);
    machine->attachDevice(std::move(rom), Contract::rom_start, Contract::rom_end_exclusive);

    auto ram = std::make_unique<Memory>(Contract::ram_end_exclusive - Contract::ram_start);
    machine->attachDevice(std::move(ram), Contract::ram_start, Contract::ram_end_exclusive);

    auto printer = std::make_unique<Trs80M1PrinterStatus>();
    machine->attachDevice(std::move(printer), Contract::printer_status_address,
                          static_cast<uint16_t>(Contract::printer_status_address + 1));

    auto keyboard = std::make_unique<Trs80M1Keyboard>();
    machine->attachDevice(std::move(keyboard), Contract::keyboard_start, Contract::keyboard_end_exclusive);

    auto expansion_ram =
        std::make_unique<Memory>(Contract::expansion_ram_end_exclusive - Contract::expansion_ram_start);
    machine->attachDevice(std::move(expansion_ram), Contract::expansion_ram_start,
                          Contract::expansion_ram_bus_end);

    auto fb = std::make_unique<Framebuffer>(Trs80M1VideoController::kFramebufferWidth,
                                            Trs80M1VideoController::kFramebufferHeight);
    Framebuffer* fb_ptr = fb.get();
    machine->addOwnedDevice(std::move(fb));

    auto video = std::make_unique<Trs80M1VideoController>(*fb_ptr);
    Trs80M1VideoController* video_ptr = video.get();
    machine->attachDevice(std::move(video), Contract::vram_start, Contract::vram_end_exclusive);

    auto system_port = std::make_unique<Trs80M1SystemPort>(*video_ptr);
    machine->bus().attachPort(*system_port, Contract::system_port,
                              static_cast<uint16_t>(Contract::system_port + 1));
    machine->addOwnedDevice(std::move(system_port));

    auto expansion_ports = std::make_unique<Trs80M1L2ExpansionPorts>();
    machine->bus().attachPort(*expansion_ports, Contract::expansion_port_start,
                              Contract::expansion_port_end_exclusive);
    machine->addOwnedDevice(std::move(expansion_ports));

    return machine;
}

bool isTrs80M1L2PresetId(const std::string& id) {
    return id == "trs80m1l2";
}
