#ifndef MY_ARRAY_H_
#define MY_ARRAY_H_

#include "MyCoreDef.h"

template<typename T>
class MyArray {

public:
  MyArray(int size=0);
  virtual ~MyArray();

  T*   AddNew();
  T*   Get(int idx);
  int  Delete(int idx);
  void Reset();

  inline BOOL Empty() { return m_Size == 0; }
  inline int Size() { return m_Size; }
  int SetSize(int size);
  inline int LastError() { return m_LastError; }

private:
  int EnsureSize(int size);
private:

  T** m_Array;
  int m_Size;
  int m_Cap;
  int m_LastError;

};

template <typename T>
MyArray<T>::MyArray(int size) {
  m_Array = NULL;
  m_Size = 0;
  m_Cap = 0;
  m_LastError = 0;
  this->EnsureSize(size);
}

template <typename T>
MyArray<T>::~MyArray() {
  this->Reset();
}

template <typename T>
T* MyArray<T>::AddNew() {
  int errCode = 0;
  if (errCode = this->EnsureSize(m_Size + 1)) {
    m_LastError = errCode;
    return NULL;
  }
  m_Size++;
  m_Array[m_Size-1] = new T();
  return m_Array[m_Size-1];
}

template <typename T>
T* MyArray<T>::Get(int idx) {
  if (idx < 0 || idx >= m_Size) {
    m_LastError = MY_ERR_INDEX_OUT_OF_BOUNDARY;
    return NULL;
  }

  return m_Array[idx];
}

template <typename T>
int MyArray<T>::Delete(int idx) {
  if (idx < 0 || idx >= m_Size) {
    m_LastError = MY_ERR_INDEX_OUT_OF_BOUNDARY;
    return m_LastError;
  }

  if (m_Array[idx] != NULL)
    delete m_Array[idx];

  for (int i = idx; i < m_Size - 1; i++) {
    m_Array[i] = m_Array[i+1];
  }
  m_Size--;
  m_Array[m_Size] = NULL;

  return 0;
}

template <typename T>
int MyArray<T>::SetSize(int size) {
  int errCode = this->EnsureSize(size);
  if (errCode) {
    m_LastError = errCode;
    return m_LastError;
  }

  for (int i = m_Size; i < size; i++) {
    m_Array[i] = new T();
  }

  m_Size = size;
  return 0;
}

template <typename T>
void MyArray<T>::Reset() {
  if (m_Array != NULL) {
    for (int i = 0; i < m_Size; i++) {
      if (m_Array[i]) delete m_Array[i];
    }
    delete[] m_Array;
    m_Array = NULL;
  }
  m_Size = 0;
  m_Cap = 0;
}

template <typename T>
int MyArray<T>::EnsureSize(int size) {
  if (size <= m_Cap) return 0;

  int newCap = m_Cap == 0 ? 32 : m_Cap;
  while (newCap < size) { newCap = newCap << 1; }

  T** newArray = new T*[newCap];
  if (newArray == NULL) return MY_ERR_OUT_OF_MEMORY;

  for (int i = 0; i < newCap; i++) {
    if (i < m_Size) {
      newArray[i] = m_Array[i];
    } else {
      newArray[i] = NULL;
    }
  }

  if (m_Array != NULL) {
    // Don't free the old element's memories, they are using by the newArray
    delete[] m_Array;
    m_Array = NULL;
  }

  m_Array = newArray;
  m_Cap = newCap;
  // This method doesn't change the m_Size
  return 0;
}



#endif // MY_ARRAY_H_