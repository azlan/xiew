#ifndef XFILE_H
#define XFILE_H

#include <QFile>
#include <windows.h>

class XFile
{
public:
    XFile(const QString fileName);
    ~XFile();
    bool parsePeFile();

    PIMAGE_FILE_HEADER mFileHeader;
    PIMAGE_OPTIONAL_HEADER mOptionalHeader;
    PIMAGE_SECTION_HEADER mSectionHeader;
    void * getBase();
    int getSize();
    uint mCurrentOffset;
    bool mIsPE;

private:
    QFile mFile;
    uchar *mMapFile;

};

#endif // XFILE_H
