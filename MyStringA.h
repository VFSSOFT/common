#ifndef MY_STRING_A_H_
#define MY_STRING_A_H_

#include "MyBuffer.h"

class MyStringA : public MyBuffer {

public:
  MyStringA();
  ~MyStringA();

  int Set(const char* str);
  int Set(const char* str, int len);
  int Set(MyStringA* str);
  int Append(const char *str, int len=-1);
  int AppendInt(int intVal, int placeHolderLen=0);
  int SetInt64(UINT64 val, int placeHolderLen=0);
  int AppendInt64(UINT64 intVal, int placeHolderLen=0);

  int SetWithFormat(const char* fmt, ...);
  int AppendWithFormat(const char* fmt, ...);

    bool Equals(const char* str, int len, bool caseInsensitive) {
        if (this->Length() != len) return false;
        if (len == 0 || this->Length() == 0) return false;

        if (caseInsensitive) {
            for (int i = 0; i < len; i++) {
                if (tolower(str[i]) != tolower(this->Deref()[i])) {
                    return false;
                }
            }
            return true;
        } else {
            return strncmp(str, this->Deref(), len) == 0;
        }
    }

  BOOL Equals(const char* str, BOOL caseInsensitive=FALSE) {
    if (this->Length() == 0) return STR_IS_EMPTY(str);
    if (caseInsensitive) return strcmpi(this->Deref(), str) == 0;
    else                 return strcmp(this->Deref(), str) == 0;
  }

  UINT64 DerefAsInt64();
  int DerefAsInt();
  int ToLower();

  int HashCode();

    static int ConvertToInt(const char* str, int len) { return ConvertToInt64(str, len); }
    static INT64 ConvertToInt64(const char* str, int len);

    static bool IsLowercaseLetter(const char c);
    static bool IsUppercaseLetter(const char c);
    static bool IsLetter(const char c);
    static bool IsLowercaseLetter(const char* str, int len);
    static bool IsUppercaseLetter(const char* str, int len);
    static bool IsLetter(const char* str, int len);
    static bool IsDigit(const char c);
    static bool IsDigit(const char* str, int len);

#ifdef _WIN32
  int SetUnicode(const wchar_t* uniStr, int len);
  int Append(const wchar_t* uniStr, int len);
#endif 
};

#endif // MY_STRING_A_H_
