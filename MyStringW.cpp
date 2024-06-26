#ifdef _WIN32

#include "MyStringW.h"

MyStringW::MyStringW(int cap) {
  m_Buffer = NULL;
  m_Cap    = 0;
  m_Length = 0;
  this->EnsureCap(cap);
}

MyStringW::~MyStringW() {
  this->Reset();
}

int MyStringW::Set(const WCHAR* str) {
  int len = wcslen(str);
  return Set(str, len);
}
int MyStringW::Set(const WCHAR* str, int len) {
  if (len == 0) return SetLength(0);

  int errCode = EnsureCap(len);
  if (errCode) {
    return errCode;
  }
  memcpy(m_Buffer, str, len * sizeof(WCHAR));
  if (errCode = SetLength(len)) return errCode;
  return errCode;
}
int MyStringW::Set(MyStringW* wstr) {
    return Set(wstr->Deref(), wstr->Length());
}
int MyStringW::SetUtf8(const char* str, int len) {
  int errCode = 0;
  if (len < 0)
    len = strlen(str);

  errCode = MultiByteToWideChar(
    CP_UTF8,
    0,
    str,
    len,
    NULL,
    NULL
  );
  if (errCode == 0)
    return -1;

  if (errCode = this->SetLength(errCode))
    return errCode;

  errCode = MultiByteToWideChar(
    CP_UTF8,
    0,
    str,
    len,
    this->Deref(),
    this->Length()
  );
  return errCode == 0 ? -1 : 0;
}
int MyStringW::Append(const char* utf8str, int len) {
    int err = 0;
    MyStringW wstr;
    if (err = wstr.SetUtf8(utf8str, len)) { return err; }
    return Append(wstr.Deref(), wstr.Length());
}
int MyStringW::Append(const WCHAR* str, int len) {
  int errCode = 0;
  if (len < 0) len = (int)wcslen(str);
  if (len == 0) return 0;

  if (errCode = EnsureCap(m_Length + len)) {
    return errCode;
  }

  memcpy(m_Buffer + m_Length, str, len * sizeof(WCHAR));
  if (errCode = SetLength(m_Length + len)) return errCode;
  return errCode;
}
int MyStringW::AppendChar(WCHAR c) {
    int errCode = 0;
    if (errCode = EnsureCap(m_Length + 1)) {
        return errCode;
    }
    m_Buffer[m_Length] = c;
    m_Length++;
    return errCode;
}
int MyStringW::AppendInt(int intVal, int placeHolderLen) {
    return AppendInt64(intVal, placeHolderLen);
}
int MyStringW::SetInt64(INT64 val, int placeHolderLen) {
    Reset();
    return AppendInt64(val, placeHolderLen);
}
int MyStringW::AppendInt64(INT64 intVal, int placeHolderLen) {
    int err = 0;
    wchar_t buf[32];
    int numLen = 0;

    memset(buf, 0, sizeof(wchar_t) * 32);

    if (intVal < 0) {
        if (err = AppendChar(L'-')) return err;
        intVal *= -1;
    }
    do {
        buf[numLen++] = (intVal % 10) + L'0';
        intVal /= 10;
    } while (intVal > 0);

    if (placeHolderLen > 0) {
        int zeroToAdd = placeHolderLen - numLen;
        for (int i = 0; i < zeroToAdd; i++) {
            if (err = AppendChar(L'0')) return err;
        }
    }

    for (numLen--; numLen >= 0; numLen--) {
        if (err = AppendChar(buf[numLen])) return err;
    }
    return 0;
}

int MyStringW::EnsureCap(int cap) {
  if (cap <= m_Cap) {
    return 0;
  }

  // extend the buffer
  WCHAR* newBuf = (WCHAR*) malloc((cap + 1) * sizeof(WCHAR));
  if (newBuf == NULL) {
    return MY_ERR_OUT_OF_MEMORY;
  }
  memset(newBuf, 0, (cap + 1) * sizeof(WCHAR));
  if (m_Length > 0) {
    memcpy(newBuf, m_Buffer, m_Length * sizeof(WCHAR));
    free(m_Buffer);
  }
  m_Buffer = newBuf;
  m_Cap = cap;
  return 0;
}

int MyStringW::SetLength(int len) {
  if (len == m_Length) return 0;

  if (len <= m_Cap) {
    // Leave the truncated data in the m_Buffer
    m_Buffer[len] = 0;
  } else /*if (len > m_Cap)*/ {
    int errCode = this->EnsureCap(len);
    if (errCode != 0) return errCode;
  }

  m_Length = len;
  return 0;
}

