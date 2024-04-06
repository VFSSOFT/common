#ifndef _MY_UTILS_CPP_
#define _MY_UTILS_CPP_

#include "MyUtils.h"
#include "MyStringA.h"

#if _WIN32
#include <processthreadsapi.h>
#endif

UINT64 MyUtils::CurrentThreadId() {
  
#if _WIN32
    return (UINT64)GetCurrentThreadId();
#else
    assert(false);
    return -1;
#endif
}

void MyUtils::RandomBytes(char* buf, int len) {

#if _WIN32
    NTSTATUS status = BCryptGenRandom(
        NULL,
        (PUCHAR)buf,
        len,
        BCRYPT_USE_SYSTEM_PREFERRED_RNG
    );
    assert(BCRYPT_SUCCESS(status));
#else
    assert(false);
#endif
}

void MyUtils::FormatFileSize(UINT64 size, int fmt, MyStringA* ret) {
    MyStringA tmp;
    bool shouldInsertComma = false;
    char buf[32];

    ret->Reset();

    switch (fmt) {

    case FILE_SIZE_FORMAT_IN_BYTES:
        tmp.SetInt64(size);
        for (int i = 0; i < tmp.Length(); i++) {
            ret->AppendChar(tmp.CharAt(i));
            
            shouldInsertComma = ((tmp.Length() - i - 1) % 3 == 0) && (i + 1 != tmp.Length());
            if (shouldInsertComma) ret->AppendChar(',');
        }
        break;

    case FILE_SIZE_FOMRAT_IN_KB_MB_GB:
        
        if (size >= 1024LL * 1024 * 1024) {
            sprintf(buf, "%0.3f", (double)size / (1024LL * 1024 * 1024));
            ret->Append(buf);
            TrimSuffixZerosAndDot(ret);
            ret->Append(" GB");
        } else if (size >= 1024 * 1024) {
            sprintf(buf, "%0.3f", (double)size / (1024 * 1024));
            ret->Append(buf);
            TrimSuffixZerosAndDot(ret);
            ret->Append(" MB");
        } else if (size >= 1024) {
            sprintf(buf, "%0.3f", (double)size / 1024);
            ret->Append(buf);
            TrimSuffixZerosAndDot(ret);
            ret->Append(" KB");
        } else {
            ret->AppendInt((int)size);
            ret->Append(" B");
        }

        break;
    }
}

void MyUtils::TrimSuffixZerosAndDot(MyStringA* ret) {
    int newLen = ret->Length();
    
    while (newLen > 0) {
        char c = ret->CharAt(newLen - 1);
        if (c == '0' || c == '.') {
            newLen--;
        } else {
            break;
        }
        if (c == '.') {
            break;
        }
    }

    if (newLen < ret->Length()) {
        ret->SetLength(newLen);
    }
}

#endif // _MY_UTILS_CPP_