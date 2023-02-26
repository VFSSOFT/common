#include "MyStringA.h"


MyStringA::MyStringA() : MyBuffer() {}
MyStringA::~MyStringA() {}

int MyStringA::Set(const char* str) {
  return MyBuffer::Set(str, str == NULL ? 0 : (int)strlen(str));
}
int MyStringA::Set(const char* str, int len) {
  return MyBuffer::Set(str, len);
}
int MyStringA::Set(MyStringA* str) {
    return MyBuffer::Set(str->Deref(), str->Length());
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
    Reset();
    return AppendInt64(val, placeHolderLen);
}
int MyStringA::AppendInt64(UINT64 intVal, int placeHolderLen) {
    int err = 0;
    char buf[32] = {0};
    int numLen = 0;
  
    memset(buf, 0, 32);
    
    // Now it won't reach here, just keep it for future referrence
    if (intVal < 0) {
        if (err = AppendChar('-')) return err;
        intVal *= -1;
    }
    do {
        buf[numLen++] = (intVal % 10) + '0';
        intVal /= 10;
    } while (intVal > 0);
    
    if (placeHolderLen > 0) {
        int zeroToAdd = placeHolderLen - numLen;
        for (int i = 0; i < zeroToAdd; i++) {
            if (err = AppendChar('0')) return err;
        }
    }
    
    for (numLen--; numLen >= 0; numLen--) {
        if (err = AppendChar(buf[numLen])) return err;
    }
    
    return 0;
}

int MyStringA::SetWithFormat(const char* fmt, ...) {
    int formattedLen = 0;
    char tmpBuf[1024];
    va_list args;
    va_start(args, fmt);
    formattedLen = vsprintf(tmpBuf, fmt, args);
    va_end(args);

    return this->Set(tmpBuf, formattedLen);
}
int MyStringA::AppendWithFormat(const char* fmt, ...) {
    int formattedLen = 0;
    char tmpBuf[1024];
    va_list args;
    va_start(args, fmt);
    formattedLen = vsprintf(tmpBuf, fmt, args);
    va_end(args);

    return this->Append(tmpBuf, formattedLen);
}

UINT64 MyStringA::DerefAsInt64() {
    INT64 val = 0;
    bool negative = Length() > 0 && Deref()[0] == '-';
    int i = negative ? 1 : 0;
    for (; i < Length(); i++) {
        char c = Deref()[i] - '0';
        assert(c >= 0 && c <= 9);
        val = val * 10 + c;
    }
    return negative ? -1 * val : val;
}
int MyStringA::DerefAsInt() {
    return (int)DerefAsInt64();
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
    while (len > 1) {
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

bool MyStringA::IsLowercaseLetter(const char c) {
    return c >= 'a' && c <= 'z';
}
bool MyStringA::IsUppercaseLetter(const char c) {
    return c >= 'A' && c <= 'Z';
}
bool MyStringA::IsLetter(const char c) {
    return IsLowercaseLetter(c) || IsUppercaseLetter(c);
}
bool MyStringA::IsLowercaseLetter(const char* str, int len) {
    for (int i = 0; i < len; i++) {
        if (!IsLowercaseLetter(str[i])) return false;
    }
    return true;
}
bool MyStringA::IsUppercaseLetter(const char* str, int len) {
    for (int i = 0; i < len; i++) {
        if (!IsUppercaseLetter(str[i])) return false;
    }
    return true;
}
bool MyStringA::IsLetter(const char* str, int len) {
    return IsLowercaseLetter(str, len) || IsUppercaseLetter(str, len);
}
bool MyStringA::IsDigit(const char c) {
    return c >= '0' && c <= '9';
}
bool MyStringA::IsDigit(const char* str, int len) {
    for (int i = 0; i < len; i++) {
        if (!IsDigit(str[i])) return false;
    }
    return true;
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
