#ifndef MOJAVE_SINCLAIR_QT_INPUT_HPP
#define MOJAVE_SINCLAIR_QT_INPUT_HPP

class Machine;
class QKeyEvent;
class SinclairHostKeyboardAdapter;

bool sinclairHandleQtKeyEvent(SinclairHostKeyboardAdapter& adapter, QKeyEvent* event,
                              Machine* machine = nullptr);

#endif
