#include "pch.h"

#include "../../common/MyEncodings.h"
#include "../../common/MyHash.h"


const char* TEST_MD4_HASH_IN[] = {
    "",
    "a",
    "abc",
    "message digest",
    "abcdefghijklmnopqrstuvwxyz",
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789",
    "12345678901234567890123456789012345678901234567890123456789012345678901234567890"
};
const char* TEST_MD4_HASH_OUT[] = {
    "31d6cfe0d16ae931b73c59d7e0c089c0",
    "bde52cb31de33e46245e05fbdbd6fb24",
    "a448017aaf21d8525fc10ae87aa6729d",
    "d9130a8164549fe818874806e1c7014b",
    "d79e1c308aa5bbcdeea8ed63df412da9",
    "043f8582f241db351ce627e153e7f0e4",
    "e33b4ddc9c38f2199c3e7b164fcc0536"
};

const char* TEST_MD5_HASH_IN[] = {
    "",
    "hello",
    "abcdefghijklmn",
    "abcdefghijklmnasasdakasdkjebjasd;x,/adjasdfn2"
};
const char* TEST_MD5_HASH_OUT[] = {
    "d41d8cd98f00b204e9800998ecf8427e",
    "5d41402abc4b2a76b9719d911017c592",
    "0845a5972cd9ad4a46bad66f1253581f",
    "f35fa7c11ca3f7ea1f546b9ebb002377"
};


TEST(MyHashTest, MD4Test) {
    int err = 0;
    MyHash hash;
    MyStringA result;

    for (int i = 0; i < 4; i++) {
        err = hash.QuickCalc(MYHASH_ALG_MD4, TEST_MD4_HASH_IN[i], strlen(TEST_MD4_HASH_IN[i]));
        ASSERT_EQ(err, 0);

        MyEncodings::HexEncode(hash.Result()->Deref(), hash.Result()->Length(), &result, false);
        ASSERT_STREQ(result.Deref(), TEST_MD4_HASH_OUT[i]);
    }
}

TEST(MyHashTest, MD5Test) {
    int err = 0;
    MyHash hash;
    MyStringA result;

    for (int i = 0; i < 4; i++) {
        err = hash.QuickCalc(MYHASH_ALG_MD5, TEST_MD5_HASH_IN[i], strlen(TEST_MD5_HASH_IN[i]));
        ASSERT_EQ(err, 0);

        MyEncodings::HexEncode(hash.Result()->Deref(), hash.Result()->Length(), &result, false);
        ASSERT_STREQ(result.Deref(), TEST_MD5_HASH_OUT[i]);
    }
}