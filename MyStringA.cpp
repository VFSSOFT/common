#include "MyStringA.h"


MyStringA::MyStringA() : MyBuffer() {}
MyStringA::~MyStringA() {}

int MyStringA::Set(const char* str) {
  return MyBuffer::Set(str, str == NULL ? 0 : (int)strlen(str));
}
int MyStringA::Set(const char* str, int len) {
  return MyBuffer::Set(str, len);
}
int MyStringA::Append(const char *str, int len) {
  len = len < 0 ? STR_LEN(str) : len;
  return MyBuffer::Append(str, len);
}
int MyStringA::AppendInt(int intVal, int placeHolderLen) {
  int retCode = 0;
  char buf[11] = {0};
  itoa(intVal, buf, 10);
  if (placeHolderLen > 0) {
    assert(strlen(buf) <= placeHolderLen);
    int zeroToAdd = placeHolderLen - strlen(buf);
    for (int i = 0; i < zeroToAdd; i++) {
      if (retCode = Append("0")) return retCode;
    }
  }
  return Append(buf);
}
int MyStringA::SetInt64(UINT64 val, int placeHolderLen) {
  int retCode = 0;
  char buf[32] = {0};
  ltoa(val, buf, 10);
  if (placeHolderLen > 0) {
    assert(strlen(buf) <= placeHolderLen);
    int zeroToAdd = placeHolderLen - strlen(buf);
    for (int i = 0; i < zeroToAdd; i++) {
      if (retCode = Append("0")) return retCode;
    }
  }
  return Set(buf);
}
int MyStringA::AppendInt64(UINT64 intVal, int placeHolderLen) {
  int retCode = 0;
  char buf[32] = {0};
  ltoa(intVal, buf, 10);
  if (placeHolderLen > 0) {
    assert(strlen(buf) <= placeHolderLen);
    int zeroToAdd = placeHolderLen - strlen(buf);
    for (int i = 0; i < zeroToAdd; i++) {
      if (retCode = Append("0")) return retCode;
    }
  }
  return Append(buf);
}

int MyStringA::DerefAsInt64() {
    INT64 val = 0;
    for (int i = 0; i < Length(); i++) {
        char c = Deref()[i] - '0';
        assert(c >= 0 && c <= 9);
        val = val * 10 + c;
    }
    return val;
}
int MyStringA::DerefAsInt() {
  int intVal = 0;
  for (int i = 0; i < Length(); i++) {
    char c = Deref()[i] - '0';
    assert(c >= 0 && c <= 9);
    intVal = intVal * 10 + c;
  }
  return intVal;
}

int MyStringA::ToLower() {
  int len = Length();
  char* ptr = Deref();
  for (int i = 0; i < len; i++) {
    ptr[i] = (char) tolower(ptr[i]);
  }
  return 0;
}

int MyStringA::HashCode() {
    // ref: https://referencesource.microsoft.com/#mscorlib/system/string.cs,0a17bbac4851d0d4

    int hash1 = 5381;
    int hash2 = hash1;

    char* p = Deref();
    int len = Length();
    while (len > 0) {
        hash1 = ((hash1 << 5) + hash1 + (hash1 >> 27)) ^ p[0];
        hash2 = ((hash2 << 5) + hash2 + (hash2 >> 27)) ^ p[1];
        p += 2;
        len -= 2;
    }

    if (len > 0) {
        hash1 = ((hash1 << 5) + hash1 + (hash1 >> 27)) ^ p[0];
    }

    return hash1 + (hash2 * 1566083941);
}


#ifdef _WIN32
int MyStringA::SetUnicode(const wchar_t* uniStr, int len) {
    int err = 0;

    int utf8Len = WideCharToMultiByte(
        CP_UTF8,
        0,
        uniStr,
        len,
        NULL,
        0,
        NULL,
        NULL
    );

    if (err = SetLength(utf8Len)) return err;

    WideCharToMultiByte(
        CP_UTF8,
        0,
        uniStr,
        len,
        Deref(),
        utf8Len + 1,
        NULL,
        NULL
    );
    return 0;
}

int MyStringA::Append(const wchar_t* uniStr, int len) {
    int err = 0;
    MyStringA tmpUtf8;
    if (err = tmpUtf8.SetUnicode(uniStr, len)) return err;
    return this->Append(tmpUtf8.Deref(), tmpUtf8.Length());
}
#endif 