#ifndef _MY_ASTR_VAL_HASH_MAP_H_
#define _MY_ASTR_VAL_HASH_MAP_H_

#include "MyCoreDef.h"
#include "MyStringA.h"

template <typename ValueT>
class MyAStrValHashNode {
public:
    int                StrHashCode;
    MyStringA          Str;
    ValueT             Value;
    MyAStrValHashNode* Next;

    MyAStrValHashNode(MyStringA* str, ValueT value, int strHashCode) {
        this->StrHashCode = strHashCode;
        this->Str.Set(str->Deref(), str->Length());
        this->Value = value;
        this->Next = NULL;
    }
};

template <typename ValueT>
class MyAStrValHashMap {
public:
  MyAStrValHashMap(int cap=65536);
  ~MyAStrValHashMap();

  bool Get(MyStringA* str, ValueT* val);
  int  Put(MyStringA* str, ValueT val);
  bool Delete(MyStringA* str);
  void Reset();

protected:
    virtual void FreeValue(MyStringA* str, ValueT val) {}
  
private:
    int InternalKeyHash(UINT64 key) {
        return (int)(key % m_Cap);
    }

private:
    int m_Cap;
    MyAStrValHashNode<ValueT>** m_Table;
};

template <typename ValueT>
MyAStrValHashMap<ValueT>::MyAStrValHashMap(int cap) {
    m_Cap = cap;
    m_Table = new MyAStrValHashNode<ValueT>*[cap];
    for (int i = 0; i < m_Cap; i++)
        m_Table[i] = NULL;
}

template <typename ValueT>
MyAStrValHashMap<ValueT>::~MyAStrValHashMap() {
  Reset();
  delete[] m_Table;
}

template <typename ValueT>
bool MyAStrValHashMap<ValueT>::Get(MyStringA* str, ValueT* val) {
    int strHashCode = str->HashCode();
    int hashValue = InternalKeyHash(strHashCode);
    MyAStrValHashNode<ValueT>* node = m_Table[hashValue];

    // quick search
    int matchedCount = 0;
    ValueT firstMatchedValue;
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
int MyAStrValHashMap<ValueT>::Put(MyStringA* str, ValueT val) {
    int strHashCode = str->HashCode();
    int hashValue = InternalKeyHash(strHashCode);
    MyAStrValHashNode<ValueT>* prev = NULL;
    MyAStrValHashNode<ValueT>* node = m_Table[hashValue];

    while (node != NULL && node->StrHashCode != strHashCode && !node->Str.Equals(str->Deref())) {
        prev = node;
        node = node->Next;
    }

    if (node == NULL) {
        node = new MyAStrValHashNode<ValueT>(str, val, strHashCode);

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
bool MyAStrValHashMap<ValueT>::Delete(MyStringA* str) {
    int strHashCode = str->HashCode();
    int hashValue = InternalKeyHash(strHashCode);
    MyAStrValHashNode<ValueT>* prev = NULL;
    MyAStrValHashNode<ValueT>* node = m_Table[hashValue];

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
void MyAStrValHashMap<ValueT>::Reset() {
    for (int i = 0; i < m_Cap; i++) {
        MyAStrValHashNode<ValueT>* node = m_Table[i];

        while (node != NULL) {
            MyAStrValHashNode<ValueT>* prev = node;
            node = node->Next;
            FreeValue(&prev->Str, prev->Value);
            delete prev;
        }

        m_Table[i] = NULL;
    }
}

#endif // _MY_ASTR_VAL_HASH_MAP_H_

