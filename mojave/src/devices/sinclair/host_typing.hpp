#ifndef MOJAVE_SINCLAIR_HOST_TYPING_HPP
#define MOJAVE_SINCLAIR_HOST_TYPING_HPP

class SinclairHostKeyboardAdapter;

bool sinclairHostCharFromText(const char* utf8, char& out);
bool sinclairHostPulseText(SinclairHostKeyboardAdapter& adapter, const char* utf8);

#endif
