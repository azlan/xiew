#ifndef MYDUMP_H
#define MYDUMP_H

#include "HexDump.h"

enum ViewEnum_t
{
    ViewHexAscii = 0,
    ViewHexUnicode,
    ViewTextAscii,
    ViewTextUnicode,
    ViewIntegerSignedShort,
    ViewIntegerSignedLong,
    ViewIntegerSignedLongLong,
    ViewIntegerUnsignedShort,
    ViewIntegerUnsignedLong,
    ViewIntegerUnsignedLongLong,
    ViewIntegerHexShort,
    ViewIntegerHexLong,
    ViewIntegerHexLongLong,
    ViewFloatFloat,
    ViewFloatDouble,
    ViewFloatLongDouble,
    ViewAddress
};

class MyDump : public HexDump
{
    Q_OBJECT
public:
    explicit MyDump(QWidget* parent = 0);
    QString paintContent(QPainter* painter, dsint rowBase, int rowOffset, int col, int x, int y, int w, int h);


    void hexAsciiSlot();
};

#endif // MYDUMP_H
