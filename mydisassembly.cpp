#include "mydisassembly.h"

MyDisassembly::MyDisassembly(QWidget* parent) : Disassembly (parent)
{
}

MyDisassembly::~MyDisassembly()
{
}

void MyDisassembly::keyPressEvent(QKeyEvent *event)
{
    int key = event->key();
    Disassembly::keyPressEvent(event);
    emit keyPressSignal(key);
}
