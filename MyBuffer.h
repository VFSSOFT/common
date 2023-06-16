#ifndef MY_BUFFER_H_
#define MY_BUFFER_H_

#include "MyCoreDef.h"

// Always allocate extra 2 bytes for string types
#define MY_BUFFER_RESERVED_BYTES 2

class MyBuffer {
public:
  MyBuffer();
  virtual ~MyBuffer();

  MyBuffer& operator=(const MyBuffer& other) {
    if (this == &other) return *this;

    this->Reset();
    this->Set(other.m_Buffer, other.m_Length);
    return *this;
  }

  bool Empty() { return m_Length == 0; }

  int SetCharAt(int index, char c);
  int CharAt(int index);

  int Set(const char* data, int len);
  int Set(int index, const char* data, int len);
  int Append(const char* data, int len);
  int AppendChar(char c);
  int InsertChar(char c, int index);
  int Insert(const char* data, int len, int index);
  int SetLength(int len);
  void Reset();
  int Delete(int start, int len);
    int Sub(int startIndex);
  int Sub(int startIndex, int endIndex);

  inline char* Deref(int off=0) { return m_Length == 0 ? (char*)"" : m_Buffer + off; }
  inline const char* DerefConst(int off=0) { return Deref(off); }
  inline int Length() { return m_Length; }

  int IndexOf(const char* ptr, int len, int startIndex=0);
  int LastIndexOf(const char* ptr, int len);
  int IndexOf(const char c, int startIndex=0);
  int LastIndexOf(const char c);
  int LastIndexOf(const char c, int startIndex, int endIndex);
  void Replace(const char toReplace, const char replaceWith);

  BOOL StartWith(const char* ptr, int len);
  BOOL EndWith(const char* ptr, int len);
  BOOL StartWith(const char c);
  BOOL EndWith(const char c);
  bool Contains(const char* ptr, int len);
  bool Equals(const char* b, int bLen) { return Equals(Deref(), Length(), b, bLen); }
  static bool Equals(const char* b1, int b1Len, const char* b2, int b2Len);

private:
  int ExtendCap(int cap);
  inline bool HeapMemUsed() { return m_Length > 62; }

private:
  char  m_StackBuf[64];
  char* m_Buffer;
  int   m_Cap;
  int   m_Length;
};

#endif // MY_BUFFER_H_