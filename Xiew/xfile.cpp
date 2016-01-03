#include "xfile.h"

#include <QString>
#include <QMessageBox>


XFile::XFile(const QString fileName)
{
    mFile = new QFile(fileName);
    mFile->open(QIODevice::ReadOnly);
    mMapFile = mFile->map(0,mFile->size());

    parsePeFile();
}

XFile::~XFile()
{
 mFile->close();
 delete mFile;
}

bool XFile::parsePeFile()
{
    auto result = false;
    Pe pe(mMapFile,mFile->size());
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
        QMessageBox::critical(0, "Error", QString("Pe::ParseHeaders failed (%1)!\n").arg(parseError));
    }

    return result;
}
