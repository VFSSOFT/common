
#ifndef _MY_MEM_POOL_H_
#define _MY_MEM_POOL_H_

#include "MyLinkList.h"

class MyMemPoolItem {
public:
  MyMemPoolItem() {
    Buf = NULL;
    BufLen = 0;
    Used = FALSE;
  }

  char* Buf;
  int   BufLen;

  BOOL  Used;
};

class MyMemPool {
public:
  MyMemPool(int perBlockSize=1024);
  ~MyMemPool();

  inline int PerBlockSize() { return m_PerBlockSize; }
  inline int TotalBytes()  { return m_Items.Size() * m_PerBlockSize; }
  inline int UsedBytes()   { return m_UsedBlocks * m_PerBlockSize; }
  inline int UnusedBytes() { return this->UnusedBlocks() * m_PerBlockSize; }
  inline int TotalBlocks() { return m_Items.Size(); }
  inline int UsedBlocks()  { return m_UsedBlocks; }
  inline int UnusedBlocks() { return m_Items.Size() - m_UsedBlocks; }
  
  MyMemPoolItem* Acquire();
  void Release(MyMemPoolItem* item);

  void Reset();

private:
  int NewBufForItem(MyMemPoolItem* item);
  void FreeBufOfItem(MyMemPoolItem* item);

private:
  int                       m_PerBlockSize;
  int                       m_UsedBlocks;
  MyLinkList<MyMemPoolItem> m_Items;
};

#endif // _MY_MEM_POOL_H_