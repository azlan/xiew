#ifndef MYDUMP_H
#define MYDUMP_H

#include "HexDump.h"

enum ViewEnum_t
{
    ViewHexAscii = 0,
    ViewHexUnicode,
    ViewTextAscii,
    ViewTextUnicode,
};

class QFile;
class QEvent;

class MyDump : public HexDump
{
    Q_OBJECT

public:
    explicit MyDump(QWidget* parent = 0);
    ~MyDump();
    QString paintContent(QPainter* painter, dsint rowBase, int rowOffset, int col, int x, int y, int w, int h);
    void hexAsciiSlot();

private:
    void keyPressEvent(QKeyEvent* e);
    bool event(QEvent * event);

signals:
    void keyPressSignal(int key);

};

#endif // MYDUMP_H
