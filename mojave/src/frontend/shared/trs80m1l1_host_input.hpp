#ifndef MOJAVE_TRS80M1L1_HOST_INPUT_HPP
#define MOJAVE_TRS80M1L1_HOST_INPUT_HPP

#include "devices/trs80m1/keyboard.hpp"
#include "machines/trs80m1l1/host_keyboard_adapter.hpp"
#include "machines/trs80m1l1/host_typing.hpp"

class Machine;

void trs80m1l1HostPulseCharacter(Trs80M1L1HostKeyboardAdapter& adapter, char ch);
bool trs80m1l1HostPulseHostCharacter(Trs80M1L1HostKeyboardAdapter& adapter, char host_ch);
bool trs80m1l1HostPulseHostText(Trs80M1L1HostKeyboardAdapter& adapter, const char* utf8);
void trs80m1l1HostPulseSpecialKey(Trs80M1L1HostKeyboardAdapter& adapter, Trs80M1Keyboard::SpecialKey key,
                                    Machine* machine = nullptr);
void trs80m1l1SyncHostShift(Trs80M1L1HostKeyboardAdapter& adapter, bool shift_down);
bool trs80m1l1HostKeyDown(Trs80M1L1HostKeyboardAdapter& adapter, char name);
void trs80m1l1HostKeyUp(Trs80M1L1HostKeyboardAdapter& adapter, char name);
bool trs80m1l1HostSpecialDown(Trs80M1L1HostKeyboardAdapter& adapter, Trs80M1Keyboard::SpecialKey key);
void trs80m1l1HostSpecialUp(Trs80M1L1HostKeyboardAdapter& adapter, Trs80M1Keyboard::SpecialKey key);

#endif
