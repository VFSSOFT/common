#ifndef _MY_UTILS_H_
#define _MY_UTILS_H_

#include "MyCoreDef.h"

class MyUtils {
public:
  static UINT64 CurrentThreadId();

  static void RandomBytes(char* buf, int len);
};

#endif  // _MY_UTILS_H_

