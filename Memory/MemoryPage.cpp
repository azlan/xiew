#include <QFile>
#include "MemoryPage.h"

MemoryPage::MemoryPage(duint parBase, duint parSize, QObject* parent) : QObject(parent), mBase(0), mSize(0)
{
    Q_UNUSED(parBase);
    Q_UNUSED(parSize);
}

MemoryPage::~MemoryPage()
{
}

bool MemoryPage::read(void* parDest, duint parRVA, duint parSize) const
{
    std::memcpy(parDest, reinterpret_cast<void*>(parRVA + mBase), parSize);
    return true;
}

bool MemoryPage::read(byte_t* parDest, duint parRVA, duint parSize) const
{
    return read(reinterpret_cast<void*>(parDest), parRVA, parSize);
}

bool MemoryPage::write(const void* parDest, duint parRVA, duint parSize)
{
    //bool ret = DbgFunctions()->MemPatch(mBase + parRVA, reinterpret_cast<const unsigned char*>(parDest), parSize);
    //GuiUpdatePatches();
    //return ret;
    return false;
}

bool MemoryPage::write(const byte_t* parDest, duint parRVA, duint parSize)
{
    //return write(reinterpret_cast<const void*>(parDest), parRVA, parSize);
    return false;
}

duint MemoryPage::getSize() const
{
    return mSize;
}

duint MemoryPage::getBase() const
{
    return mBase;
}

duint MemoryPage::va(dsint rva) const
{
    return mBase + rva;
}

void MemoryPage::setAttributes(duint base, duint size)
{
    mBase = base;
    mSize = size;
}
