#ifndef _MY_DATA_PACKET_CPP_
#define _MY_DATA_PACKET_CPP_

#include "MyDataPacket.h"

MyDataPacket::MyDataPacket(int cap) {
  Reset();

  if (cap > 0) {
    m_Buffer.SetLength(cap);
  }
}
MyDataPacket::MyDataPacket(const char* data, int len) {
  Reset();

  if (len > 0) {
    m_Buffer.Set(data, len);
  }
}
void MyDataPacket::Reset() {
  m_ReadIndex = 0;
  m_LastErrorCode = 0;
  m_Buffer.Reset();
}

bool MyDataPacket::HasLine(const char* delimiter) {
  int index = m_Buffer.IndexOf(delimiter, strlen(delimiter), m_ReadIndex);
  return index >= 0;
}
const char* MyDataPacket::ReadLine(const char* delimiter) {
  int deliLen = strlen(delimiter);
  int index = m_Buffer.IndexOf(delimiter, deliLen, m_ReadIndex);
  if (index < 0) return "";
  int len = index - m_ReadIndex;
  m_RetBuf.Set(m_Buffer.Deref(m_ReadIndex), len);
  m_ReadIndex = index + deliLen;
  return m_RetBuf.Deref();
}

int MyDataPacket::PeekByte(int* val) {
    int err = 0;
    if (this->Available() < 1) {
        return LastError(MY_ERR_NOT_ENOUGH_DATA, "No so much data to read");
    }
    *val = *m_Buffer.Deref(m_ReadIndex);
    return 0;
}
int MyDataPacket::ReadBytes(int len, MyBuffer* buf) {
    int err = 0;
    if (err = buf->SetLength(len)) return err;
    return this->ReadBytes(len, buf->Deref());
}
int MyDataPacket::ReadBytes(int len, char* buf) {
    int err = 0;
    if (this->Available() < len) {
        return LastError(MY_ERR_NOT_ENOUGH_DATA, "No so much data to read");
    }
    memcpy(buf, m_Buffer.Deref(m_ReadIndex), len);
    m_ReadIndex += len;
    return 0;
}
int MyDataPacket::ReadNum(int bytesLen, UINT64* val) {
    // Little endian
    assert(bytesLen >= 1 && bytesLen <= 8);
    int err = 0;
    UINT64 ret = 0;
    if (this->Available() < bytesLen)
        return LastError(MY_ERR_NOT_ENOUGH_DATA, "No so much data to read");

    char buf[8];
    memcpy(buf, m_Buffer.Deref(m_ReadIndex), bytesLen);
    m_ReadIndex += bytesLen;

    for (int i = 0; i < bytesLen; i++) {
        ret += ((UINT64)buf[i]) << i;
    }
    *val = ret;
    return 0;
}
int MyDataPacket::ReadInt8(int* val) {
    int err = 0;
    UINT64 num;
    if (err = ReadNum(1, &num)) return err;
    *val = num & 0x00FF;
    return 0;
}
int MyDataPacket::ReadInt16(int* val) {
    int err = 0;
    UINT64 num;
    if (err = ReadNum(2, &num)) return err;
    *val = num & 0x00FFFF;
    return 0;
}
int MyDataPacket::ReadInt32(int* val) {
    int err = 0;
    UINT64 num;
    if (err = ReadNum(4, &num)) return err;
    *val = num & 0x00FFFFFFFF;
    return 0;
}
int MyDataPacket::ReadInt64(UINT64* val) {
    return ReadNum(8, val);
}
int MyDataPacket::ReadL8StringA(MyStringA* str) {
    int err = 0;
    int len = 0;
    str->Reset();
    if (err = this->ReadInt8(&len)) return err;
    if (len > 0) {
        if (err = this->ReadBytes(len, str)) return err;
    }
    return 0;
}
int MyDataPacket::ReadL16StringA(MyStringA* str) {
    int err = 0;
    int len = 0;
    str->Reset();
    if (err = this->ReadInt16(&len)) return err;
    if (len > 0) {
        if (err = this->ReadBytes(len, str)) return err;
    }
    return 0;
}
int MyDataPacket::ReadL16StringW(MyStringW* str) {
    int err = 0;
    int len = 0;
    str->Reset();
    if (err = this->ReadInt16(&len)) return err;
    if (len > 0) {
        if (err = str->SetLength(len)) return err;
        if (err = this->ReadBytes(len * 2, (char*)str->Deref())) return err;
    }
    return 0;
}

int MyDataPacket::Write(const char* data, int len) {
  return m_Buffer.Append(data, len);
}
int MyDataPacket::WriteNum(UINT64 val, int bytesLen) {
    assert(bytesLen > 0 && bytesLen <= 8);
    char buf[8];
    int i = 0;
    memset(buf, 0, 8);
    while (val > 0) {
        buf[i] = (char)(val & 0xFF);
        val = val >> 8;
        i++;
    }
    return m_Buffer.Append(buf, bytesLen);
}
int MyDataPacket::WriteInt8(int val) {
    return WriteNum(val, 1);
}
int MyDataPacket::WriteInt16(int val) {
    return WriteNum(val, 2);
}
int MyDataPacket::WriteInt32(int val) {
    return WriteNum(val, 4);
}
int MyDataPacket::WriteInt64(UINT64 val) {
    return WriteNum(val, 8);
}
int MyDataPacket::WriteL8String(const char* str, int len) {
    int err = 0;
    if (err = WriteInt8(len)) return err;
    if (err = Write(str, len)) return err;
    return 0;
}

int MyDataPacket::WriteL16StringA(MyStringA* str) {
    int err = 0;
    if (err = WriteInt16(str->Length())) return err;
    if (err = Write(str->Deref(), str->Length())) return err;
    return 0;
}
int MyDataPacket::WriteL16StringW(MyStringW* str) {
    int err = 0;
    if (err = WriteInt16(str->Length())) return err;
    if (err = Write((char*)str->Deref(), str->Length() * 2)) return err;
    return 0;
}

UINT64 MyDataPacket::Hex2Decimal(const char* str, int len) {
  if (len < 0) {
    len = str == NULL ? 0 : strlen(str);
  }

  UINT64 ret = 0;
  for (int i = 0; i < len; i++) {
    char c = str[i];
    if (c >= '0' && c <= '9') {
      ret = ret * 16 + (c - '0');
    } else if (c >= 'a' && c <= 'f') {
      ret = ret * 16 + (c - 'a' + 10);
    } else if (c >= 'A' && c <= 'F') {
      ret = ret * 16 + (c - 'A' + 10);
    } else {
      return -1;
    }
  }
  return ret;
}



#endif // _MY_DATA_PACKET_CPP_