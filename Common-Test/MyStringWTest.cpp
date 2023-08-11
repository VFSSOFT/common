#include "pch.h"


#include "../MyStringW.h"

TEST(MyStringWTest, ReplaceStringTest1) {
    int errCode = 0;
    MyStringW str;

    errCode = str.Set(L"ABCDEFG");
    EXPECT_EQ(errCode, 0);

    errCode = str.Replace(L"A", L"B");
    EXPECT_EQ(errCode, 0);

    errCode = str.Replace(L"B", L"G");
    EXPECT_EQ(errCode, 0);

    EXPECT_EQ(7, str.Length());
    EXPECT_STREQ(L"GGCDEFG", str.Deref());
}

TEST(MyStringWTest, ReplaceStringTest2) {
    int errCode = 0;
    MyStringW str;

    errCode = str.Set(L"AABBCCDD");
    EXPECT_EQ(errCode, 0);

    errCode = str.Replace(L"AA", L"A");
    EXPECT_EQ(errCode, 0);

    errCode = str.Replace(L"BB", L"B");
    EXPECT_EQ(errCode, 0);

    errCode = str.Replace(L"CC", L"C");
    EXPECT_EQ(errCode, 0);

    errCode = str.Replace(L"DD", L"D");
    EXPECT_EQ(errCode, 0);

    EXPECT_EQ(4, str.Length());
    EXPECT_STREQ(L"ABCD", str.Deref());
}

TEST(MyStringWTest, ReplaceStringTest3) {
    int errCode = 0;
    MyStringW str;

    errCode = str.Set(L"ABCD");
    EXPECT_EQ(errCode, 0);

    errCode = str.Replace(L"A", L"AAAA");
    EXPECT_EQ(errCode, 0);

    errCode = str.Replace(L"B", L"BBB");
    EXPECT_EQ(errCode, 0);

    errCode = str.Replace(L"C", L"CC");
    EXPECT_EQ(errCode, 0);

    errCode = str.Replace(L"D", L"D");
    EXPECT_EQ(errCode, 0);

    EXPECT_EQ(10, str.Length());
    EXPECT_STREQ(L"AAAABBBCCD", str.Deref());
}

TEST(MyStringWTest, ReplaceStringTest4) {
    int errCode = 0;
    MyStringW str;

    errCode = str.Set(L"ABAACAADAA");
    EXPECT_EQ(errCode, 0);

    errCode = str.Replace(L"A", L"");
    EXPECT_EQ(errCode, 0);

    EXPECT_EQ(3, str.Length());
    EXPECT_STREQ(L"BCD", str.Deref());
}

TEST(MyStringWTest, ReplaceStringTest5) {
    int errCode = 0;
    MyStringW str;

    errCode = str.Set(L"ABCDEF");
    EXPECT_EQ(errCode, 0);

    errCode = str.Replace(L"EE", L"");
    EXPECT_EQ(errCode, 0);

    EXPECT_EQ(6, str.Length());
    EXPECT_STREQ(L"ABCDEF", str.Deref());
}

TEST(MyStringWTest, ReplaceStringTest6) {
    int errCode = 0;
    MyStringW str;

    errCode = str.Set(L"AAABCDEF");
    EXPECT_EQ(errCode, 0);

    errCode = str.Replace(L"ABCDEF", L"A");
    EXPECT_EQ(errCode, 0);

    EXPECT_EQ(3, str.Length());
    EXPECT_STREQ(L"AAA", str.Deref());
}

TEST(MyStringWTest, LastIndexTest) {
    int errCode = 0;
    MyStringW str;

    errCode = str.Set(L"testdata/testfolder/abcdef.def");
    EXPECT_EQ(errCode, 0);

    int index = str.IndexOf(L'/', 11);
    EXPECT_EQ(index, 19);

    index = str.LastIndexOf(L'/');
    EXPECT_EQ(index, 19);
}

TEST(MyStringWTest, IndexTest2) {
    int errCode = 0;
    MyStringW str;

    errCode = str.Set(L"/s3fs/dontdelete");
    EXPECT_EQ(errCode, 0);

    int index = str.IndexOf(L"/", 1);
    EXPECT_EQ(index, 5);
}

TEST(MyStringWTest, LastIndexTest2) {
    int errCode = 0;
    MyStringW str;

    errCode = str.Set(L"/s3fs/dontdelete");
    EXPECT_EQ(errCode, 0);

    int index = str.LastIndexOf(L"/");
    EXPECT_EQ(index, 5);
}

TEST(MyStringWTest, ChinsesTest) {
    int err = 0;
    MyStringW str;

    err = str.Set(L"测试");
    EXPECT_EQ(err, 0);

    err = str.Append(L"数据");
    EXPECT_EQ(err, 0);

    EXPECT_TRUE(wcscmp(L"测试数据", str.Deref()) == 0);
}

TEST(MyStringWTest, Chinses2Test) {
    int err = 0;
    MyStringW str;

    err = str.Set(L"测试");
    EXPECT_EQ(err, 0);

    err = str.Append(L".meta", 5);
    EXPECT_EQ(err, 0);

    EXPECT_TRUE(wcscmp(L"测试.meta", str.Deref()) == 0);
}

TEST(MyStringWTest, SetZeroLengthStringTest) {
    int err = 0;
    MyStringW str;

    err = str.Set(L"TEST");
    EXPECT_EQ(err, 0);
    EXPECT_TRUE(wcscmp(L"TEST", str.Deref()) == 0);

    err = str.Set(L"a", 0);
    EXPECT_EQ(err, 0);
    EXPECT_TRUE(wcscmp(L"", str.Deref()) == 0);
}

TEST(MyStringWTest, TruncateLengthTest) {
    int err = 0;
    MyStringW str;

    err = str.Set(L"TEST_ABC");
    EXPECT_EQ(err, 0);
    EXPECT_TRUE(wcscmp(L"TEST_ABC", str.Deref()) == 0);

    err = str.Set(L"TEST", 4);
    EXPECT_EQ(err, 0);
    EXPECT_TRUE(wcscmp(L"TEST", str.Deref()) == 0);
}

TEST(MyStringWTest, AppendAfterSetTest) {
    int err = 0;
    MyStringW str;

    err = str.Set(L"TEST_ABC");
    EXPECT_EQ(err, 0);
    EXPECT_TRUE(wcscmp(L"TEST_ABC", str.Deref()) == 0);

    err = str.Set(L"TEST", 4);
    EXPECT_EQ(err, 0);
    EXPECT_TRUE(wcscmp(L"TEST", str.Deref()) == 0);

    err = str.Append(L"AAA");
    EXPECT_EQ(err, 0);
    EXPECT_TRUE(wcscmp(L"TESTAAA", str.Deref()) == 0);
    EXPECT_EQ(str.Length(), 7);
}

TEST(MyStringWTest, LastIndexOfTest) {
    const wchar_t* rawstr = L"C:\\Users\\martin\\Desktop\\testdata/oss-bucket-chengdu/1G.txt.meta";

    int err = 0;
    MyStringW str;

    err = str.Set(rawstr);
    EXPECT_EQ(err, 0);

    int index = str.LastIndexOf(L'/');
    EXPECT_EQ(index, 51);
    
}

