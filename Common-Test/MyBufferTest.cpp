#include "pch.h"

#include "../../common/MyBuffer.h"

TEST(MyBufferTest, SetNullDataTest) {
  MyBuffer buf;

  EXPECT_EQ(0, buf.Set(NULL, 0));
  EXPECT_EQ(0, buf.Length());
  EXPECT_EQ(NULL, buf.Deref());
}

TEST(MyBufferTest, Set1KDataTest) {
  MyBuffer buf;
  char data[1024] = { 0 };

  EXPECT_EQ(0, buf.Set(data, 1024));
  EXPECT_EQ(1024, buf.Length());
}

TEST(MyBufferTest, Set100KDataTest) {
  MyBuffer buf;
  char data[1024 * 100] = { 0 };

  EXPECT_EQ(0, buf.Set(data, 1024 * 100));
  EXPECT_EQ(1024 * 100, buf.Length());
}

TEST(MyBufferTest, SetLengthTest) {
  MyBuffer buf;

  EXPECT_EQ(0, buf.SetLength(100));
  EXPECT_EQ(100, buf.Length());
  EXPECT_TRUE(NULL != buf.Deref());

  EXPECT_EQ(0, buf.SetLength(100 * 1024));
  EXPECT_EQ(100 * 1024, buf.Length());

  EXPECT_EQ(0, buf.SetLength(1));
  EXPECT_EQ(1, buf.Length());

  EXPECT_EQ(0, buf.SetLength(0));
  EXPECT_EQ(0, buf.Length());
}

TEST(MyBufferTest, AppendTest) {
  MyBuffer buf;

  char* str = "hello world";
  int len = strlen(str);
  EXPECT_EQ(0, buf.Append(str, len));
  EXPECT_EQ(len, buf.Length());
  EXPECT_STREQ(buf.Deref(), str);

  str = "!!!";
  len = strlen(str);
  EXPECT_EQ(0, buf.Append(str, len));
  EXPECT_EQ(14, buf.Length());
  EXPECT_STREQ(buf.Deref(), "hello world!!!");

  EXPECT_EQ(0, buf.Deref()[14]);
}

TEST(MyBufferTest, Append100MTest) {
  MyBuffer buf;

  int totalSize = 100 * 1024 * 1024;
  int blockSize = 1024 * 10;
  char* bytes = new char[blockSize];
  memset(bytes, 1, blockSize);
  for (int i = 0; i < totalSize / blockSize; i++) {
    int errCode = buf.Append(bytes, blockSize);
    EXPECT_EQ(errCode, 0);
    EXPECT_EQ(buf.Length(), (i + 1) * blockSize);
  }

  EXPECT_EQ(buf.Length(), totalSize);
  for (int i = buf.Length() - 1; i >= 0; i--) {
    EXPECT_EQ(buf.Deref()[i], 1);
  }
    
}

TEST(MyBufferTest, Append100MTest2) {
  MyBuffer buf;

  int totalSize = 100 * 1024 * 1024;
  int blockSize = 1024;
  char* bytes = new char[blockSize];
  memset(bytes, 1, blockSize);
  for (int i = 0; i < totalSize / blockSize; i++) {
    int errCode = buf.Append(bytes, blockSize);
    EXPECT_EQ(errCode, 0);
    EXPECT_EQ(buf.Length(), (i + 1) * blockSize);
  }

  EXPECT_EQ(buf.Length(), totalSize);
  for (int i = 0; i < buf.Length(); i++)
    EXPECT_EQ(buf.Deref()[i], 1);
}


TEST(MyBufferTest, DeleteTest) {
  MyBuffer buf;

  char* str = "hello world";
  int len = strlen(str);
  EXPECT_EQ(0, buf.Set(str, len));
  EXPECT_EQ(len, buf.Length());
  EXPECT_STREQ(buf.Deref(), str);

  EXPECT_EQ(0, buf.Delete(0, 1));
  EXPECT_STREQ(buf.Deref(), "ello world");

  EXPECT_EQ(0, buf.Delete(2, 3));
  EXPECT_STREQ(buf.Deref(), "elworld");

  EXPECT_EQ(0, buf.Delete(4, 3));
  EXPECT_STREQ(buf.Deref(), "elwo");
}

