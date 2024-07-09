#ifndef MOJAVE_ZX80_VIDEO_GENERATOR_HPP
#define MOJAVE_ZX80_VIDEO_GENERATOR_HPP

#include <cstdint>
#include "bus/bus.hpp"
#include "devices/device.hpp"
#include "devices/shared/framebuffer.hpp"
#include "devices/zx80/display_file.hpp"

class Zx80VideoGenerator : public Device {
public:
    static constexpr uint16_t kColumns = Zx80DisplayFile::kColumns;
    static constexpr uint16_t kRows = Zx80DisplayFile::kRows;
    static constexpr uint16_t kCellSize = 8;
    static constexpr uint16_t kFramebufferWidth = kColumns * kCellSize;
    static constexpr uint16_t kFramebufferHeight = kRows * kCellSize;
    static constexpr uint32_t kFrameHz = 50;

    Zx80VideoGenerator(Framebuffer& fb, Bus& bus, uint32_t guest_cpu_clock_hz);

    uint8_t read(uint16_t address) override;
    void write(uint16_t address, uint8_t value) override;
    void reset() override;
    void tick(unsigned cycles) override;

    void refreshFramebuffer();
    uint32_t framesRendered() const { return frames_rendered_; }

    static void renderToFramebuffer(Framebuffer& fb, Zx80DisplayFile::ReadFn read);

private:
    Framebuffer& fb_;
    Bus& bus_;
    uint32_t cycles_per_frame_;
    uint32_t cycle_accumulator_ = 0;
    uint32_t frames_rendered_ = 0;

    void renderFrame();
};

#endif
