#ifndef _MY_VAL_LINK_LIST_H_
#define _MY_VAL_LINK_LIST_H_

#include "MyCoreDef.h"

template<typename T>
class MyValLinkListNode {
public:
  T Data;
  MyValLinkListNode* Prev;
  MyValLinkListNode* Next;

  MyValLinkListNode(MyValLinkListNode* prev = NULL, MyValLinkListNode* next=NULL) {
    Prev = prev;
    Next = next;
  }
};

template<typename T>
class MyValLinkList {

public:
  MyValLinkList();
  virtual ~MyValLinkList();

  inline MyValLinkListNode<T>* Head() { return m_Head; }
  inline MyValLinkListNode<T>* Tail() { return m_Tail; }
  inline BOOL Empty()                 { return m_Size == 0; }
  inline int Size()                   { return m_Size; }

  MyValLinkListNode<T>* AppendNew();
  MyValLinkListNode<T>* InsertBefore(MyValLinkListNode<T>* node);  
  MyValLinkListNode<T>* InsertAfter(MyValLinkListNode<T>* node);
  void Remove(MyValLinkListNode<T>* node);
  void Reset();

private:
  MyValLinkListNode<T>* NewNode();
  void FreeNode(MyValLinkListNode<T>* node);

private:
  int                   m_Size;
  MyValLinkListNode<T>* m_Head;
  MyValLinkListNode<T>* m_Tail;

};

template <typename T>
MyValLinkList<T>::MyValLinkList() {
  m_Size = 0;
  m_Head = NULL;
  m_Tail = NULL;
}

template <typename T>
MyValLinkList<T>::~MyValLinkList() {
  this->Reset();
}

template <typename T>
void MyValLinkList<T>::Reset() {
  if (m_Size > 0) {
    MyValLinkListNode<T>* node = m_Head;
    MyValLinkListNode<T>* nodeToFree = NULL;
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
MyValLinkListNode<T>* MyValLinkList<T>::AppendNew() {
  MyValLinkListNode<T>* newNode = NewNode();

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

  return newNode;
}

template <typename T>
MyValLinkListNode<T>* MyValLinkList<T>::InsertBefore(MyValLinkListNode<T>* node) {
  assert(node);

  MyValLinkListNode<T>* newNode = NewNode();
  MyValLinkListNode<T>* prevNode = node->Prev;
  
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
MyValLinkListNode<T>* MyValLinkList<T>::InsertAfter(MyValLinkListNode<T>* node) {
  assert(node);

  MyValLinkListNode<T>* newNode = NewNode();
  MyValLinkListNode<T>* nextNode = node->Next;
  
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
void MyValLinkList<T>::Remove(MyValLinkListNode<T>* node) {
  MyValLinkListNode<T>* prevNode = node->Prev;
  MyValLinkListNode<T>* nextNode = node->Next;

  if (prevNode == NULL && nextNode == NULL) {
    // it's the only node in the current link list
    m_Head = m_Tail = NULL;
  } else if (prevNode == NULL) { // header node
    nextNode->Prev = NULL;
    m_Head = nextNode;
  } else if (nextNode == NULL) { // tail node
    MyValLinkListNode<T>* prevNode = m_Tail->Prev;
    prevNode->Next = NULL;
    m_Tail = prevNode;
  } else {
    // intermediate node
    prevNode->Next = nextNode;
    nextNode->Prev = prevNode;
  }
  
  FreeNode(node);
  m_Size--;
}

template <typename T>
MyValLinkListNode<T>* MyValLinkList<T>::NewNode() {
  return new MyValLinkListNode<T>(NULL, NULL);
}

template <typename T>
void MyValLinkList<T>::FreeNode(MyValLinkListNode<T>* node) {
  delete node;
}

#endif // _MY_VAL_LINK_LIST_H_