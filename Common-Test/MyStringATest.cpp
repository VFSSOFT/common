#include "pch.h"


#include "../MyStringA.h"

TEST(MyStringATest, SetNullStringTest) {
  MyStringA str;

  EXPECT_EQ(0, str.Length());
  EXPECT_STREQ("", str.Deref());
}

TEST(MyStringATest, SetSmallStringTest) {
  MyStringA str;

  EXPECT_EQ(0, str.Set("hello"));
  EXPECT_EQ(5, str.Length());
  EXPECT_STREQ("hello", str.Deref());
}

TEST(MyStringATest, SetStringTest) {
  MyStringA str;

  EXPECT_EQ(0, str.Set("hello"));
  EXPECT_EQ(5, str.Length());
  EXPECT_STREQ("hello", str.Deref());

  EXPECT_EQ(0, str.Set("he"));
  EXPECT_EQ(2, str.Length());
  EXPECT_STREQ("he", str.Deref());
}

TEST(MyStringATest, SetZeroLengthStringTest) {
  MyStringA str;

  EXPECT_EQ(0, str.Set("hello"));
  EXPECT_EQ(5, str.Length());
  EXPECT_STREQ("hello", str.Deref());

  EXPECT_EQ(0, str.Set("1", 0));
  EXPECT_EQ(0, str.Length());
  EXPECT_STREQ(str.Deref(), "");
}

TEST(MyStringATest, TruncateLengthTest) {
    int err = 0;
    MyStringA str;

    err = str.Set("TEST_ABC");
    EXPECT_EQ(err, 0);
    EXPECT_STREQ("TEST_ABC", str.Deref());

    err = str.Set("TEST", 4);
    EXPECT_EQ(err, 0);
    EXPECT_STREQ("TEST", str.Deref());
}

TEST(MyStringATest, AppendAfterSetTest) {
    int err = 0;
    MyStringA str;

    err = str.Set("TEST_ABC");
    EXPECT_EQ(err, 0);
    EXPECT_STREQ("TEST_ABC", str.Deref());

    err = str.Set("TEST", 4);
    EXPECT_EQ(err, 0);
    EXPECT_STREQ("TEST", str.Deref());

    err = str.Append("AAA");
    EXPECT_EQ(err, 0);
    EXPECT_STREQ("TESTAAA", str.Deref());
    EXPECT_EQ(str.Length(), 7);
}

TEST(MyStringATest, AppendIntTest1) {
    int err = 0;
    MyStringA str;

    err = str.AppendInt(0);
    EXPECT_EQ(err, 0);

    EXPECT_STREQ(str.Deref(), "0");
    EXPECT_EQ(str.DerefAsInt(), 0);
}

TEST(MyStringATest, AppendIntTest2) {
    int err = 0;
    MyStringA str;

    err = str.AppendInt(103);
    EXPECT_EQ(err, 0);

    EXPECT_STREQ(str.Deref(), "103");
    EXPECT_EQ(str.DerefAsInt(), 103);
}

TEST(MyStringATest, AppendIntTest3) {
    int err = 0;
    MyStringA str;

    err = str.AppendInt(-2);
    EXPECT_EQ(err, 0);

    EXPECT_STREQ(str.Deref(), "-2");
    EXPECT_EQ(str.DerefAsInt(), -2);
}
TEST(MyStringATest, AppendInt64Test1) {
    int err = 0;
    MyStringA str;

    err = str.AppendInt64(0);
    EXPECT_EQ(err, 0);

    EXPECT_STREQ(str.Deref(), "0");
    EXPECT_EQ(str.DerefAsInt64(), 0);
}

TEST(MyStringATest, AppendInt64Test2) {
    int err = 0;
    MyStringA str;

    err = str.AppendInt64(103);
    EXPECT_EQ(err, 0);

    EXPECT_STREQ(str.Deref(), "103");
    EXPECT_EQ(str.DerefAsInt64(), 103);
}

TEST(MyStringATest, AppendInt64Test3) {
    int err = 0;
    MyStringA str;

    err = str.AppendInt64(-2);
    EXPECT_EQ(err, 0);

    EXPECT_STREQ(str.Deref(), "-2");
    EXPECT_EQ(str.DerefAsInt64(), -2);
}

TEST(MyStringATest, AppendInt64Test4) {
    int err = 0;
    MyStringA str;

    err = str.Set("bytes=");
    EXPECT_EQ(err, 0);

    err = str.AppendInt64(0);
    EXPECT_EQ(err, 0);

    EXPECT_STREQ(str.Deref(), "bytes=0");
}

TEST(MyStringATest, LongStringTest) {
    const char* longstr = "{Local:{Path:\"/s3fs/RenameFileToDifferentFolder_TargetFolderExist\",FileName:\"file\",Size:1,Attributes:0,CreationTime:0,ModifiedTime:0,AccessTime:0},Remote:{Path:\"/s3fs/RenameFileToDifferentFolder_TargetFolderExist\",FileName:\"file\",Size:1,Attributes:0,CreationTime:0,ModifiedTime:0,AccessTime:0},Data:{BlockCount:1,[0,0,1,0]},MetadataFile:\"testdata/s3fs/RenameFileToDifferentFolder_TargetFolderExist/file.meta\",CacheFile:\"testdata/s3fs/RenameFileToDifferentFolder_TargetFolderExist/file\",RefCount:0,Deleted:False}";
    int err = 0;
    MyStringA str;

    err = str.Set(longstr);
    EXPECT_EQ(err, 0);

    str.Reset();
}

TEST(MyStringATest, LongStringTest2) {
    const char* longstr = "{Local:{Path:\"/s3fs/RenameFileToDifferentFolder_TargetFolderExist\",FileName:\"file\",Size:1,Attributes:0,CreationTime:0,ModifiedTime:0,AccessTime:0},Remote:{Path:\"/s3fs/RenameFileToDifferentFolder_TargetFolderExist\",FileName:\"file\",Size:1,Attributes:0,CreationTime:0,ModifiedTime:0,AccessTime:0},Data:{BlockCount:1,[0,0,1,0]},MetadataFile:\"testdata/s3fs/RenameFileToDifferentFolder_TargetFolderExist/file.meta\",CacheFile:\"testdata/s3fs/RenameFileToDifferentFolder_TargetFolderExist/file\",RefCount:0,Deleted:False}";
    int len = strlen(longstr);
    int err = 0;
    MyStringA str;

    for (int i = 0; i < len; i += 10) {
        int l = i + 10 < len ? 10 : len - i;
        err = str.Append(longstr + i, l);
        EXPECT_EQ(err, 0);
    }

    str.Reset();
}

