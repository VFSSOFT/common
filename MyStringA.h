#ifndef MY_STRING_A_H_
#define MY_STRING_A_H_

#include "MyBuffer.h"

class MyStringA : public MyBuffer {

public:
  MyStringA();
  ~MyStringA();

  int Set(const char* str);
  int Set(const char* str, int len);
  int Append(const char *str, int len=-1);
  int AppendInt(int intVal, int placeHolderLen=0);
  int SetInt64(UINT64 val, int placeHolderLen=0);
  int AppendInt64(UINT64 intVal, int placeHolderLen=0);
  BOOL Equals(const char* str, BOOL caseInsensitive=FALSE) {
    if (this->Length() == 0) return str == NULL;
    if (caseInsensitive) return strcmpi(this->Deref(), str) == 0;
    else                 return strcmp(this->Deref(), str) == 0;
  }

  int DerefAsInt64();
  int DerefAsInt();
  int ToLower();

  int HashCode();

#ifdef _WIN32
  int SetUnicode(const wchar_t* uniStr, int len);
  int Append(const wchar_t* uniStr, int len);
#endif 
};

#endif // MY_STRING_A_H_