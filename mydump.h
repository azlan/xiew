#ifndef MYDUMP_H
#define MYDUMP_H

#include "HexDump.h"

class MyDump : public HexDump
{
    Q_OBJECT
public:
    explicit MyDump(QWidget* parent = 0);
    QString paintContent(QPainter* painter, dsint rowBase, int rowOffset, int col, int x, int y, int w, int h);


};

#endif // MYDUMP_H
