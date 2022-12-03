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
  return -1;
#endif
}

#endif // _MY_UTILS_CPP_