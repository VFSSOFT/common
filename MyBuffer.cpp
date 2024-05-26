#include "MyBuffer.h"

MyBuffer::MyBuffer() {
  memset(m_StackBuf, 0, 64);
  m_Buffer      = m_StackBuf;
  m_Cap         = 62; // 2 bytes for the NULL(char, unicode)
  m_Length      = 0;
}

MyBuffer::~MyBuffer() {
  this->Reset();
}

int MyBuffer::SetCharAt(int index, char c) {
    if (index < 0 || index > Length()) 
        return MY_ERR_INDEX_OUT_OF_BOUNDARY;

    m_Buffer[index] = c;
    return 0;
}
int MyBuffer::CharAt(int index) {
    if (index < 0 || index > Length()) 
        return MY_ERR_INDEX_OUT_OF_BOUNDARY;

    return m_Buffer[index];
}

int MyBuffer::Set(const char* data, int len) {
  return Set(0, data, len);
}
int MyBuffer::Set(int index, const char* data, int len) {
  if (index < 0 || index > Length()) 
    return MY_ERR_INDEX_OUT_OF_BOUNDARY;

  int errCode = SetLength(index + len);
  if (errCode) return errCode;

  memcpy(m_Buffer + index, data, len);
  return 0;
}
int MyBuffer::Append(const char* data, int len) {
  return Set(m_Length, data, len);
}
int MyBuffer::AppendChar(char c) {
  char oneChar[1] = { c };
  return Set(m_Length, oneChar, 1);
}

int MyBuffer::InsertChar(char c, int index) {
  if (index < 0 || index > Length()) 
    return MY_ERR_INDEX_OUT_OF_BOUNDARY;

  if (index == Length()) {
    return AppendChar(c);
  }

  int oldLen = m_Length;
  int errCode = SetLength(Length() + 1);
  if (errCode) {
    return errCode;
  }

  memmove(m_Buffer + index + 1, m_Buffer + index, oldLen);
  m_Buffer[index] = c;
  return 0;
}
int MyBuffer::Insert(const char* data, int len, int index) {
  if (index < 0 || index > Length()) 
    return MY_ERR_INDEX_OUT_OF_BOUNDARY;

  if (len == 0) 
    return 0;

  if (index == Length()) {
    return Append(data, len);
  }

  int oldLen = m_Length;
  int errCode = SetLength(Length() + len);
  if (errCode) {
    return errCode;
  }

  memmove(m_Buffer + index + len, m_Buffer + index, oldLen - index);
  memcpy(m_Buffer + index, data, len);
  return 0;
}


int MyBuffer::SetLength(int len) {
  if (len >= m_Cap) { // m_Cap >= 62 is always TRUE.
    int err = ExtendCap(len);
    if (err) return err;
  } else if (len <= 62 && HeapMemUsed()) {
    memcpy(m_StackBuf, m_Buffer, len);
    free(m_Buffer);
    m_Buffer = m_StackBuf;
    m_Cap = 62;
  }

  // MY_BUFFER_RESERVED_BYTES is 2 for now, so hard code here
  m_Buffer[len]   = 0;
  m_Buffer[len+1] = 0;
  m_Length = len;
  return 0;
}
int MyBuffer::ExtendCap(int cap) {
  assert(cap >= m_Cap); // don't call this function when there is enough buffer

  int newCap = 0;
  if (cap < 1024) {
    newCap = ((cap + 63) / 64 * 64) * 4;
  } else if (cap < 1024 * 1024) {
    newCap = ((cap + 1023) / 1024) * 1024 * 4;
  } else {
    while (newCap <= cap) newCap += 1024 * 1024 * 64;
  }
  if (newCap < cap + MY_BUFFER_RESERVED_BYTES) {
    newCap += 64;
  }
  assert(newCap > 64);

  if (!HeapMemUsed()) {
    m_Buffer = (char*) malloc(newCap);
    if (m_Buffer == NULL)
      return MY_ERR_OUT_OF_MEMORY;

    if (m_Length > 0)
      memcpy(m_Buffer, m_StackBuf, m_Length);

  } else {
    m_Buffer = (char*) realloc(m_Buffer, newCap);
    if (m_Buffer == NULL) {
      return MY_ERR_OUT_OF_MEMORY;
    }
  }
  memset(m_Buffer + m_Length, 0, newCap - m_Length);
  m_Cap = newCap - MY_BUFFER_RESERVED_BYTES;
  return 0;
}

