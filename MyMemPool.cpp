#ifndef _MY_MEM_POOL_CPP_
#define _MY_MEM_POOL_CPP_

#include "MyMemPool.h"

MyMemPool::MyMemPool(int perBlockSize) {
  m_PerBlockSize = perBlockSize;
  m_UsedBlocks = 0;
}
MyMemPool::~MyMemPool() {
  this->Reset();
}

MyMemPoolItem* MyMemPool::Acquire() {

  for (MyLinkListNode<MyMemPoolItem>* node = m_Items.Head(); node != NULL; node = node->Next) {
    MyMemPoolItem* item = node->Data;
    if (item->Used == FALSE) {
      item->Used = TRUE;
      m_UsedBlocks++;
      return item;
    }
  }
 
  // new buf for the current alloc
  MyLinkListNode<MyMemPoolItem>* node = m_Items.AppendNew();
  int err = NewBufForItem(node->Data);
  if (err != 0) return NULL;

  m_UsedBlocks++;
  node->Data->Used = TRUE;
  return node->Data;
}
void MyMemPool::Release(MyMemPoolItem* item) {
  for (MyLinkListNode<MyMemPoolItem>* node = m_Items.Head(); node != NULL; node = node->Next) {
    MyMemPoolItem* curItem = node->Data;
    if (curItem == item) {
      curItem->Used = FALSE;
      m_UsedBlocks--;
      return;
    }
  }
  assert(m_UsedBlocks >= 0);
  assert(0); // should never reach here.
}

void MyMemPool::Reset() {
  if (m_Items.Empty()) return;

  for (MyLinkListNode<MyMemPoolItem>* node = m_Items.Head(); node != NULL; node = node->Next) {
    MyMemPoolItem* item = node->Data;
    assert(item->Used == FALSE);
    if (item->BufLen > 0) // For the case: When Reset is called, and the de-constructor is called again
      FreeBufOfItem(item); // item itself will be freed by the LinkList
  }
  
  m_UsedBlocks = 0;
}

int MyMemPool::NewBufForItem(MyMemPoolItem* item) {
  assert(item->Buf == NULL);
  assert(item->BufLen == 0);

  item->Buf = (char*)malloc(m_PerBlockSize);
  item->BufLen = m_PerBlockSize;

  return item->Buf == NULL ? MY_ERR_OUT_OF_MEMORY : 0;
}
void MyMemPool::FreeBufOfItem(MyMemPoolItem* item) {
  assert(item->Buf != NULL);
  assert(item->BufLen > 0);

  free(item->Buf);
  item->Buf = NULL;
  item->BufLen = 0;
}

#endif // _MY_MEM_POOL_CPP_