TEST(MyBufferTest, InsertCharAtStartTest) {
  MyBuffer buf;

  EXPECT_EQ(0, buf.Set("D", 1));
  EXPECT_EQ(1, buf.Length());
  EXPECT_STREQ(buf.Deref(), "D");

  EXPECT_EQ(0, buf.InsertChar('C', 0));
  EXPECT_EQ(2, buf.Length());
  EXPECT_STREQ(buf.Deref(), "CD");

  EXPECT_EQ(0, buf.InsertChar('B', 0));
  EXPECT_EQ(3, buf.Length());
  EXPECT_STREQ(buf.Deref(), "BCD");

  EXPECT_EQ(0, buf.InsertChar('A', 0));
  EXPECT_EQ(4, buf.Length());
  EXPECT_STREQ(buf.Deref(), "ABCD");

}

TEST(MyBufferTest, InsertCharMiddleTest) {
  MyBuffer buf;

  EXPECT_EQ(0, buf.Set("ABD", 3));
  EXPECT_EQ(3, buf.Length());
  EXPECT_STREQ(buf.Deref(), "ABD");

  EXPECT_EQ(0, buf.InsertChar('C', 2));
  EXPECT_EQ(4, buf.Length());
  EXPECT_STREQ(buf.Deref(), "ABCD");

}

TEST(MyBufferTest, InsertCharEndTest) {
  MyBuffer buf;

  EXPECT_EQ(0, buf.Set("ABD", 3));
  EXPECT_EQ(3, buf.Length());
  EXPECT_STREQ(buf.Deref(), "ABD");

  EXPECT_EQ(0, buf.InsertChar('C', 3));
  EXPECT_EQ(4, buf.Length());
  EXPECT_STREQ(buf.Deref(), "ABDC");

}

TEST(MyBufferTest, InsertStrTest) {
  MyBuffer buf;

  EXPECT_EQ(0, buf.Set("ABC", 3));
  EXPECT_EQ(3, buf.Length());
  EXPECT_STREQ(buf.Deref(), "ABC");

  EXPECT_EQ(0, buf.Insert("AB", 2, 0));
  EXPECT_EQ(5, buf.Length());
  EXPECT_STREQ(buf.Deref(), "ABABC");


  EXPECT_EQ(0, buf.Insert("CC", 2, 3));
  EXPECT_EQ(7, buf.Length());
  EXPECT_STREQ(buf.Deref(), "ABACCBC");

}

TEST(MyBufferTest, SetWithIndexTest) {
  MyBuffer buf;

  // Set empty string
  EXPECT_EQ(0, buf.Set(0, "ABCDEF", 6));
  EXPECT_EQ(6, buf.Length());
  EXPECT_STREQ(buf.Deref(), "ABCDEF");

  EXPECT_EQ(0, buf.Set(1, "AA", 2));
  EXPECT_EQ(3, buf.Length());
  EXPECT_STREQ(buf.Deref(), "AAA");

  EXPECT_EQ(0, buf.Set(3, "FGH", 3));
  EXPECT_EQ(6, buf.Length());
  EXPECT_STREQ(buf.Deref(), "AAAFGH");
}


TEST(MyBufferTest, LastIndexOfWithRangeTest) {
  MyBuffer buf;

  const char* p = "A/BC/DEF/GHI/JKL/";

  EXPECT_EQ(0, buf.Set(0, p, strlen(p)));

  int idx = buf.LastIndexOf('/', 0, 2);
  EXPECT_EQ(1, idx);

  idx = buf.LastIndexOf('D', 0, buf.Length());
  EXPECT_EQ(5, idx);

  idx = buf.LastIndexOf('/', 0, buf.Length());
  EXPECT_EQ(strlen(p) - 1, idx);

  idx = buf.LastIndexOf('/', 0, buf.Length() - 1);
  EXPECT_EQ(strlen(p) - 5, idx);
}

TEST(MyBufferTest, SubBufTest) {
  MyBuffer buf;

  const char* p = "A/BC/DEF/GHI/JKL/";

  EXPECT_EQ(0, buf.Set(0, p, strlen(p)));

  buf.Sub(0, buf.Length());
  EXPECT_STREQ(p, buf.Deref());

  buf.Sub(0, buf.Length() - 1);
  EXPECT_STREQ("A/BC/DEF/GHI/JKL", buf.Deref());

  buf.Sub(1, buf.Length());
  EXPECT_STREQ("/BC/DEF/GHI/JKL", buf.Deref());

  buf.Sub(1, buf.Length() - 3);
  EXPECT_STREQ("BC/DEF/GHI/", buf.Deref());
}

