#include "pch.h"

#include "../MyStringA.h"
#include "../MyUtils.h"

TEST(MyUtilsTest, FormatSizeInBytes) {
    MyStringA ret;

    MyUtils::FormatFileSize(0, FILE_SIZE_FORMAT_IN_BYTES, &ret);
    ASSERT_STREQ(ret.Deref(), "0");

    MyUtils::FormatFileSize(12, FILE_SIZE_FORMAT_IN_BYTES, &ret);
    ASSERT_STREQ(ret.Deref(), "12");

    MyUtils::FormatFileSize(123, FILE_SIZE_FORMAT_IN_BYTES, &ret);
    ASSERT_STREQ(ret.Deref(), "123");

    MyUtils::FormatFileSize(1234, FILE_SIZE_FORMAT_IN_BYTES, &ret);
    ASSERT_STREQ(ret.Deref(), "1,234");

    MyUtils::FormatFileSize(12345678, FILE_SIZE_FORMAT_IN_BYTES, &ret);
    ASSERT_STREQ(ret.Deref(), "12,345,678");

    MyUtils::FormatFileSize(123456789, FILE_SIZE_FORMAT_IN_BYTES, &ret);
    ASSERT_STREQ(ret.Deref(), "123,456,789");
}

TEST(MyUtilsTest, FormatSizeInKMGB) {
    MyStringA ret;

    MyUtils::FormatFileSize(0, FILE_SIZE_FOMRAT_IN_KB_MB_GB, &ret);
    ASSERT_STREQ(ret.Deref(), "0 B");

    MyUtils::FormatFileSize(12, FILE_SIZE_FOMRAT_IN_KB_MB_GB, &ret);
    ASSERT_STREQ(ret.Deref(), "12 B");

    MyUtils::FormatFileSize(123, FILE_SIZE_FOMRAT_IN_KB_MB_GB, &ret);
    ASSERT_STREQ(ret.Deref(), "123 B");

    MyUtils::FormatFileSize(1024, FILE_SIZE_FOMRAT_IN_KB_MB_GB, &ret);
    ASSERT_STREQ(ret.Deref(), "1 KB");

    MyUtils::FormatFileSize(1234, FILE_SIZE_FOMRAT_IN_KB_MB_GB, &ret);
    ASSERT_STREQ(ret.Deref(), "1.205 KB");

    MyUtils::FormatFileSize(100 * 1024 + 256, FILE_SIZE_FOMRAT_IN_KB_MB_GB, &ret);
    ASSERT_STREQ(ret.Deref(), "100.25 KB");

    MyUtils::FormatFileSize(1024 * 1024, FILE_SIZE_FOMRAT_IN_KB_MB_GB, &ret);
    ASSERT_STREQ(ret.Deref(), "1 MB");

    MyUtils::FormatFileSize(1024 * 1024 * 10 + 1024, FILE_SIZE_FOMRAT_IN_KB_MB_GB, &ret);
    ASSERT_STREQ(ret.Deref(), "10.001 MB");

    MyUtils::FormatFileSize(1024 * 1024 * 200, FILE_SIZE_FOMRAT_IN_KB_MB_GB, &ret);
    ASSERT_STREQ(ret.Deref(), "200 MB");

    MyUtils::FormatFileSize(1024LL * 1024 * 1024 * 10, FILE_SIZE_FOMRAT_IN_KB_MB_GB, &ret);
    ASSERT_STREQ(ret.Deref(), "10 GB");
}