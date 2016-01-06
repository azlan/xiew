#include "mydisassembly.h"

MyDisassembly::MyDisassembly(QWidget* parent) : Disassembly (parent)
{
}

MyDisassembly::~MyDisassembly()
{
}

void MyDisassembly::keyPressEvent(QKeyEvent *event)
{
    int wKey = event->key();

    if (wKey == Qt::Key_PageUp)
    {
        dsint previousPageRVA = getInstructionRVA(getTableOffset(), -getNbrOfLineToPrint());
        setSingleSelection( previousPageRVA);
    }
    else if (wKey == Qt::Key_PageDown)
    {
        dsint nextPageRVA = getInstructionRVA(getTableOffset(), getNbrOfLineToPrint());
        setSingleSelection( nextPageRVA);
    }

    Disassembly::keyPressEvent(event);
    emit keyPressSignal(wKey);
    emit currentOffsetSignal(getInitialSelection());
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
