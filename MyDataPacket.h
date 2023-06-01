#ifndef _MY_DATA_PACKET_H_
#define _MY_DATA_PACKET_H_

#include "MyCoreDef.h"
#include "MyStringA.h"
#include "MyBuffer.h"
#include "MyStringW.h"

class MyDataPacket {
public:

  MyDataPacket(int cap=0);
  MyDataPacket(const char* data, int len);

  bool HasLine(const char* delimiter="\r\n");
  const char* ReadLine(const char* delimiter="\r\n");
  
  inline int Available() { return m_Buffer.Length() - m_ReadIndex; }
  int ReadOffset() { return m_ReadIndex; }

  int PeekByte(int* val);
  int ReadBytes(int len, MyBuffer* buf);
  int ReadBytes(int len, char* buf);
  int ReadNum(int bytesLen, UINT64* val);
  int ReadInt8(int* val);
  int ReadInt16(int* val);
  int ReadInt32(int* val);
  int ReadInt64(UINT64* val);
  int ReadL8StringA(MyStringA* str);
  int ReadL16StringA(MyStringA* str);
  int ReadL16StringW(MyStringW* str);

  int Write(const char* data, int len);
  int WriteNum(UINT64 val, int bytesLen);
  int WriteInt8(int val);
  int WriteInt16(int val);
  int WriteInt32(int val);
  int WriteInt64(UINT64 val);
  int WriteL8String(const char* str, int len);
  int WriteL16StringA(MyStringA* str);
  int WriteL16StringW(MyStringW* str);

  void Reset();

  char* Deref() { return m_Buffer.Deref(); }
  int   DerefLength() { return m_Buffer.Length(); }

public: // public static functions goes here
  static UINT64 Hex2Decimal(const char* str, int len=-1);

private:
  MyBuffer m_Buffer;
  int      m_ReadIndex;

  MyBuffer m_RetBuf;

  MY_LAST_ERROR_DECL;
};


#endif // _MY_DATA_PACKET_H_