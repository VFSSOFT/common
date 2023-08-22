#ifndef _MY_CHANGE_JOURNAL_CPP_
#define _MY_CHANGE_JOURNAL_CPP_

#include "MyChangeJournal.h"
#include "MyWin.h"


int MyUsnRecord::From(PUSN_RECORD r) {
    PUSN_RECORD_V2 v2 = NULL;
    PUSN_RECORD_V3 v3 = NULL;
    PUSN_RECORD_V4 v4 = NULL;

    MajorVersion = r->MajorVersion;
    MinorVersion = r->MinorVersion;
    
    switch (r->MajorVersion) {
    case 2:
        v2               = (PUSN_RECORD_V2)r;
        FileRefNum       = v2->FileReferenceNumber;
        ParentFileRefNum = v2->ParentFileReferenceNumber;
        USN              = v2->Usn;
        Timestamp        = v2->TimeStamp.QuadPart;
        Reason           = v2->Reason;
        SourceInfo       = v2->SourceInfo;
        SecurityId       = v2->SecurityId;
        FileName.Set(v2->FileName, v2->FileNameLength / 2);
        break;

    case 3:
        v3               = (PUSN_RECORD_V3)r;
        FileRefNum       = *(UINT64*)v3->FileReferenceNumber.Identifier;
        ParentFileRefNum = *(UINT64*)v3->ParentFileReferenceNumber.Identifier;
        USN              = v3->Usn;
        Timestamp        = v3->TimeStamp.QuadPart;
        Reason           = v3->Reason;
        SourceInfo       = v3->SourceInfo;
        SecurityId       = v3->SecurityId;
        FileName.Set(v3->FileName, v3->FileNameLength / 2);
        
        break;

    case 4:
        assert(false); // TODO:
        v4               = (PUSN_RECORD_V4)r;
        FileRefNum       = *(UINT64*)v4->FileReferenceNumber.Identifier;
        ParentFileRefNum = *(UINT64*)v4->ParentFileReferenceNumber.Identifier;
        USN              = v4->Usn;
        Reason           = v4->Reason;
        SourceInfo       = v4->SourceInfo;
        // TODO:
        break;

    default:
        return -1;
    }
    return 0;
}

MyChangeJournal::MyChangeJournal() : m_VolHandle(NULL) {
    INIT_LAST_ERROR;
}
MyChangeJournal::~MyChangeJournal() {

}

int MyChangeJournal::Open(wchar_t volName) {
    int err = 0;
    HANDLE handle;
    
    MyStringW fileName;
    fileName.Set(L"\\\\.\\");
    fileName.AppendChar(volName);
    fileName.AppendChar(L':');

    handle = CreateFile(
        fileName.Deref(),
        GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL,
        OPEN_EXISTING,
        0,
        NULL
    );

    if (handle == INVALID_HANDLE_VALUE) {
        err = GetLastError();
        MyWin::GetSysLastErrorMessage(&m_LastErrorMessage, err);
        return err;
    }

    m_VolHandle = handle;
    return 0;
}
int MyChangeJournal::Close() {
    if (m_VolHandle) {
        CloseHandle(m_VolHandle);
    }
    m_VolHandle = NULL;
    return 0;
}


int MyChangeJournal::Query(USN_JOURNAL_DATA* journalData) {
    int err = 0;
    DWORD dwBytes;

    BOOL suc = DeviceIoControl(
        m_VolHandle,
        FSCTL_QUERY_USN_JOURNAL,
        NULL,
        0,
        journalData,
        sizeof(USN_JOURNAL_DATA),
        &dwBytes,
        NULL
    );
    if (!suc) {
        err = GetLastError();
        MyWin::GetSysLastErrorMessage(&m_LastErrorMessage, err);
        return err;
    }

    return 0;
}

int MyChangeJournal::Read(READ_USN_JOURNAL_DATA* readData, MyArray<MyUsnRecord>* records, USN* nextUsn) {
    int err = 0;
    CHAR buffer[4096];
    DWORD dwBytes;

    /*
     * Retrieves the set of update sequence number (USN) change journal records
     * between two specified USN values.
     */

    BOOL suc = DeviceIoControl(
        m_VolHandle,
        FSCTL_READ_USN_JOURNAL,
        readData,
        sizeof(READ_USN_JOURNAL_DATA),
        buffer,
        4096,
        &dwBytes,
        NULL
    );
    if (!suc) {
        err = GetLastError();
        if (err != ERROR_HANDLE_EOF) {
            MyWin::GetSysLastErrorMessage(&m_LastErrorMessage, err);
            return err;
        }
    }

    int retBytes = dwBytes - sizeof(USN);

    // find the first record
    PUSN_RECORD record = (PUSN_RECORD)(((PUCHAR)buffer) + sizeof(USN));

    while (retBytes > 0) {
        MyUsnRecord* r = records->AddNew();
        r->From(record);

        retBytes -= record->RecordLength;

        // Find the next record
        record = (PUSN_RECORD)(((PCHAR)record) + record->RecordLength);
    }

    // Update the starting USN for next call
    *nextUsn = *(USN*)buffer;
    if (err == ERROR_HANDLE_EOF) {
        *nextUsn = 0;
    }

    return 0;
}

int MyChangeJournal::Enum(MFT_ENUM_DATA* enumData, MyArray<MyUsnRecord>* records, DWORDLONG* nextStartFileReferenceNumber) {
    int err = 0;
    CHAR buffer[4096];
    DWORD dwBytes;

    /*
     * Enumerates the update sequence number (USN) data between two specified boundaries 
     * to obtain master file table (MFT) records.
     */

    BOOL suc = DeviceIoControl(
        m_VolHandle,
        FSCTL_ENUM_USN_DATA,
        enumData,
        sizeof(MFT_ENUM_DATA),
        buffer,
        4096,
        &dwBytes,
        NULL
    );
    if (!suc) {
        err = GetLastError();
        if (err != ERROR_HANDLE_EOF) {
            MyWin::GetSysLastErrorMessage(&m_LastErrorMessage, err);
            return err;
        }
    }

    int retBytes = dwBytes - sizeof(USN);

    // find the first record
    PUSN_RECORD record = (PUSN_RECORD)(((PUCHAR)buffer) + sizeof(USN));

    while (retBytes > 0) {
        MyUsnRecord* r = records->AddNew();
        r->From(record);

        retBytes -= record->RecordLength;

        // Find the next record
        record = (PUSN_RECORD)(((PCHAR)record) + record->RecordLength);
    }

    // Update the starting USN for next call
    *nextStartFileReferenceNumber = *(DWORDLONG*)buffer;
    if (err == ERROR_HANDLE_EOF) {
        *nextStartFileReferenceNumber = 0;
    }

    return 0;
}

#endif // _MY_CHANGE_JOURNAL_CPP_
