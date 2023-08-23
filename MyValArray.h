#ifndef MY_VAL_ARRAY_H_
#define MY_VAL_ARRAY_H_

#include "MyCoreDef.h"

template<typename T>
class MyValArray {

public:
  MyValArray(int size=0);
  virtual ~MyValArray();

  int Add(T item);
  int Set(int idx, T item);
  int Insert(int idx, T item);
  T   Get(int idx);
  T   Last();
  int IndexOf(T tiem);
  int Delete(int idx);
  int Delete2(T val);
  void Clear();
  void Reset();

  inline BOOL Empty() { return m_Size == 0; }
  inline int Size() { return m_Size; }
  int SetSize(int size);
  inline int LastError() { return m_LastError; }

private:
  int EnsureSize(int size);
private:

  T*  m_Array;
  int m_Size;
  int m_Cap;
  int m_LastError;

};

template <typename T>
MyValArray<T>::MyValArray(int size) {
  m_Array = NULL;
  m_Size = 0;
  m_Cap = 0;
  m_LastError = 0;
  this->EnsureSize(size);
}

template <typename T>
MyValArray<T>::~MyValArray() {
  this->Reset();
}

template <typename T>
int MyValArray<T>::Add(T item) {
  int errCode = 0;
  if (errCode = this->EnsureSize(m_Size + 1)) {
    m_LastError = errCode;
    return NULL;
  }
  m_Size++;
  m_Array[m_Size-1] = item;
  return 0;
}

template <typename T>
int MyValArray<T>::Set(int idx, T item) {
  if (idx < 0 || idx >= m_Size) {
    m_LastError = MY_ERR_INDEX_OUT_OF_BOUNDARY;
    return NULL;
  }

  m_Array[idx] = item;
  return 0;
}

template <typename T>
int MyValArray<T>::Insert(int idx, T item) {
  if (idx < 0 || idx >= m_Size) {
    m_LastError = MY_ERR_INDEX_OUT_OF_BOUNDARY;
    return NULL;
  }

  int errCode = this->EnsureSize(m_Size + 1);
  if (errCode) {
    m_LastError = errCode;
    return NULL;
  }

  // m_Size + 1 is the last element in the Array after EnsureSize(m_Size + 1)
  for (int i = m_Size - 1; i >= idx; i--) {
    m_Array[i + 1] = m_Array[i];
  }
  m_Array[idx] = item;
  m_Size++;
  return 0;
}


template <typename T>
T MyValArray<T>::Get(int idx) {
  assert(idx >= 0 && idx < m_Size);
  return m_Array[idx];
}

template <typename T>
T MyValArray<T>::Last() {
    assert(Size() > 0);
    return m_Array[m_Size-1];
}

template <typename T>
int MyValArray<T>::IndexOf(T item) {
  for (int i = 0; i < m_Size; i++) {
    if (m_Array[i] == item)
      return i;
  }
  return -1;
}

template <typename T>
int MyValArray<T>::Delete(int idx) {
  if (idx < 0 || idx >= m_Size) {
    m_LastError = MY_ERR_INDEX_OUT_OF_BOUNDARY;
    return m_LastError;
  }

  for (int i = idx; i < m_Size; i++) {
    m_Array[i] = m_Array[i+1];
  }
  m_Size--;
  return 0;
}

template <typename T>
int MyValArray<T>::Delete2(T val) {
    int index = -1;
    for (int i = 0; i < m_Size; i++) {
        if (m_Array[i] == val) {
            index = i;
            break;
        }
    }
    return Delete(index);
}

template <typename T>
int MyValArray<T>::SetSize(int size) {
  int errCode = this->EnsureSize(size);
  if (errCode) {
    m_LastError = errCode;
    return m_LastError;
  }

  m_Size = size;
  return 0;
}

template <typename T>
void MyValArray<T>::Clear() {
  m_Size = 0; // only reset the size, don't touch the m_Array to improve the performance
}

template <typename T>
void MyValArray<T>::Reset() {
  if (m_Array != NULL) {
    delete[] m_Array;
    m_Array = NULL;
  }
  m_Size = 0;
  m_Cap = 0;
}

template <typename T>
int MyValArray<T>::EnsureSize(int size) {
  if (size <= m_Cap) return 0;

  int newCap = m_Cap == 0 ? 32 : m_Cap;
  while (newCap < size) { newCap = newCap << 1; }

  T* newArray = new T[newCap];
  if (newArray == NULL) return MY_ERR_OUT_OF_MEMORY;

  for (int i = 0; i < newCap; i++) {
    if (i < m_Size) {
      newArray[i] = m_Array[i];
    } else {
      newArray[i] = 0; // default value for T
    }
  }

  if (m_Array != NULL) {
    delete[] m_Array;
    m_Array = NULL;
  }

  m_Array = newArray;
  m_Cap = newCap;
  // This method doesn't change the m_Size
  return 0;
}



#endif // MY_VAL_ARRAY_H_