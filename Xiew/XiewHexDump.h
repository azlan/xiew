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

class XiewHexDump : public HexDump
{
    Q_OBJECT

public:
    explicit XiewHexDump(QWidget* parent = 0);
    ~XiewHexDump();
    QString paintContent(QPainter* painter, dsint rowBase, int rowOffset, int col, int x, int y, int w, int h);
    void hexAsciiSlot();

private:
    void keyPressEvent(QKeyEvent* e);
    bool event(QEvent * event);

    void checkBottomRow(const int wCurrentTableOffset, const int wViewableRow, int wSelectedOffset, const int wBytePerRow);

    void checkTopRow(int wSelectedOffset, const int wCurrentTableOffset, const int wBytePerRow);

signals:
    void keyPressSignal(int key);
    void currentOffsetSignal(int offset);
    void currentTableOffsetSignal(int offset);

};

#endif // MYDUMP_H
