#ifndef MOJAVE_TRS80M1L1_HOST_TYPING_HPP
#define MOJAVE_TRS80M1L1_HOST_TYPING_HPP

class Trs80M1L1HostKeyboardAdapter;

bool trs80m1l1HostCharFromText(const char* utf8, char& out);
bool trs80m1l1HostPulseText(Trs80M1L1HostKeyboardAdapter& adapter, const char* utf8);

#endif
