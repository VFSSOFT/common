#include "pch.h"

#include "../../common/MyFile.h"

TEST(MyFileTest, GetFreeSpaceTest) {
    int err = 0;
    MyFile f;
    UINT64 avalBytes = 0;
    UINT64 totalBytes = 0;
    UINT64 totalFreeBytes = 0;
    
    err = f.MyGetDiskFreeSpace(L"D:\\Dev\\fs\\common", &avalBytes, &totalBytes, &totalFreeBytes);
    EXPECT_EQ(err, 0);

    EXPECT_TRUE(avalBytes > 0);
    EXPECT_TRUE(totalBytes > 0);
    EXPECT_TRUE(totalFreeBytes > 0);
}
