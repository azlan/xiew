#ifndef STATIC_PE_H
#define STATIC_PE_H

#include "Static.Region.h"

namespace GleeBug
{
    class Pe
    {
    public:
        enum Error
        {
            ErrorOk = 0,
            ErrorDosHeaderRead = 1,
            ErrorDosHeaderMagic = 2,
            ErrorDosHeaderNtHeaderOffset = 3,
            ErrorDosHeaderNtHeaderOffsetOverlap = 4,
            ErrorAfterDosHeaderData = 5,
            ErrorNtSignatureRead = 6,
            ErrorNtSignatureMagic = 7,
            ErrorNtFileHeaderRead = 8,
            ErrorNtFileHeaderSizeOfOptionalHeaderOverlap = 9,
            ErrorNtFileHeaderUnsupportedMachine = 10,
            ErrorNtFileHeaderUnsupportedMachineOptionalHeaderRead = 11,
            ErrorNtFileHeaderUnsupportedMachineNtHeadersRegionSize = 12,
            ErrorNtOptionalHeaderRead = 13,
            ErrorNtOptionalHeaderMagic = 14,
            ErrorNtHeadersRegionSize = 15,
        };

        explicit Pe(void* mBuffer, uint32 size);

        void Clear();
        Error ParseHeaders();
        bool IsValidPe() const;
        bool IsPe64() const;

        const Region<IMAGE_DOS_HEADER> & GetDosHeader() const { return mDosHeader; }
        const Region<uint8> & GetAfterDosData() const { return mAfterDosData; }
        const Region<IMAGE_NT_HEADERS32> & GetNtHeaders32() const { return mNtHeaders32; }
        const Region<IMAGE_NT_HEADERS64> & GetNtHeaders64() const { return mNtHeaders64; }
        const Region<uint8> & GetAfterOptionalData() const { return mAfterOptionalData; }
        const Region<IMAGE_SECTION_HEADER> & GetSectionHeaders() const { return mSectionHeaders; }

    private:
        uint32 readData(uint32 size);
        void setupErrorMap();

        template<typename T>
        inline Region<T> readRegion(uint32 count = 1)
        {
            return Region<T>(&mData, readData(sizeof(T) * count), count);
        }

        std::unordered_map<Error, const char*> mErrorMap;
        const uint32 mFileSize;
        void *mBuffer;
        std::vector<uint8> mData;
        uint32 mOffset;

        Region<IMAGE_DOS_HEADER> mDosHeader;
        Region<uint8> mAfterDosData;
        Region<IMAGE_NT_HEADERS32> mNtHeaders32;
        Region<IMAGE_NT_HEADERS64> mNtHeaders64;
        Region<uint8> mAfterOptionalData;
        Region<IMAGE_SECTION_HEADER> mSectionHeaders;
    };
};

#endif //STATIC_PE_H
