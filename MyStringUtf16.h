#ifndef MY_STRING_UTF16_H_
#define MY_STRING_UTF16_H_

#include "MyBuffer.h"

class MyStringUtf16 : public MyBuffer {
public:
  MyStringUtf16();
  ~MyStringUtf16();

  int Set(UCHAR* str);
  int Append(UCHAR* str);

  static int StrLen(UCHAR* str) {
    int len = 0;
    UCHAR* p = str;
    while (*p != 0) {
      len++;
      p++;
    }
    return len;
  }

};

#endif // MY_STRING_UTF16_H_