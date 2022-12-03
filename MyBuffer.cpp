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
  return errCode;
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
  return errCode;
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
  m_Cap = newCap;
  return 0;
}

void MyBuffer::Reset() {
  if (HeapMemUsed()) {
    free(m_Buffer);
  }
  m_Buffer = m_StackBuf;
  m_Cap = 62;
  m_Length = 0;
}

int MyBuffer::Delete(int start, int len) {
  if (start < 0 || len < 0 || start + len > m_Length) return MY_ERR_INDEX_OUT_OF_BOUNDARY;
  if (len == 0) return 0;

  memmove(m_Buffer + start, m_Buffer + start + len, m_Length - start - len);
  return SetLength(m_Length - len);
}

int MyBuffer::Sub(int startIndex, int endIndex) {
  if (startIndex < 0 || startIndex > endIndex || endIndex > m_Length) 
    return MY_ERR_INDEX_OUT_OF_BOUNDARY;

  if (startIndex == endIndex) return 0;

  int newLen = endIndex - startIndex;
  memmove(m_Buffer, m_Buffer + startIndex, newLen);
  return SetLength(newLen);
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
  char* pStart = m_Buffer + startIndex;
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

    for (int i = 0; i < m_Length - len; i++) {
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