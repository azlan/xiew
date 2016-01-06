#include "mydump.h"

#define MAX_LABEL_SIZE 256
#define MAX_MODULE_SIZE 256

MyDump::MyDump( QWidget* parent) : HexDump(parent)
{
    hexAsciiSlot();
    printDumpAt(mMemPage->getBase());
    setShowHeader(false); //hide header
}

MyDump::~MyDump()
{

}

void MyDump::hexAsciiSlot()
{
    Config()->setUint("HexDump", "DefaultView", (duint)ViewHexAscii);
    int charwidth = getCharWidth();
    ColumnDescriptor_t wColDesc;
    DataDescriptor_t dDesc;

    wColDesc.isData = true; //hex byte
    wColDesc.itemCount = 16;
    wColDesc.separator = 4;
    dDesc.itemSize = Byte;
    dDesc.byteMode = HexByte;
    wColDesc.data = dDesc;
    appendResetDescriptor(8 + charwidth * 47, "Hex", false, wColDesc);

    wColDesc.isData = true; //ascii byte
    wColDesc.itemCount = 16;
    wColDesc.separator = 0;
    dDesc.itemSize = Byte;
    dDesc.byteMode = AsciiByte;
    wColDesc.data = dDesc;
    appendDescriptor(8 + charwidth * 16, "ASCII", false, wColDesc);

    wColDesc.isData = false; //empty column
    wColDesc.itemCount = 0;
    wColDesc.separator = 0;
    dDesc.itemSize = Byte;
    dDesc.byteMode = AsciiByte;
    wColDesc.data = dDesc;
    appendDescriptor(0, "", false, wColDesc);

    reloadData();
}

void MyDump::keyPressEvent(QKeyEvent *event)
{
    const auto wKey = event->key();
    const auto wBytePerRow = getBytePerRowCount();
    const auto wTotalRowCount = getRowCount();
    const auto wSize = mMemPage->getSize();
    const auto wViewableRow = getViewableRowsCount() - 1;
    const auto wCurrentTableOffset = getTableOffset();
    auto  wSelectedOffset = getInitialSelection();

    if(wKey == Qt::Key_Right)
    {
        if (++wSelectedOffset >= wSize)
            wSelectedOffset = wSize - 1;

        setSingleSelection(wSelectedOffset);
    }
    else if(wKey == Qt::Key_Left)
    {
        if (wSelectedOffset != 0)
        {
            setSingleSelection(--wSelectedOffset);
        }
    }
    else if(wKey == Qt::Key_Up)
    {
        if (wSelectedOffset - wBytePerRow >= 0)
        {
            wSelectedOffset -= wBytePerRow;
            setSingleSelection(wSelectedOffset);

            auto currentRow = wSelectedOffset / wBytePerRow;
            if (currentRow == (wCurrentTableOffset - 1))
                verticalScrollBar()->triggerAction(QAbstractSlider::SliderSingleStepSub);
       }
    }
    else if(wKey == Qt::Key_Down)
    {
        wSelectedOffset += wBytePerRow;
        if (wSelectedOffset > wSize)
            wSelectedOffset = wSize - 1;

        setSingleSelection(wSelectedOffset);

        auto lastRowOffset = (wCurrentTableOffset * wBytePerRow) + (wViewableRow * wBytePerRow);

        if (wSelectedOffset > lastRowOffset)
            verticalScrollBar()->triggerAction(QAbstractSlider::SliderSingleStepAdd);
    }
    else if(wKey == Qt::Key_PageUp)
    {
        if (wSelectedOffset - (wViewableRow * wBytePerRow) >= 0)
        {
            wSelectedOffset -= ((wViewableRow + 1) * wBytePerRow);
            setSingleSelection(wSelectedOffset);

            auto currentRow = wSelectedOffset / wBytePerRow;
            if (currentRow < wCurrentTableOffset)
                verticalScrollBar()->triggerAction(QAbstractSlider::SliderPageStepSub);
       } else
        {
            wSelectedOffset = 0;
            setSingleSelection(wSelectedOffset);
            setTableOffset(0);
        }
    }
    else if(wKey == Qt::Key_PageDown)
    {
        auto viewableRow = getViewableRowsCount();
        auto currentRow  = wSelectedOffset / wBytePerRow;
        if (currentRow < wTotalRowCount)
        {
            wSelectedOffset += viewableRow * wBytePerRow;
            if (wSelectedOffset >= wSize)
                wSelectedOffset = wSize - 1;

            setSingleSelection(wSelectedOffset);
            verticalScrollBar()->triggerAction(QAbstractSlider::SliderPageStepAdd);
        }
    }
    else if(wKey == Qt::Key_Return || wKey == Qt::Key_Enter) //user pressed enter
        emit enterPressedSignal();


    emit currentOffsetSignal(wSelectedOffset);
    emit currentTableOffsetSignal(getTableOffset());
    this->viewport()->update();
    emit keyPressSignal(wKey);
}

