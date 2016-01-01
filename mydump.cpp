#include "mydump.h"

MyDump::MyDump( QWidget* parent) : HexDump(parent)
{

}

#define MAX_LABEL_SIZE 256
#define MAX_MODULE_SIZE 256

QString MyDump::paintContent(QPainter *painter, dsint rowBase, int rowOffset, int col, int x, int y, int w, int h)
{
    // Reset byte offset when base address is reached
    //TODO
//    if(rowBase == 0 && mByteOffset != 0)
//        printDumpAt(mMemPage->getBase(), false, false);

    QString wStr = "";
    if(!col) //address
    {
        char label[MAX_LABEL_SIZE] = "";
        QString addrText = "";
        dsint cur_addr = rvaToVa((rowBase + rowOffset) * getBytePerRowCount() - mByteOffset);
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
        //mMemPage->read((byte_t*)&data, wRva, sizeof(duint));
        char modname[MAX_MODULE_SIZE] = "";
//        if(!DbgGetModuleAt(data, modname))
//            modname[0] = '\0';
        char label_text[MAX_LABEL_SIZE] = "";
//        if(DbgGetLabelAt(data, SEG_DEFAULT, label_text))
//            wStr = QString(modname) + "." + QString(label_text);
    }
    else //data
    {
        wStr = HexDump::paintContent(painter, rowBase, rowOffset, col, x, y, w, h);
    }
    return wStr;
}
