#include "pch.h"

#include "../../common/MyEncodings.h"
#include "../../common/MyAes.h"

const char* TEST_AES_128_KEY = "\x00\x00\x01\x01\x02\x02\x03\x03\x04\x04\x05\x05\x06\x06\x07\x07";
const char* TEST_AES_128_IV  = "\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0a\x0b\x0c\x0d\x0e\x0f";
const char* TEST_AES_128_IN[]  = {
    "12345678",
    "abcdefgh",
    "abcdefgh12345678",
    "thisisateststrin"
};

const char* TEST_AES_128_CBC_OUT[] = {
    "926ac80595f133830688b9647e56ed57",
    "d61257e7c851711a29a7cf6562fb240c",
    "f5d05c84fe9ab70f47b7d1969231267617c4abed51ad8bb0af51d493aa01f32e",
    "7554edb9d77f9e547803a6c07006a208d5a7234bdb6889c8d39d5ac271361a52"
};


TEST(MyAesTest, Aes128CbcTest) {
    int err = 0;
    MyAes aes;
    MyStringA hex;

    for (int i = 0; i < 4; i++) {
        //err = aes.Init(MY_AES_128_CBC, TEST_AES_128_KEY, TEST_AES_128_IV, true);
        ASSERT_EQ(err, 0);

        err = aes.Update(TEST_AES_128_IN[i], strlen(TEST_AES_128_IN[i]));
        ASSERT_EQ(err, 0);

        err = aes.Final();
        ASSERT_EQ(err, 0);

        MyEncodings::HexEncode(aes.Result()->Deref(), aes.Result()->Length(), &hex);

        ASSERT_STREQ(hex.Deref(), TEST_AES_128_CBC_OUT[i]);
    }

    for (int i = 0; i < 4; i++) {
        //err = aes.Init(MY_AES_128_CBC, TEST_AES_128_KEY, TEST_AES_128_IV, false);
        ASSERT_EQ(err, 0);

        MyBuffer cipher;
        MyEncodings::HexDecode(TEST_AES_128_CBC_OUT[i], strlen(TEST_AES_128_CBC_OUT[i]), &cipher);
        err = aes.Update(cipher.Deref(), cipher.Length());
        ASSERT_EQ(err, 0);

        err = aes.Final();
        ASSERT_EQ(err, 0);

        ASSERT_STREQ(aes.Result()->Deref(), TEST_AES_128_IN[i]);
    }

}