void MyStringW::Reset() {
  if (m_Cap > 0) {
    assert(m_Buffer != NULL);
    free(m_Buffer);
  } else {
    assert(m_Buffer == NULL);
  }
  m_Buffer = NULL;
  m_Cap    = 0;
  m_Length = 0;
}

int MyStringW::IndexOf(const WCHAR* str, int startIndex) {
  if (str == NULL || wcslen(str) == 0 || m_Length == 0) return -1;

  LPCWSTR curStr = m_Buffer + startIndex;
  LPCWSTR p = wcsstr(curStr, str);
  return  p == NULL ? -1 : (int)(p - m_Buffer);
}
int MyStringW::LastIndexOf(const WCHAR* str) {
  int index    = -1;
  int curIndex = IndexOf(str, 0);

  while (curIndex >= 0) {
    index = curIndex;
    curIndex = IndexOf(str, curIndex + 1);
  }
  return index;
}
int MyStringW::IndexOf(const WCHAR c, int startIndex) {
  if (m_Length == 0) return -1;

  int index = startIndex;
  while (index < m_Length && m_Buffer[index] != c) index++;

  return index < m_Length ? index : -1;
}
int MyStringW::LastIndexOf(const WCHAR c) {
  int index    = -1;
  int curIndex = IndexOf(c, 0);

  while (curIndex >= 0) {
    index = curIndex;
    curIndex = IndexOf(c, curIndex + 1);
  }
  return index;
}

void MyStringW::Replace(const WCHAR toReplace, const WCHAR replaceWith) {
  if (m_Length == 0) return;

  for (int i = 0; i < m_Length; i++) {
    if (m_Buffer[i] == toReplace) m_Buffer[i] = replaceWith;
  }
}
int MyStringW::Replace(const WCHAR* toReplace, const WCHAR* replaceWith) {
    int errCode = 0;
    int toReplaceLen = wcslen(toReplace);
    int replaceWithLen = wcslen(replaceWith);
    if (m_Length == 0 || toReplaceLen == 0) return 0;

    int index = 0;
    while (index < m_Length) {
        wchar_t* p = wcsstr(m_Buffer + index, toReplace);
        if (p == NULL) return 0;

        if (toReplaceLen == replaceWithLen) { // Special case: no more allocation needed
            memcpy(p, replaceWith, replaceWithLen * sizeof(wchar_t));
        } else if (toReplaceLen > replaceWithLen) { // no more allocation needed
            memcpy(p, replaceWith, replaceWithLen * sizeof(wchar_t));
            memmove(p + replaceWithLen, p + toReplaceLen, (m_Length - (p - m_Buffer) - toReplaceLen) * sizeof(wchar_t));
            if (errCode = SetLength(m_Length - (toReplaceLen - replaceWithLen))) return errCode;
        } else {
            int lenToMove = m_Length - (int)(p - m_Buffer) - toReplaceLen;
            if (errCode = SetLength(m_Length + (replaceWithLen - toReplaceLen))) return errCode;
            memmove(p + replaceWithLen, p + toReplaceLen, lenToMove * sizeof(wchar_t));
            memcpy(p, replaceWith, replaceWithLen * sizeof(wchar_t));
        }
        index = (int)(p - m_Buffer) + replaceWithLen;
    }
    return 0;
}

int MyStringW::TrimLeft(const WCHAR* toTrim) {
    int off = 0;
    
    while (off < m_Length) {
        WCHAR c = m_Buffer[off];
        if (Contains(toTrim, c)) {
            off++;
        } else {
            break;
        }
    }

    if (off > 0) {
        Sub(off);
    }
    return 0;
}
int MyStringW::TrimRight(const WCHAR* toTrim) {
    int newLen = m_Length;
    
    while (newLen > 0) {
        WCHAR c = m_Buffer[newLen-1];
        if (Contains(toTrim, c)) {
            newLen--;
        } else {
            break;
        }
    }

    if (newLen < m_Length) {
        SetLength(newLen);
    }
    return 0;
}
int MyStringW::Trim(const WCHAR* toTrim) {
    int err = 0;
    if (err = TrimLeft(toTrim)) return err;
    if (err = TrimRight(toTrim)) return err;
    return 0;
}

int MyStringW::Sub(int startIndex) {
    return Sub(startIndex, m_Length);
}
int MyStringW::Sub(int startIndex, int endIndex) {
    if (startIndex < 0 || startIndex > endIndex || endIndex > m_Length) 
        return MY_ERR_INDEX_OUT_OF_BOUNDARY;

    if (startIndex == endIndex) return 0;

    int newLen = endIndex - startIndex;
    memmove(m_Buffer, m_Buffer + startIndex, newLen * sizeof(WCHAR));
    return SetLength(newLen);
}

