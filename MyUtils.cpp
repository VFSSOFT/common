#ifndef _MY_UTILS_CPP_
#define _MY_UTILS_CPP_

#include "MyUtils.h"

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

#endif // _MY_UTILS_CPP_