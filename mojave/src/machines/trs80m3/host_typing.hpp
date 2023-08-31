#ifndef MOJAVE_TRS80M3_HOST_TYPING_HPP
#define MOJAVE_TRS80M3_HOST_TYPING_HPP

class Trs80M3HostKeyboardAdapter;

bool trs80m3HostCharFromText(const char* utf8, char& out);
bool trs80m3HostPulseText(Trs80M3HostKeyboardAdapter& adapter, const char* utf8);

#endif
