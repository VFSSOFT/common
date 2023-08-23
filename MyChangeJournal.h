#ifndef _MY_CHANGE_JOURNAL_H_
#define _MY_CHANGE_JOURNAL_H_

#include "MyStringA.h"
#include "MyStringW.h"
#include "MyArray.h"


class MyUsnRecord {
public:
    MyUsnRecord() {
        Reset();
    }
    int From(PUSN_RECORD r);

    void Reset() {
        MajorVersion     = 0;
        MinorVersion     = 0;
        FileRefNum       = 0;
        ParentFileRefNum = 0;
        USN              = 0;
        Timestamp        = 0;
        Reason           = 0;
        SourceInfo       = 0;
        SecurityId       = 0;
        FileAttributes   = 0;
        FileName.Reset();
    }

    int       MajorVersion;
    int       MinorVersion;
    UINT64    FileRefNum;
    UINT64    ParentFileRefNum;
    USN       USN;
    UINT64    Timestamp;
    INT64     Reason;
    INT64     SourceInfo;
    INT64     SecurityId;
    UINT64    FileAttributes;
    MyStringW FileName;
};

class MyChangeJournal {
public:
    MyChangeJournal();
    ~MyChangeJournal();

    int Open(wchar_t volName);
    int Close();

    int Query(USN_JOURNAL_DATA_V2* journalData);
    int Read(READ_USN_JOURNAL_DATA* readData, MyArray<MyUsnRecord>* records, USN* nextUsn);
    int Enum(MFT_ENUM_DATA* enumData, MyArray<MyUsnRecord>* records, DWORDLONG* nextStartFileReferenceNumber);

private:

    HANDLE m_VolHandle;


    MY_LAST_ERROR_DECL;
};

#endif // _MY_CHANGE_JOURNAL_H_
