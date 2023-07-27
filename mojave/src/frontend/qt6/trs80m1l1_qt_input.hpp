#ifndef MOJAVE_TRS80M1L1_QT_INPUT_HPP
#define MOJAVE_TRS80M1L1_QT_INPUT_HPP

class QKeyEvent;
class Trs80M1L1HostKeyboardAdapter;
class Machine;

bool trs80m1l1HandleQtKeyEvent(Trs80M1L1HostKeyboardAdapter& adapter, QKeyEvent* event,
                               Machine* machine = nullptr);

#endif
