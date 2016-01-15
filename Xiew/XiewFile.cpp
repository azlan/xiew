#include "XiewFile.h"
#include "Static.Pe.h"


//#include <QString>
#include <QMessageBox>

using namespace GleeBug;

XiewFile::XiewFile(const QString fileName):
    mFile(fileName),
    mIsPE(false),
    mCurrentOffset(0),
    mCurrentTableOffset(0)
{
    //mFile = new QFile(fileName);
    mFile.open(QIODevice::ReadOnly);

    if (mFile.isOpen())
    {
        mMapFile = mFile.map(0,getSize());
        mIsPE = parsePeFile();
    }
    else
    {
        QMessageBox::critical(0, "Error", QString("XFile::XFile failed (%1) - (%2)!\n").arg(fileName).arg(mFile.errorString()));
    }
}

XiewFile::~XiewFile()
{
}

bool XiewFile::parsePeFile()
{
    auto result = false;
    Pe pe(mMapFile,mFile.size());
    auto parseError = pe.ParseHeaders();
    if (parseError == Pe::ErrorOk)
    {
        result = true;
        auto idh = pe.GetDosHeader();
        auto afterDosData = pe.GetAfterDosData();

#ifdef _WIN64
        auto inth = pe.GetNtHeaders64();
#else //x32
        auto inth = pe.GetNtHeaders32();
#endif //_WIN64

        mFileHeader = &inth->FileHeader;
        mOptionalHeader = &inth->OptionalHeader;
        mSectionHeader = pe.GetSectionHeaders().Data();
    }
    else
    {
        //QMessageBox::critical(0, "Error", QString("Pe::ParseHeaders failed (%1)!\n").arg(parseError));
    }

    return result;
}

void *XiewFile::getBase()
{
    return mMapFile;
}

int XiewFile::getSize()
{
    return mFile.size();
}

uint XiewFile::getCurrentTableOffset() const
{
    return mCurrentTableOffset;
}

void XiewFile::setCurrentTableOffset(const uint &currentTableOffset)
{
    mCurrentTableOffset = currentTableOffset;
}

QString XiewFile::getFilename() const
{
    return mFile.fileName();
}

bool XiewFile::isOpened()
{
    return mFile.isOpen();
}

uint XiewFile::getCurrentOffset() const
{
    return mCurrentOffset;
}

void XiewFile::setCurrentOffset(const uint &currentOffset)
{
    mCurrentOffset = currentOffset;
}
