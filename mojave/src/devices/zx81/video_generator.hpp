#ifndef MOJAVE_ZX81_VIDEO_GENERATOR_HPP
#define MOJAVE_ZX81_VIDEO_GENERATOR_HPP

#include <cstdint>
#include "bus/bus.hpp"
#include "devices/device.hpp"
#include "devices/shared/framebuffer.hpp"
#include "devices/zx81/display_file.hpp"

class Z80;
class Zx81PortDecode;

class Zx81VideoGenerator : public Device {
public:
    static constexpr uint16_t kColumns = Zx81DisplayFile::kColumns;
    static constexpr uint16_t kRows = Zx81DisplayFile::kRows;
    static constexpr uint16_t kCellSize = 8;
    static constexpr uint16_t kFramebufferWidth = kColumns * kCellSize;
    static constexpr uint16_t kFramebufferHeight = kRows * kCellSize;
    static constexpr uint32_t kFrameHz = 50;
    // ~TV line period at 3.25 MHz (NMI generator rate while OUT FE is active).
    static constexpr uint32_t kNmiPeriodCycles = 207;

    // CDFLAG (0x403B) bit 7: 1 = SLOW (display kept), 0 = FAST (blank).
    static constexpr uint16_t kCdflagAddress = 0x403B;
    static constexpr uint8_t kCdflagSlowDisplayBit = 7;
    static constexpr uint8_t kCdflagSlowDisplayMask = 1u << kCdflagSlowDisplayBit;

    Zx81VideoGenerator(Framebuffer& fb, Bus& bus, Z80& cpu, Zx81PortDecode& ports,
                       uint32_t guest_cpu_clock_hz);

    uint8_t read(uint16_t address) override;
    void write(uint16_t address, uint8_t value) override;
    void reset() override;
    void tick(unsigned cycles) override;

    void refreshFramebuffer();
    uint32_t framesRendered() const { return frames_rendered_; }
    uint32_t nmisRaised() const { return nmis_raised_; }
    uint32_t cyclesPerFrame() const { return cycles_per_frame_; }

    static bool isDisplayActive(Zx81DisplayFile::ReadFn read);
    static void renderToFramebuffer(Framebuffer& fb, Zx81DisplayFile::ReadFn read);

private:
    Framebuffer& fb_;
    Bus& bus_;
    Z80& cpu_;
    Zx81PortDecode& ports_;
    uint32_t cycles_per_frame_;
    uint32_t cycle_accumulator_ = 0;
    uint32_t nmi_cycle_accumulator_ = 0;
    uint32_t frames_rendered_ = 0;
    uint32_t nmis_raised_ = 0;

    void updateDisplay();
    void renderFrame();
    void serviceUlaInterrupts(unsigned cycles);
};

#endif