bool MyDump::event(QEvent *event)
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

QString MyDump::paintContent(QPainter *painter, dsint rowBase, int rowOffset, int col, int x, int y, int w, int h)
{
    // Reset byte offset when base address is reached
    if(rowBase == 0 && mByteOffset != 0)
        printDumpAt(mMemPage->getBase(), false, false);

    QString wStr = "";
    if(!col) //address
    {
        char label[MAX_LABEL_SIZE] = "";
        QString addrText = "";
        //dsint cur_addr = rvaToVa((rowBase + rowOffset) * getBytePerRowCount() - mByteOffset);
        dsint cur_addr = ((rowBase + rowOffset) * getBytePerRowCount() - mByteOffset);
        if(mRvaDisplayEnabled) //RVA display
        {
            dsint rva = cur_addr - mRvaDisplayBase;
            if(rva == 0)
            {
#ifdef _WIN64
                addrText = "$ ==>            ";
#else
                addrText = "$ ==>    ";
#endif //_WIN64
            }
            else if(rva > 0)
            {
#ifdef _WIN64
                addrText = "$+" + QString("%1").arg(rva, -15, 16, QChar(' ')).toUpper();
#else
                addrText = "$+" + QString("%1").arg(rva, -7, 16, QChar(' ')).toUpper();
#endif //_WIN64
            }
            else if(rva < 0)
            {
#ifdef _WIN64
                addrText = "$-" + QString("%1").arg(-rva, -15, 16, QChar(' ')).toUpper();
#else
                addrText = "$-" + QString("%1").arg(-rva, -7, 16, QChar(' ')).toUpper();
#endif //_WIN64
            }
        }
        addrText += QString("%1").arg(cur_addr, sizeof(dsint) * 2, 16, QChar('0')).toUpper();
//        if(DbgGetLabelAt(cur_addr, SEG_DEFAULT, label)) //has label
//        {
//            char module[MAX_MODULE_SIZE] = "";
//            if(DbgGetModuleAt(cur_addr, module) && !QString(label).startsWith("JMP.&"))
//                addrText += " <" + QString(module) + "." + QString(label) + ">";
//            else
//                addrText += " <" + QString(label) + ">";
//        }
//        else
            *label = 0;
        if(*label) //label
        {
            QColor background = ConfigColor("HexDumpLabelBackgroundColor");
            if(background.alpha())
                painter->fillRect(QRect(x, y, w, h), QBrush(background)); //fill bookmark color
            painter->setPen(ConfigColor("HexDumpLabelColor")); //TODO: config
        }
        else
        {
            QColor background = ConfigColor("HexDumpAddressBackgroundColor");
            if(background.alpha())
                painter->fillRect(QRect(x, y, w, h), QBrush(background)); //fill bookmark color
            painter->setPen(ConfigColor("HexDumpAddressColor")); //TODO: config
        }
        painter->drawText(QRect(x + 4, y , w - 4 , h), Qt::AlignVCenter | Qt::AlignLeft, addrText);
    }
    else if(col && mDescriptor.at(col - 1).isData == false && mDescriptor.at(col - 1).itemCount == 1) //print comments
    {
        duint data = 0;
        dsint wRva = (rowBase + rowOffset) * getBytePerRowCount() - mByteOffset;
        //TODO
        mMemPage->read((byte_t*)&data, wRva, sizeof(duint));
//        char modname[MAX_MODULE_SIZE] = "";
//        if(!DbgGetModuleAt(data, modname))
//            modname[0] = '\0';
//        char label_text[MAX_LABEL_SIZE] = "";
//        if(DbgGetLabelAt(data, SEG_DEFAULT, label_text))
//            wStr = QString(modname) + "." + QString(label_text);
    }
    else //data
    {
        wStr = HexDump::paintContent(painter, rowBase, rowOffset, col, x, y, w, h);
    }
    return wStr;
}
