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

bool MyDisassembly::event(QEvent *event)
{
    if (event->type()==QEvent::KeyPress )
    {
        QKeyEvent *k = (QKeyEvent *)event;
        if (k->key() == Qt::Key_Tab)
        {
            emit keyPressSignal(Qt::Key_Tab);
            return true;
        }
    }
    QAbstractScrollArea::event(event);
    return true;
}
