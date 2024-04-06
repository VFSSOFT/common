#ifndef _MY_UTILS_H_
#define _MY_UTILS_H_

#include "MyCoreDef.h"

#define FILE_SIZE_FORMAT_IN_BYTES    0
#define FILE_SIZE_FOMRAT_IN_KB_MB_GB 1


class MyStringA;

class MyUtils {
public:
  static UINT64 CurrentThreadId();

  static void RandomBytes(char* buf, int len);

  static void FormatFileSize(UINT64 size, int fmt, MyStringA* ret);

private:
    static void TrimSuffixZerosAndDot(MyStringA* ret);
};

#endif  // _MY_UTILS_H_

