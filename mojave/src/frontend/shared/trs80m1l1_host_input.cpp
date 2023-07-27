#include "frontend/shared/trs80m1l1_host_input.hpp"
#include "cpus/z80.hpp"
#include "machines/shared/machine.hpp"

void trs80m1l1HostPulseCharacter(Trs80M1L1HostKeyboardAdapter& adapter, char ch) {
    adapter.pulseCharacter(ch);
}

bool trs80m1l1HostPulseHostCharacter(Trs80M1L1HostKeyboardAdapter& adapter, char host_ch) {
    char text[2] = {host_ch, '\0'};
    return trs80m1l1HostPulseText(adapter, text);
}

bool trs80m1l1HostPulseHostText(Trs80M1L1HostKeyboardAdapter& adapter, const char* utf8) {
    return trs80m1l1HostPulseText(adapter, utf8);
}

void trs80m1l1HostPulseSpecialKey(Trs80M1L1HostKeyboardAdapter& adapter, Trs80M1Keyboard::SpecialKey key,
                                  Machine* machine) {
    adapter.pulseSpecialKey(key);
    if (key != Trs80M1Keyboard::SpecialKey::Break || machine == nullptr)
        return;
    if (auto* z80 = dynamic_cast<Z80*>(&machine->cpu()))
        z80->requestNmi();
}

void trs80m1l1SyncHostShift(Trs80M1L1HostKeyboardAdapter& adapter, bool shift_down) {
    adapter.syncHostShift(shift_down);
}

bool trs80m1l1HostKeyDown(Trs80M1L1HostKeyboardAdapter& adapter, char name) {
    return adapter.hostKeyDown(name);
}

void trs80m1l1HostKeyUp(Trs80M1L1HostKeyboardAdapter& adapter, char name) {
    adapter.hostKeyUp(name);
}

bool trs80m1l1HostSpecialDown(Trs80M1L1HostKeyboardAdapter& adapter, Trs80M1Keyboard::SpecialKey key) {
    return adapter.hostSpecialDown(key);
}

void trs80m1l1HostSpecialUp(Trs80M1L1HostKeyboardAdapter& adapter, Trs80M1Keyboard::SpecialKey key) {
    adapter.hostSpecialUp(key);
}
