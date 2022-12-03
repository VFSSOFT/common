#include "pch.h"

#include "../../common/MyMemPool.h"

TEST(MyMemPoolTest, DefaultTest) {

  MyMemPool pool;

  EXPECT_TRUE(pool.PerBlockSize() == 1024);
  EXPECT_TRUE(pool.UsedBlocks() == 0);
  EXPECT_TRUE(pool.UnusedBlocks() == 0);
  EXPECT_TRUE(pool.TotalBytes() == 0);
  EXPECT_TRUE(pool.UsedBytes() == 0);
  EXPECT_TRUE(pool.UnusedBytes() == 0);
}

TEST(MyMemPoolTest, SingleBlockTest) {

  MyMemPool pool(65536);

  MyMemPoolItem* item = pool.Acquire();
  EXPECT_TRUE(item->Buf != NULL);
  EXPECT_TRUE(item->BufLen == 65536);

  EXPECT_TRUE(pool.UsedBlocks() == 1);
  EXPECT_TRUE(pool.UnusedBlocks() == 0);
  EXPECT_TRUE(pool.TotalBytes() == 65536);
  EXPECT_TRUE(pool.UsedBytes() == 65536);
  EXPECT_TRUE(pool.UnusedBytes() == 0);

  pool.Release(item);

  EXPECT_TRUE(pool.UsedBlocks() == 0);
  EXPECT_TRUE(pool.UnusedBlocks() == 1);
  EXPECT_TRUE(pool.TotalBytes() == 65536);
  EXPECT_TRUE(pool.UsedBytes() == 0);
  EXPECT_TRUE(pool.UnusedBytes() == 65536);

  pool.Reset();
}

TEST(MyMemPoolTest, MultipleBlocksTest) {

  MyMemPool pool(1024);

  MyMemPoolItem* item1 = pool.Acquire();
  MyMemPoolItem* item2 = pool.Acquire();
  MyMemPoolItem* item3 = pool.Acquire();

  EXPECT_TRUE(item1->Buf != NULL);
  EXPECT_TRUE(item1->BufLen == 1024);
  EXPECT_TRUE(item2->Buf != NULL);
  EXPECT_TRUE(item2->BufLen == 1024);
  EXPECT_TRUE(item1->Buf != item2->Buf); // ensure item1 and item2 points to different memory addresses
  EXPECT_TRUE(pool.UsedBlocks() == 3);
  EXPECT_TRUE(pool.UnusedBlocks() == 0);
  EXPECT_TRUE(pool.TotalBytes() == 3 * 1024);
  EXPECT_TRUE(pool.UsedBytes() == 3 * 1024);
  EXPECT_TRUE(pool.UnusedBytes() == 0);

  pool.Release(item1);
  EXPECT_TRUE(pool.UsedBlocks() == 2);
  EXPECT_TRUE(pool.UnusedBlocks() == 1);
  EXPECT_TRUE(pool.TotalBytes() == 3 * 1024);
  EXPECT_TRUE(pool.UsedBytes() == 2 * 1024);
  EXPECT_TRUE(pool.UnusedBytes() == 1024);

  MyMemPoolItem* newItem1 = pool.Acquire();
  EXPECT_TRUE(newItem1->Buf == item1->Buf); // re-acquire the released buf, it should be reused

  pool.Release(newItem1);
  pool.Release(item2);
  pool.Release(item3);

  EXPECT_TRUE(pool.UsedBlocks() == 0);
  EXPECT_TRUE(pool.UnusedBlocks() == 3);
  EXPECT_TRUE(pool.TotalBytes() == 3 * 1024);
  EXPECT_TRUE(pool.UsedBytes() == 0);
  EXPECT_TRUE(pool.UnusedBytes() == 3 * 1024);
}