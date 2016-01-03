#ifndef MYDISASSEMBLY_H
#define MYDISASSEMBLY_H

#include "Disassembly.h"

class MyDisassembly : public Disassembly
{
    Q_OBJECT

public:
    explicit MyDisassembly(QWidget* parent = 0);
    ~MyDisassembly();

private:
    void keyPressEvent(QKeyEvent* e);

signals:
    void keyPressSignal(int key);

};

#endif // MYDISASSEMBLY_H
