#ifndef MY_STRING_W_H_
#define MY_STRING_W_H_

#include "MyCoreDef.h"

#ifdef _WIN32


class MyStringW {
public:
  MyStringW(int cap=62);
  ~MyStringW();

  int Set(const WCHAR* str);
  int Set(const WCHAR* str, int len);
  int Set(MyStringW* wstr);
  int SetUtf8(const char* str, int len);
  int Append(const WCHAR* str, int len=-1);
  int AppendChar(WCHAR c);
  int SetLength(int len); // set wstring length
  void Reset();

  int IndexOf(const WCHAR* str, int startIndex=0);
  int LastIndexOf(const WCHAR* str);
  int IndexOf(const WCHAR c, int startIndex=0);
  int LastIndexOf(const WCHAR c);
  void Replace(const WCHAR toReplace, const WCHAR replaceWith);
  int Replace(const WCHAR* toReplace, const WCHAR* replaceWith);

  int RemoveLast() { return SetLength(m_Length - 1); }

  inline WCHAR* Deref(int off=0) { return m_Buffer + off; }
  inline int Length()   { return m_Length; }

  BOOL StartWith(const WCHAR* str);
  BOOL EndWith(const WCHAR* str);
  BOOL StartWith(const WCHAR c);
  BOOL EndWith(const WCHAR c);
  BOOL Equals(const WCHAR* str);

  int HashCode();

    static int WStrNCmp(const wchar_t* str1, int len1, const wchar_t* str2, int len2, bool caseSensitive);
    static int JoinPath(const wchar_t* p1, int len1, const wchar_t* p2, int len2, MyStringW* ret);
    static int JoinPath(const wchar_t* p1, int len1, const wchar_t* p2, int len2, const wchar_t* name, int lenName, MyStringW* ret);
    static int SplitPath(const wchar_t* path, int pathLen, MyStringW* dir, MyStringW* name);

private:
  int EnsureCap(int cap);
private:
  WCHAR* m_Buffer;
  int    m_Length;
  int    m_Cap;
};

#endif // _WIN32

#endif // MY_STRING_W_H_