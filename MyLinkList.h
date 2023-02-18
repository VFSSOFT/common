#ifndef _MY_LINK_LIST_H_
#define _MY_LINK_LIST_H_

#include "MyCoreDef.h"

template<typename T>
class MyLinkListNode {
public:
  T* Data;
  MyLinkListNode* Prev;
  MyLinkListNode* Next;

  MyLinkListNode(T* data, MyLinkListNode* prev = NULL, MyLinkListNode* next=NULL) {
    Data = data;
    Prev = prev;
    Next = next;
  }
};

template<typename T>
class MyLinkList {

public:
  MyLinkList();
  virtual ~MyLinkList();

  inline MyLinkListNode<T>* Head() { return m_Head; }
  inline MyLinkListNode<T>* Tail() { return m_Tail; }
  inline BOOL Empty()           { return m_Size == 0; }
  inline int Size()             { return m_Size; }

  MyLinkListNode<T>* AppendNew();
  MyLinkListNode<T>* InsertBefore(MyLinkListNode<T>* node);  
  MyLinkListNode<T>* InsertAfter(MyLinkListNode<T>* node);
  void Remove(MyLinkListNode<T>* node);
  void Reset();

    void RemoveNoFree(MyLinkListNode<T>* node);
    void Append(MyLinkListNode<T>* node);

private:
  MyLinkListNode<T>* NewNode();
  void FreeNode(MyLinkListNode<T>* node);

    void RemoveNode(MyLinkListNode<T>* node, bool freeNode);
    void AppendNode(MyLinkListNode<T>* newNode);

private:
  int                m_Size;
  MyLinkListNode<T>* m_Head;
  MyLinkListNode<T>* m_Tail;

};

template <typename T>
MyLinkList<T>::MyLinkList() {
  m_Size = 0;
  m_Head = NULL;
  m_Tail = NULL;
}

template <typename T>
MyLinkList<T>::~MyLinkList() {
  this->Reset();
}

template <typename T>
void MyLinkList<T>::Reset() {
  if (m_Size > 0) {
    MyLinkListNode<T>* node = m_Head;
    MyLinkListNode<T>* nodeToFree = NULL;
    while (node != NULL) {
      nodeToFree = node;
      node = node->Next;
      FreeNode(nodeToFree);
    }
  }

  m_Size = 0;
  m_Head = NULL;
  m_Tail = NULL;
}

template <typename T>
MyLinkListNode<T>* MyLinkList<T>::AppendNew() {
    MyLinkListNode<T>* newNode = NewNode();
    AppendNode(newNode);
    return newNode;
}

template <typename T>
MyLinkListNode<T>* MyLinkList<T>::InsertBefore(MyLinkListNode<T>* node) {
  assert(node);

  MyLinkListNode<T>* newNode = NewNode();
  MyLinkListNode<T>* prevNode = node->Prev;
  
  node->Prev = newNode;
  newNode->Next = node;
  newNode->Prev = prevNode;

  if(prevNode != NULL) {
    prevNode->Next = newNode;
  } else {
    m_Head = newNode;
  }
  m_Size++;
  return newNode;
}

template <typename T>
MyLinkListNode<T>* MyLinkList<T>::InsertAfter(MyLinkListNode<T>* node) {
  assert(node);

  MyLinkListNode<T>* newNode = NewNode();
  MyLinkListNode<T>* nextNode = node->Next;
  
  node->Next = newNode;
  newNode->Prev = node;
  newNode->Next = nextNode;

  if (nextNode != NULL) {
    nextNode->Prev = newNode;
  } else {
    m_Tail = newNode;
  }
  m_Size++;

  return newNode;
}

template <typename T>
void MyLinkList<T>::Remove(MyLinkListNode<T>* node) {
  RemoveNode(node, true);
}

template <typename T>
void MyLinkList<T>::RemoveNoFree(MyLinkListNode<T>* node) {
  RemoveNode(node, false);
}

template <typename T>
void MyLinkList<T>::Append(MyLinkListNode<T>* node) {
    AppendNode(node);
}

template <typename T>
void MyLinkList<T>::AppendNode(MyLinkListNode<T>* newNode) {
  if (m_Tail == NULL) {
    // LinkList is empty
    m_Head = m_Tail = newNode;
  } else {
    if (m_Head == m_Tail) {
      // LinkList contains only one node
      m_Tail = newNode;
      m_Tail->Prev = m_Head;
      m_Head->Next = m_Tail;
    } else {
      // LinkList contains more than one node
      m_Tail->Next = newNode;
      newNode->Prev = m_Tail;

      m_Tail = newNode;
    }
  }
  m_Size++;
}

template <typename T>
void MyLinkList<T>::RemoveNode(MyLinkListNode<T>* node, bool freeNode) {
    MyLinkListNode<T>* prevNode = node->Prev;
    MyLinkListNode<T>* nextNode = node->Next;

    if (prevNode == NULL && nextNode == NULL) {
        // it's the only node in the current link list
        m_Head = m_Tail = NULL;
    } else if (prevNode == NULL) { // header node
        nextNode->Prev = NULL;
        m_Head = nextNode;
    } else if (nextNode == NULL) { // tail node
        prevNode->Next = NULL;
        m_Tail = prevNode;
    } else {
        // intermediate node
        prevNode->Next = nextNode;
        nextNode->Prev = prevNode;
    }

    node->Prev = NULL;
    node->Next = NULL;
    if (freeNode) FreeNode(node);
    m_Size--;
}

template <typename T>
MyLinkListNode<T>* MyLinkList<T>::NewNode() {
  T* data = new T();
  return new MyLinkListNode<T>(data, NULL, NULL);
}

template <typename T>
void MyLinkList<T>::FreeNode(MyLinkListNode<T>* node) {
  delete node->Data;
  delete node;
}

#endif // _MY_LINK_LIST_H_