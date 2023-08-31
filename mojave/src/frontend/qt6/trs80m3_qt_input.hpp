#ifndef MOJAVE_TRS80M3_QT_INPUT_HPP
#define MOJAVE_TRS80M3_QT_INPUT_HPP

class QKeyEvent;
class Machine;
class Trs80M3HostKeyboardAdapter;

bool trs80m3HandleQtKeyEvent(Trs80M3HostKeyboardAdapter& adapter, QKeyEvent* event,
                             Machine* machine = nullptr);

#endif
