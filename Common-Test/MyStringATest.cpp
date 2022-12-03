#include "pch.h"


#include "../MyStringA.h"

TEST(MyStringATest, SetNullStringTest) {
  MyStringA str;

  EXPECT_EQ(0, str.Length());
  EXPECT_STREQ(NULL, str.Deref());
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
  EXPECT_TRUE(str.Deref() == NULL);
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