TEST(MyBufferTest, IndexWithStartOffsetTest) {
   MyBuffer buf;

  const char* p = "Key1: Value1\r\nKey2: Value2\r\nKey3: Value3\r\n\r\n";

  EXPECT_EQ(0, buf.Set(0, p, strlen(p)));

  int idx = buf.IndexOf("\r\n", 0);
  EXPECT_EQ(idx, 12);

  idx = buf.IndexOf("\r\n", 2, idx + 1);
  EXPECT_EQ(idx, 26);

  idx = buf.IndexOf("\r\n\r\n", 4, 0);
  EXPECT_EQ(idx, 40);

  idx = buf.IndexOf('\r', 13);
  EXPECT_EQ(idx, 26);

}

TEST(MyBufferTest, TrunctTest) {
   MyBuffer buf;

  const char* longText = "http://www.baidu.com/abc/def/ghd";
  EXPECT_EQ(0, buf.Set(0, longText, strlen(longText)));

  const char* shortText = "http://www.jd.com/xuz";
  EXPECT_EQ(0, buf.Set(0, shortText, strlen(shortText)));

  EXPECT_STREQ(buf.Deref(), shortText);
}

TEST(MyBufferTest, IndexOfWithStartIndexTest) {
  MyBuffer buf;

  const char* longText = "abc\r\ndefghadasdfa-193kdmajdf\r\n\r\naskide";
  EXPECT_EQ(0, buf.Set(0, longText, strlen(longText)));

  int idx = buf.IndexOf("\r\n", 2, 0);
  EXPECT_EQ(longText[idx], '\r');
  EXPECT_EQ(longText[idx+1], '\n');

  int idx2 = buf.IndexOf("\r\n", 2, idx + 2);
  EXPECT_TRUE(idx2 > idx);
  EXPECT_EQ(longText[idx2], '\r');
  EXPECT_EQ(longText[idx2+1], '\n');

  int idx3 = buf.IndexOf("\r\n", 2, idx2 + 2);
  EXPECT_TRUE(idx3 == idx2 + 2);
  EXPECT_EQ(longText[idx3], '\r');
  EXPECT_EQ(longText[idx3+1], '\n');

}

TEST(MyBufferTest, IndexOfTest) {
  MyBuffer buf;

  const char* longText = "\r\nabcdef\r\nadasf\r\n";
  EXPECT_EQ(0, buf.Set(0, longText, strlen(longText)));

  int idx = buf.IndexOf("\r\n", 2, 0);
  EXPECT_EQ(idx, 0);

  int idx2 = buf.IndexOf("\r\n", 2, idx + 2);
  EXPECT_TRUE(idx2 > idx);
  EXPECT_EQ(longText[idx2], '\r');
  EXPECT_EQ(longText[idx2+1], '\n');

  int idx3 = buf.IndexOf("\r\n", 2, idx2 + 2);
  EXPECT_EQ(idx3, strlen(longText) - 2);

}

TEST(MyBufferTest, ContainsTest) {
    MyBuffer buf;

    const char* text = "ABCDEFGHIJKLMN";
    EXPECT_EQ(0, buf.Set(text, strlen(text)));

    bool res = buf.Contains("ABCD", 4);
    EXPECT_TRUE(res);

    res = buf.Contains("ACD", 3);
    EXPECT_FALSE(res);

    const char* subText = "EFGHIJKLMN_";
    res = buf.Contains(subText, strlen(subText));
    EXPECT_FALSE(res);
}

TEST(MyBufferTest, Contains2Test) {
    MyBuffer buf;

    char data[5];
    data[0] = 0;
    data[1] = 1;
    data[2] = 2;
    data[3] = 3;
    data[4] = 4;
    EXPECT_EQ(0, buf.Set(0, data, 5));

    bool res = buf.Contains("\x1\x2", 2);
    EXPECT_TRUE(res);

    res = buf.Contains("\x1\x1\x2", 2);
    EXPECT_FALSE(res);
}

