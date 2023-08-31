#ifndef MOJAVE_TRS80M3_HOST_INPUT_HPP
#define MOJAVE_TRS80M3_HOST_INPUT_HPP

#include "devices/trs80m3/keyboard.hpp"
#include "machines/trs80m3/host_keyboard_adapter.hpp"
#include "machines/trs80m3/host_typing.hpp"

class Machine;

void trs80m3HostPulseCharacter(Trs80M3HostKeyboardAdapter& adapter, char ch);
bool trs80m3HostPulseHostCharacter(Trs80M3HostKeyboardAdapter& adapter, char host_ch);
bool trs80m3HostPulseHostText(Trs80M3HostKeyboardAdapter& adapter, const char* utf8);
void trs80m3HostPulseSpecialKey(Trs80M3HostKeyboardAdapter& adapter, Trs80M3Keyboard::SpecialKey key,
                                  Machine* machine = nullptr);
void trs80m3SyncHostShift(Trs80M3HostKeyboardAdapter& adapter, bool shift_down);
bool trs80m3HostKeyDown(Trs80M3HostKeyboardAdapter& adapter, char name);
void trs80m3HostKeyUp(Trs80M3HostKeyboardAdapter& adapter, char name);
bool trs80m3HostSpecialDown(Trs80M3HostKeyboardAdapter& adapter, Trs80M3Keyboard::SpecialKey key);
void trs80m3HostSpecialUp(Trs80M3HostKeyboardAdapter& adapter, Trs80M3Keyboard::SpecialKey key);

#endif
