#include "frontend/shared/trs80m3_host_input.hpp"
#include "machines/shared/machine.hpp"

void trs80m3HostPulseCharacter(Trs80M3HostKeyboardAdapter& adapter, char ch) {
    adapter.pulseCharacter(ch);
}

bool trs80m3HostPulseHostCharacter(Trs80M3HostKeyboardAdapter& adapter, char host_ch) {
    const char text[2] = {host_ch, '\0'};
    return trs80m3HostPulseText(adapter, text);
}

bool trs80m3HostPulseHostText(Trs80M3HostKeyboardAdapter& adapter, const char* utf8) {
    return trs80m3HostPulseText(adapter, utf8);
}

void trs80m3HostPulseSpecialKey(Trs80M3HostKeyboardAdapter& adapter, Trs80M3Keyboard::SpecialKey key,
                                Machine*) {
    adapter.pulseSpecialKey(key);
}

void trs80m3SyncHostShift(Trs80M3HostKeyboardAdapter& adapter, bool shift_down) {
    adapter.syncHostShift(shift_down);
}

bool trs80m3HostKeyDown(Trs80M3HostKeyboardAdapter& adapter, char name) {
    return adapter.hostKeyDown(name);
}

void trs80m3HostKeyUp(Trs80M3HostKeyboardAdapter& adapter, char name) {
    adapter.hostKeyUp(name);
}

bool trs80m3HostSpecialDown(Trs80M3HostKeyboardAdapter& adapter, Trs80M3Keyboard::SpecialKey key) {
    return adapter.hostSpecialDown(key);
}

void trs80m3HostSpecialUp(Trs80M3HostKeyboardAdapter& adapter, Trs80M3Keyboard::SpecialKey key) {
    adapter.hostSpecialUp(key);
}
