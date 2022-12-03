#ifndef _MY_WSTR_VAL_HASH_MAP_H_
#define _MY_WSTR_VAL_HASH_MAP_H_

#include "MyCoreDef.h"
#include "MyStringW.h"

template <typename ValueT>
class MyWStrValHashNode {
public:
    int                StrHashCode;
    MyStringW          Str;
    ValueT             Value;
    MyWStrValHashNode* Next;

    MyWStrValHashNode(MyStringW* str, ValueT value, int strHashCode) {
        this->StrHashCode = strHashCode;
        this->Str.Set(str->Deref(), str->Length());
        this->Value = value;
        this->Next = NULL;
    }
};

template <typename ValueT>
class MyWStrValHashMap {
public:
  MyWStrValHashMap(int cap=65536);
  ~MyWStrValHashMap();

  bool Get(MyStringW* str, ValueT* val);
  int  Put(MyStringW* str, ValueT val);
  bool Delete(MyStringW* str);
  void Reset();

protected:
    virtual void FreeValue(MyStringW* str, ValueT val) {}
  
private:
    int InternalKeyHash(UINT64 key) {
        return (int)(key % m_Cap);
    }

private:
    int m_Cap;
    MyWStrValHashNode<ValueT>** m_Table;
};

template <typename ValueT>
MyWStrValHashMap<ValueT>::MyWStrValHashMap(int cap) {
    m_Cap = cap;
    m_Table = new MyWStrValHashNode<ValueT>*[cap];
    for (int i = 0; i < m_Cap; i++)
        m_Table[i] = NULL;
}

template <typename ValueT>
MyWStrValHashMap<ValueT>::~MyWStrValHashMap() {
  Reset();
  delete[] m_Table;
}

template <typename ValueT>
bool MyWStrValHashMap<ValueT>::Get(MyStringW* str, ValueT* val) {
    int strHashCode = str->HashCode();
    int hashValue = InternalKeyHash(strHashCode);
    MyWStrValHashNode<ValueT>* node = m_Table[hashValue];

    // quick search
    int matchedCount = 0;
    ValueT firstMatchedValue = 0;
    while (node != NULL) {
        if (node->StrHashCode == strHashCode) {
            if (matchedCount == 0) {
                firstMatchedValue = node->Value;
            }
            matchedCount++;
            if (matchedCount > 1) {
                break;
            }
        }
        node = node->Next;
    }

    if (matchedCount == 0) {
        return false;
    }
    if (matchedCount == 1) {
        *val = firstMatchedValue;
        return true;
    }

    // general search
    node = m_Table[hashValue];
    while (node != NULL) {
        if (node->StrHashCode == strHashCode) {
           if (node->Str.Equals(str->Deref())) {
               *val = node->Value;
               return true;
           }
        }
        node = node->Next;
    }

    return false;
}

template <typename ValueT>
int MyWStrValHashMap<ValueT>::Put(MyStringW* str, ValueT val) {
    int strHashCode = str->HashCode();
    int hashValue = InternalKeyHash(strHashCode);
    MyWStrValHashNode<ValueT>* prev = NULL;
    MyWStrValHashNode<ValueT>* node = m_Table[hashValue];

    while (node != NULL && node->StrHashCode != strHashCode && !node->Str.Equals(str->Deref())) {
        prev = node;
        node = node->Next;
    }

    if (node == NULL) {
        node = new MyWStrValHashNode<ValueT>(str, val, strHashCode);

        if (prev == NULL) {
          // insert as the first bucket
          m_Table[hashValue] = node;
        } else {
          prev->Next = node;
        }
    } else {
        // node founded
        node->Value = val;
    }
    return 0;
}

template <typename ValueT>
bool MyWStrValHashMap<ValueT>::Delete(MyStringW* str) {
    int strHashCode = str->HashCode();
    int hashValue = InternalKeyHash(strHashCode);
    MyWStrValHashNode<ValueT>* prev = NULL;
    MyWStrValHashNode<ValueT>* node = m_Table[hashValue];

    while (node != NULL && node->StrHashCode != strHashCode && !node->Str.Equals(str->Deref())) {
        prev = node;
        node = node->Next;
    }

    if (node == NULL) {
        // Key not found
        return false;
    } else {
        if (prev == NULL) {
            // remove the first bucket of the list
            m_Table[hashValue] = node->Next;
        } else {
            prev->Next = node->Next;
        }

        FreeValue(&node->Str, node->Value);
        delete node;
        return true;
    }
}

template <typename ValueT>
void MyWStrValHashMap<ValueT>::Reset() {
    for (int i = 0; i < m_Cap; i++) {
        MyWStrValHashNode<ValueT>* node = m_Table[i];

        while (node != NULL) {
            MyWStrValHashNode<ValueT>* prev = node;
            node = node->Next;
            FreeValue(&prev->Str, prev->Value);
            delete prev;
        }

        m_Table[i] = NULL;
    }
}

#endif // _MY_WSTR_VAL_HASH_MAP_H_

