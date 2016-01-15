#ifndef MYDISASSEMBLY_H
#define MYDISASSEMBLY_H

#include "Disassembly.h"

class XiewDisassembly : public Disassembly
{
    Q_OBJECT

public:
    explicit XiewDisassembly(QWidget* parent = 0);
    ~XiewDisassembly();

private:
    void keyPressEvent(QKeyEvent* e);

    bool event(QEvent *event);
signals:
    void keyPressSignal(int key);
    void currentOffsetSignal(int offset);
};

#endif // MYDISASSEMBLY_H
