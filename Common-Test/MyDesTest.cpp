#include "pch.h"

#include "../../common/MyEncodings.h"
#include "../../common/MyDes.h"


const char* TEST_DES3_KEY = "SEcKEy01SEcKEy02SEcKEy03";
const char* TEST_DES3_IV  = "12345678";
const char* TEST_DES3_IN[]  = {
    "12345678",
    "abcdefgh",
    "abcdefgh12345678",
    "thisisateststrin"
};

const char* TEST_DES3_CBC_OUT[] = {
    "b97908fa9169a52a",
    "b2c111eff1a981c4",
    "b2c111eff1a981c4ee2af6cca373a9f9",
    "07031cfd28ec33a014b455cb8262198f"
};



TEST(MyDesTest, Des3CbcTest) {
    int err = 0;
    MyDes des;
    MyBuffer result;
    MyStringA hex;

    for (int i = 0; i < 4; i++) {
        err = des.InitDES3(TEST_DES3_IV, TEST_DES3_KEY, 8);
        ASSERT_EQ(err, 0);

        err = des.Des3CbcEncrypt(TEST_DES3_IN[i], strlen(TEST_DES3_IN[i]), &result);
        ASSERT_EQ(err, 0);

        MyEncodings::HexEncode(result.Deref(), result.Length(), &hex);
        ASSERT_STREQ(TEST_DES3_CBC_OUT[i], hex.Deref());
    }

    for (int i = 0; i < 4; i++) {
        err = des.InitDES3(TEST_DES3_IV, TEST_DES3_KEY, 8);
        ASSERT_EQ(err, 0);

        MyBuffer cipher;
        MyEncodings::HexDecode(TEST_DES3_CBC_OUT[i], strlen(TEST_DES3_CBC_OUT[i]), &cipher);

        err = des.Des3CbcDecrypt(cipher.Deref(), cipher.Length(), &result);
        ASSERT_EQ(err, 0);

        ASSERT_STREQ(TEST_DES3_IN[i], result.Deref());
    }

}