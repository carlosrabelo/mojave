#ifndef MOJAVE_TRS80M3_FLOPPY_CONTROLLER_HPP
#define MOJAVE_TRS80M3_FLOPPY_CONTROLLER_HPP

#include <cstdint>
#include "devices/device.hpp"
#include "devices/shared/port_device.hpp"

class Z80;
class Trs80M3PortDecode;

class Trs80M3FloppyController : public Device, public PortDevice {
public:
    static constexpr uint16_t kPortStart = 0xF0;
    static constexpr uint16_t kPortEndExclusive = 0xF5;

    static constexpr uint8_t kStatusOffset = 0x00;
    static constexpr uint8_t kTrackOffset = 0x01;
    static constexpr uint8_t kSectorOffset = 0x02;
    static constexpr uint8_t kDataOffset = 0x03;
    static constexpr uint8_t kDriveSelectOffset = 0x04;

    // Idle / no-drive: floating bus. Model III ROM treats 0xFF as "no disk"
    // and falls through to the Cass?/Memory Size prompts.
    static constexpr uint8_t kStatusIdle = 0xFF;
    static constexpr uint8_t kStatusTrack0 = 0x04;
    static constexpr uint8_t kStatusDrq = 0x02;
    static constexpr uint8_t kStatusRnf = 0x10;

    explicit Trs80M3FloppyController(Trs80M3PortDecode& ports);

    uint8_t readPort(uint16_t port) override;
    void writePort(uint16_t port, uint8_t value) override;
    uint8_t read(uint16_t address) override;
    void write(uint16_t address, uint8_t value) override;
    void reset() override;

    uint8_t track() const { return track_; }
    uint8_t sector() const { return sector_; }
    uint8_t driveSelect() const { return drive_select_; }
    uint8_t lastCommand() const { return last_command_; }

    void bindCpu(Z80* cpu) { cpu_ = cpu; }

    // Explicit disk-index NMI for future media support. Drive-select writes do
    // not raise NMI automatically: early ROM init writes F4 and a premature
    // NMI would derail the Cass?/READY boot path.
    void requestDiskIndexNmi() { pulseIndexNmi(); }

private:
    uint8_t track_ = 0;
    uint8_t sector_ = 0;
    uint8_t data_ = 0;
    uint8_t drive_select_ = 0;
    uint8_t last_command_ = 0;
    uint8_t pending_status_ = 0;
    bool status_pending_ = false;
    Z80* cpu_ = nullptr;
    Trs80M3PortDecode& ports_;

    void issueCommand(uint8_t command);
    void pulseIndexNmi();
};

#endif
