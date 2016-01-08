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
    uint mCurrentTableOffset;
    bool mIsPE;

    uint getCurrentOffset() const;
    void setCurrentOffset(const uint &currentOffset);

    uint getCurrentTableOffset() const;
    void setCurrentTableOffset(const uint &currentTableOffset);

private:
    QFile mFile;
    uchar *mMapFile;

};

#endif // XFILE_H
