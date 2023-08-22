#include "pch.h"


#include "../MyChangeJournal.h"


TEST(MyChangeJournalTest, ReadChangeJournal) {
    int err = 0;
    MyChangeJournal journal;

    err = journal.Open(L'D');
    ASSERT_EQ(err, 0);

    USN_JOURNAL_DATA journalData;
    memset(&journalData, 0, sizeof(USN_JOURNAL_DATA));
    err = journal.Query(&journalData);
    ASSERT_EQ(err, 0);

    READ_USN_JOURNAL_DATA readData;
    readData.StartUsn = 0;
    readData.ReasonMask = 0xFFFFFFFF;
    readData.Timeout = 0;
    readData.BytesToWaitFor = 0;
    readData.UsnJournalID = journalData.UsnJournalID;
    readData.MinMajorVersion = journalData.MinSupportedMajorVersion;
    readData.MaxMajorVersion = journalData.MaxSupportedMajorVersion;

    MyArray<MyUsnRecord> records;


    DWORDLONG nextFileRefNum = 0;
    MFT_ENUM_DATA enumData;
    enumData.StartFileReferenceNumber = 0;
    enumData.LowUsn = 0;
    enumData.HighUsn = MAXLONGLONG;
    enumData.MinMajorVersion = journalData.MinSupportedMajorVersion;
    enumData.MaxMajorVersion = journalData.MaxSupportedMajorVersion;

    int i = 0;
    while (true) {
        i++;
        err = journal.Enum(&enumData, &records, &nextFileRefNum);
        ASSERT_EQ(err, 0);

        wchar_t buf[4096];
        for (int i = 0; i < records.Size(); i++) {
            MyUsnRecord* r = records.Get(i);
            //wsprintf(buf, L"[%d] USN=%I64d, major=%d, PFileRefNum=%I64d, FileRefNum=%I64d, %s\n", i, r->USN, r->MajorVersion, r->ParentFileRefNum, r->FileRefNum, r->FileName.Deref());
            //OutputDebugStringW(buf);
        }

        if (nextFileRefNum == 0 || records.Size() == 0) {
            int a = 0;
        }

        enumData.StartFileReferenceNumber = nextFileRefNum;
    }


    /*USN nextUSN;
    while (true) {
        err = journal.Read(&readData, &records, &nextUSN);
        ASSERT_EQ(err, 0);

        wchar_t buf[4096];
        for (int i = 0; i < records.Size(); i++) {
            MyUsnRecord* r = records.Get(i);
            wsprintf(buf, L"[%d] USN=%I64d, major=%d, minor=%d, %s\n", i, r->USN, r->MajorVersion, r->MinorVersion, r->FileName.Deref());
            OutputDebugStringW(buf);
        }

        readData.StartUsn = nextUSN;
    }*/
   

    

    int b = 0;
}
