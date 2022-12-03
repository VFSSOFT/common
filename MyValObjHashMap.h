#ifndef _MY_HASH_MAP_H_
#define _MY_HASH_MAP_H_

#include "MyCoreDef.h"

template <typename KeyT, typename ValueT>
class MyValObjHashNodeValueCreator {
public:
  virtual ValueT* Create(KeyT key) = 0;
  virtual void    Destroy(KeyT key, ValueT* value) = 0;
};

template <typename KeyT, typename ValueT>
class MyValObjHashNode {
public:
  KeyT              Key;
  ValueT*           Value;
  MyValObjHashNode* Next;

  MyValObjHashNode(KeyT key, ValueT* value) {
    Key = key;
    Value = value;
    Next = NULL;
  }
};

template <typename KeyT, typename ValueT>
class MyValObjHashMap {
public:
  MyValObjHashMap(int cap=256);
  ~MyValObjHashMap();

  ValueT* Get(KeyT key);
  MyValObjHashNode<KeyT, ValueT>* Put(KeyT key);
  void Delete(KeyT key);
  void Reset();

  MyValObjHashNodeValueCreator<KeyT, ValueT>* ValueCreator;
  
private:
  int InternalKeyHash(KeyT key) {
    UINT64 val = (UINT64)key;
    return (int)(val % m_Cap);
  }
  BOOL InternalKeyEquals(KeyT key1, KeyT key2) {
    return key1 == key2; // both of key1,key2 are value type, just compare them.
  }
  MyValObjHashNode<KeyT, ValueT>* NewNode(KeyT key) {
    ValueT* val = NULL;
    if (ValueCreator) {
      val = ValueCreator->Create(key);
    } else {
      val = new ValueT();
    }
    assert(val != NULL);
    return new MyValObjHashNode<KeyT, ValueT>(key, val);
  }
  void FreeNode(MyValObjHashNode<KeyT, ValueT>* node) {
    if (node->Value) {
      if (ValueCreator) {
        ValueCreator->Destroy(node->Key, node->Value);
      } else {
        delete node->Value;
      }
    }
    delete node;
  }

private:
  int m_Cap;
  MyValObjHashNode<KeyT, ValueT>** m_Table;
};

template <typename KeyT, typename ValueT>
MyValObjHashMap<KeyT, ValueT>::MyValObjHashMap(int cap) {
  m_Cap = cap;
  m_Table = new MyValObjHashNode<KeyT, ValueT>*[cap];
  for (int i = 0; i < m_Cap; i++)
    m_Table[i] = NULL;

  ValueCreator = NULL;
}

template <typename KeyT, typename ValueT>
MyValObjHashMap<KeyT, ValueT>::~MyValObjHashMap() {
  Reset();
  delete[] m_Table;

  if (ValueCreator != NULL)
    delete ValueCreator;
}

template <typename KeyT, typename ValueT>
ValueT* MyValObjHashMap<KeyT, ValueT>::Get(KeyT key) {
  int hashValue = InternalKeyHash(key);
  MyValObjHashNode<KeyT, ValueT>* node = m_Table[hashValue];

  while (node != NULL) {
    if (InternalKeyEquals(node->Key, key)) {
      return node->Value;
    }
    node = node->Next;
  }

  return NULL;
}

template <typename KeyT, typename ValueT>
MyValObjHashNode<KeyT, ValueT>* MyValObjHashMap<KeyT, ValueT>::Put(KeyT key) {
  int hashValue = InternalKeyHash(key);
  MyValObjHashNode<KeyT, ValueT>* prev = NULL;
  MyValObjHashNode<KeyT, ValueT>* node = m_Table[hashValue];

  while (node != NULL && !InternalKeyEquals(node->Key, key)) {
    prev = node;
    node = node->Next;
  }

  if (node == NULL) {
    node = NewNode(key);

    if (prev == NULL) {
      // insert as the first bucket
      m_Table[hashValue] = node;
    } else {
      prev->Next = node;
    }

    return node;
  } else {
    // node founded
    return node;
  }
}

template <typename KeyT, typename ValueT>
void MyValObjHashMap<KeyT, ValueT>::Delete(KeyT key) {
  int hashValue = InternalKeyHash(key);
  MyValObjHashNode<KeyT, ValueT>* prev = NULL;
  MyValObjHashNode<KeyT, ValueT>* node = m_Table[hashValue];

  while (node != NULL && !InternalKeyEquals(node->Key, key)) {
    prev = node;
    node = node->Next;
  }

  if (node == NULL) {
    // Key not found
    return;
  } else {
    if (prev == NULL) {
      // remove the first bucket of the list
      m_Table[hashValue] = node->Next;
    } else {
      prev->Next = node->Next;
    }

    FreeNode(node);
  }
}

template <typename KeyT, typename ValueT>
void MyValObjHashMap<KeyT, ValueT>::Reset() {
  for (int i = 0; i < m_Cap; i++) {
    MyValObjHashNode<KeyT, ValueT>* node = m_Table[i];

    while (node != NULL) {
      MyValObjHashNode<KeyT, ValueT>* prev = node;
      node = node->Next;
      FreeNode(prev);
    }

    m_Table[i] = NULL;
  }
}

#endif // _MY_HASH_MAP_H_