void MyBuffer::Reset() {
  if (HeapMemUsed()) {
    free(m_Buffer);
  }
  m_Buffer = m_StackBuf;
  memset(m_StackBuf, 0, 64);
  m_Cap = 62;
  m_Length = 0;
}

int MyBuffer::Delete(int start, int len) {
  if (start < 0 || len < 0 || start + len > m_Length) return MY_ERR_INDEX_OUT_OF_BOUNDARY;
  if (len == 0) return 0;

  memmove(m_Buffer + start, m_Buffer + start + len, m_Length - start - len);
  return SetLength(m_Length - len);
}

int MyBuffer::Sub(int startIndex) {
    return Sub(startIndex, m_Length);
}
int MyBuffer::Sub(int startIndex, int endIndex) {
  if (startIndex < 0 || startIndex > endIndex || endIndex > m_Length) 
    return MY_ERR_INDEX_OUT_OF_BOUNDARY;

  if (startIndex == endIndex) return 0;

  int newLen = endIndex - startIndex;
  memmove(m_Buffer, m_Buffer + startIndex, newLen);
  return SetLength(newLen);
}

int MyBuffer::TrimLeft(const char* toTrim) {
    int off = 0;
    
    while (off < m_Length) {
        char c = m_Buffer[off];
        if (Contains_(toTrim, c)) {
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
int MyBuffer::TrimRight(const char* toTrim) {
    int newLen = m_Length;
    
    while (newLen > 0) {
        char c = m_Buffer[newLen-1];
        if (Contains_(toTrim, c)) {
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
int MyBuffer::Trim(const char* toTrim) {
    int err = 0;
    if (err = TrimLeft(toTrim)) return err;
    if (err = TrimRight(toTrim)) return err;
    return 0;
}

int MyBuffer::IndexOf(const char* ptr, int len, int startIndex) {
  if (Length() == 0 || len < 0 || startIndex < 0 || startIndex + len > Length())
    return -1;

  int curIdx = startIndex;
  while (curIdx < Length()) {

    if (*(m_Buffer + curIdx) != *ptr) {
      curIdx++;
      continue;
    }

    if (curIdx + len > Length())
      return -1;

    BOOL matched = TRUE;
    for (int i = curIdx; i < curIdx + len; i++) {
      if (*(m_Buffer + i) != *(ptr + i - curIdx)) {
        matched = FALSE;
        break;
      }
    }
    if (matched)
      return curIdx;

    curIdx++;
  }

  return -1;
  
}
int MyBuffer::LastIndexOf(const char* ptr, int len) {
  if (len < 0)
    return -1;

  if (Length() <= 0 || len == 0 || len > Length())
    return -1;

  int curIndex = IndexOf(ptr, len, 0);
  int lastIndex = -1;
  while (curIndex >= 0) {
    lastIndex = curIndex;
    curIndex = IndexOf(ptr, len, lastIndex + len);
  }
  return lastIndex;
}
int MyBuffer::IndexOf(const char c, int startIndex) {
  if (Length() == 0)
    return -1;

  if (startIndex < 0 || startIndex >= Length()) 
    return -1;

  for (int i = startIndex; i < Length(); i++) {
    if (*(m_Buffer + i) == c) 
      return i;
  }
  return -1;

}
int MyBuffer::LastIndexOf(const char c) {
  for (int i = Length() - 1; i >= 0; i--) {
    if (*(m_Buffer + i) == c) 
      return i;
  }
  return -1;
}
int MyBuffer::LastIndexOf(const char c, int startIndex, int endIndex) {
  int index = endIndex - 1;
  while (index >= startIndex) {
    char* pEnd = m_Buffer + index;
    if (*pEnd == c) return index;
    index--;
  }
  return -1;
}
void MyBuffer::Replace(const char toReplace, const char replaceWith) {
  for (int i = 0; i < m_Length; i++) {
    if (*(m_Buffer + i) == toReplace)
      m_Buffer[i] = replaceWith;
  }
}
void MyBuffer::Replace(const char* toReplace, int toReplaceLen, const char* replaceWith, int replaceWithLen) {
    int matched = 0;
    for (int i = 0; i < Length() - toReplaceLen + 1; i++) {
        if (Equals_(Deref(i), toReplace, toReplaceLen)) {
            matched++;
            i += toReplaceLen - 1;
        }
    }
    if (matched == 0) return; // nothing

    int j = 0;
    int newlen = Length() + (replaceWithLen - toReplaceLen) * matched;
    char* newbuf = (char*) malloc(newlen);
    for (int i = 0; i < Length(); i++) {
        if (i + toReplaceLen <= Length() && Equals_(Deref(i), toReplace, toReplaceLen)) {
            memcpy(newbuf + j, replaceWith, replaceWithLen);
            i += toReplaceLen - 1;
            j += replaceWithLen;
        } else {
            newbuf[j++] = m_Buffer[i];
        }
    }
    Set(newbuf, newlen);
    free(newbuf);
}

bool MyBuffer::StartWith(const char* ptr) {
    if (ptr == NULL) return false;
    return StartWith(ptr, strlen(ptr));
}
BOOL MyBuffer::StartWith(const char* ptr, int len) {
  if (len < 0)
    return MY_ERR_INVALID_PARAMETERS;

  if (len > Length()) 
    return FALSE;

  for (int i = 0; i < len; i++) {
    if (*(m_Buffer + i) != *(ptr + i))
      return FALSE;
  }
  return TRUE;
}
BOOL MyBuffer::EndWith(const char* ptr, int len) {
  if (len < 0)
    return MY_ERR_INVALID_PARAMETERS;

  if (len > Length()) 
    return FALSE;

  const char* cur = m_Buffer + Length() - len;
  for (int i = 0; i < len; i++) {
    if (*(cur + i) != *(ptr + i))
      return FALSE;
  }
  return TRUE;
}
BOOL MyBuffer::StartWith(const char c) {
  return Length() == 0 ? FALSE : m_Buffer[0] == c;
}
BOOL MyBuffer::EndWith(const char c) {
  return Length() == 0 ? FALSE : m_Buffer[Length()-1] == c;
}
bool MyBuffer::Contains(const char* ptr, int len) {
    if (len == 0) return false;

    for (int i = 0; i < m_Length - len + 1; i++) {
        if (m_Buffer[i] == ptr[0]) {
            bool matched = true;
            for (int index = 0; index < len; index++) {
                if (m_Buffer[i + index] != ptr[index]) {
                    matched = false;
                    break;
                }
            }
            if (matched) return true;
        }
    }
    return false;
}
bool MyBuffer::Contains(const char* ptr) {
    if (ptr == NULL) return false;
    return Contains(ptr, strlen(ptr));
}

bool MyBuffer::Equals(const char* b1, int b1Len, const char* b2, int b2Len) {
    if (b1Len != b2Len) return false;

    for (int i = 0; i < b1Len; i++) {
        if (b1[i] != b2[i]) return false;
    }

    return true;
}


bool MyBuffer::Contains_(const char* chars, char c) {
    const char* p = chars;

    while (p != NULL && *p != NULL) {
        if (*p == c) {
            return true;
        }
        p++;
    }
    return false;
}

bool MyBuffer::Equals_(const char* p1, const char* p2, int len) {
    for (int i = 0; i < len; i++) {
        if (p1[i] != p2[i]) return false;
    }
    return true;
}