INT64 MyStringW::DerefAsInt64() {
    INT64 val = 0;
    bool negative = Length() > 0 && Deref()[0] == L'-';
    int i = negative ? 1 : 0;
    for (; i < Length(); i++) {
        char c = Deref()[i] - L'0';
        assert(c >= 0 && c <= 9);
        val = val * 10 + c;
    }
    return negative ? -1 * val : val;
}
int MyStringW::DerefAsInt() {
    return (int) DerefAsInt64();
}

BOOL MyStringW::StartWith(const WCHAR* str) {
  if (str == NULL || wcslen(str) == 0 || m_Length == 0) return FALSE;

  return wcsstr(m_Buffer, str) == m_Buffer;
}
BOOL MyStringW::EndWith(const WCHAR* str) {
  if (str == NULL) return FALSE;

  int len = wcslen(str);
  if (len == 0 && len > m_Length) return FALSE;
  
  int offset = m_Length - len;
  for (int i = 0; i < len; i++) {
    if (m_Buffer[offset + i] != str[i]) return FALSE;
  }
  return TRUE;
}
BOOL MyStringW::StartWith(const WCHAR c) {
  if (m_Length == 0) return FALSE;

  return m_Buffer[0] == c;
}
BOOL MyStringW::EndWith(const WCHAR c) {
  if (m_Length == 0) return FALSE;

  return m_Buffer[m_Length - 1] == c;
}

BOOL MyStringW::Equals(const WCHAR* str) {
  int len = str == NULL ? 0 : (int)wcslen(str);
  if (len == 0) return m_Length == 0;

  return wcscmp(str, m_Buffer) == 0;
}

int MyStringW::HashCode() {
    // ref: https://referencesource.microsoft.com/#mscorlib/system/string.cs,0a17bbac4851d0d4
    int hash1 = (5381<<16) + 5381;
    int hash2 = hash1;

    wchar_t* p = m_Buffer;
    int len = m_Length;
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

int MyStringW::WStrNCmp(const wchar_t* str1, int len1, const wchar_t* str2, int len2, bool caseSensitive) {

  int index1 = 0;
  int index2 = 0;
  while (index1 < len1 && index2 < len2) {
    wchar_t c1 = str1[index1];
    wchar_t c2 = str2[index2];
    
    int cmpRet = caseSensitive ? c1 - c2 : tolower(c1) - tolower(c2);
    if (cmpRet != 0) return cmpRet;

    index1++;
    index2++;
  }

  return index1 - index2;
}

int MyStringW::JoinPath(const wchar_t* p1, int len1, const wchar_t* p2, int len2, MyStringW* ret) {
    int err = 0;

    // normalize path first, trim the suffix '/' char
    // if the input is just a '/', its length will be trimed to 0.
    if (len1 > 0 && p1[len1 - 1] == L'/') {
        len1--;
    }
    if (len2 > 0 && p2[len2 - 1] == L'/') {
        len2--;
    }

    if (err = ret->Set(p1, len1)) return err;
    if (len2 > 0) {
        if (p2[0] != L'/') {
            if (err = ret->AppendChar(L'/')) return err;
        }
        if (err = ret->Append(p2, len2)) return err;
    }
    return 0;
}
int MyStringW::JoinPath(const wchar_t* p1, int len1, const wchar_t* p2, int len2, const wchar_t* p3, int len3, MyStringW* ret) {
    int err = 0;
    MyStringW tmp;
    if (err = JoinPath(p1, len1, p2, len2, &tmp)) return err;
    if (err = JoinPath(tmp.Deref(), tmp.Length(), p3, len3, ret)) return err;
    return 0;
}
int MyStringW::SplitPath(const wchar_t* path, int pathLen, MyStringW* dir, MyStringW* name) {
    int err = 0;
    MyStringW tmp;

    if (dir) dir->Reset();
    if (name) name->Reset();

    if (pathLen == 0) return 0;
    if (path[pathLen-1] == L'/') pathLen--;

    tmp.Set(path, pathLen);
    int index = tmp.LastIndexOf(L'/');
    assert(index > 0);
    if (dir) dir->Set(path, index); // no '/' suffix
    if (name) name->Set(path + index + 1, pathLen - index - 1);

    return 0;
}

bool MyStringW::Contains(const WCHAR* chars, WCHAR c) {
    const WCHAR* p = chars;

    while (p != NULL && *p != NULL) {
        if (*p == c) {
            return true;
        }
        p++;
    }
    return false;
}

#endif // _WIN32