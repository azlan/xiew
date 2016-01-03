#ifndef FILE_H
#define FILE_H

#include "Static.Pe.h"
#include <QFile>

using namespace GleeBug;

class QString;


class XFile
{
public:
    XFile(const QString fileName);
    ~XFile();
    bool parsePeFile();

    PIMAGE_FILE_HEADER mFileHeader;
    PIMAGE_OPTIONAL_HEADER mOptionalHeader;
    PIMAGE_SECTION_HEADER mSectionHeader;
    void * getBase() { return mMapFile; };
    int getSize() { return mFile->size(); };


private:
//    bool open();
    wchar_t *mFileName;

    std::vector<uint8> mDiskData;

     QFile *mFile;
     uchar *mMapFile;

};

#endif // FILE_H
