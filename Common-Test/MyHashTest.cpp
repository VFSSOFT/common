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

const char* TEST_SHA_HASH_IN[] = {
    "",
    "ieieiei",
    "123dsazxcvfhnbb3",
    "ad2cv554523t basd asdad2cv554523t basd asd"
};
const char* TEST_SHA1_HASH_OUT[] = {
    "da39a3ee5e6b4b0d3255bfef95601890afd80709",
    "e8474c577f99d256b13e73d2f4f067881b03d8b9",
    "0d9dce4a0525c99503b6fbed41f8161d7b5bfe9f",
    "0788a78d25e7ba05bb0ece4c7029e50d7eba2590"
};
const char* TEST_SHA224_HASH_OUT[] = {
    "d14a028c2a3a2bc9476102bb288234c415a2b01f828ea62ac5b3e42f",
    "d7493f5423bb0d6884869c1b9b974055dfab02852f6547b8ccd2ada5",
    "20348036471690ad396940093084986995540856fc69534fe4ecdd29",
    "679d4323dae7102524553d7ec7edb3d9306a7323729f26547837a2f9"
};
const char* TEST_SHA256_HASH_OUT[] = {
    "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855",
    "1c753fa9b1493783fbc8bf5f26403d7b43f92d39539b3413580e5e75acd88ca4",
    "be2990897f70cf9be0dd1023c85c2f17449cbef2679d145b150d5f8b26a0d301",
    "f8fe0f111277196dbf2ac342a11bb13061498a40f0202b9b4e5e693a806e63ab"
};
const char* TEST_SHA384_HASH_OUT[] = {
    "38b060a751ac96384cd9327eb1b1e36a21fdb71114be07434c0cc7bf63f6e1da274edebfe76f65fbd51ad2f14898b95b",
    "c571ad4b32ba80c3f7e33b86561f40e907cbfd1efa654e397208ccec8d754de3329e72f9427fcb06252acae327ca521a",
    "5a825715b02bbfe44e65e4fc2e2dfa567714c5d90ead0bb6f3b640ea135fd17c777ad27704f0eb9b426f7670c4f1d9c4",
    "5bd8155c2ce2c8453167f7ee125ea66525ef913752dc0fc2a4ca0bbd84846f190156c7f9754bf9966e74b9667dd4c493"
};
const char* TEST_SHA512_HASH_OUT[] = {
    "cf83e1357eefb8bdf1542850d66d8007d620e4050b5715dc83f4a921d36ce9ce47d0d13c5d85f2b0ff8318d2877eec2f63b931bd47417a81a538327af927da3e",
    "969b65fd199dff713a5e118b93a0be4de5e1598017cf2f86be21094e76d01bd72b937d30cfc6c477b5b1c7840d20319a9d013926f7d7f7ab038403d723109191",
    "795236432f8d6aaac6ae8f389548fb2a052aa02d95d51536f20903bdbcbd9f3642381917271c3eee00d8bcd5db1d249890d08672abb2701c5fd32cabe4376b2e",
    "cf17d3f12e7bde30f538f9eb7dc71f7462364cf43a8da2ae8fa1a771d2a6a548b42bac48b1872c8339c47cdfb9f3fb1f683fc9f3e0b5da3252bc8286bf8a2dab"
};
const char* TEST_SHA3224_HASH_OUT[] = {
    "6b4e03423667dbb73b6e15454f0eb1abd4597f9a1b078e3f5b5a6bc7",
    "206c7c92e475c5479055912d2afec3c56d9154ed395769472f3bd70e",
    "8a0cf3c1415f63b299a1dd1638cb0e5e5227e53549dbfdf9d1a41977",
    "9c9b889d22b9501d28e6cfd08a548884e2e2db4d0713ea3bd80e2097"
};
const char* TEST_SHA3256_HASH_OUT[] = {
    "a7ffc6f8bf1ed76651c14756a061d662f580ff4de43b49fa82d80a4b80f8434a",
    "d50ad25315c738936033839ce7cd2ed067b8bd6cbecf8c9988994e9a28c3bd83",
    "8efcc870a3c4f661409f6a41830911db5f0d06565ba34a41d7327075745d715d",
    "64d0048331eaa810842bc1e12814f4fc8116c469871c82ca8fccc541de880a25"
};
const char* TEST_SHA3384_HASH_OUT[] = {
    "0c63a75b845e4f7d01107d852e4c2485c51a50aaaa94fc61995e71bbee983a2ac3713831264adb47fb6bd1e058d5f004",
    "0f3dc0bfd098935bf5a6c6fe7e1d1cdf1db8cf9ba7150dd5b068c6f9ea7ae5e22f8efa6b3b6523197dbca248a84bc3ee",
    "57c17240866d53dc2f671e281cfc32d368c707dcbf6de8f98f7c527369c82be9b9b7ab92d36eeb603a5333506e86233d",
    "db4313e8ff1fd19a37c1872c1d3c7c8b05ee8d34ed265c703dd5d39d4ff5898d20db75086a25c0f9af011c27c78352cb"
};
const char* TEST_SHA3512_HASH_OUT[] = {
    "a69f73cca23a9ac5c8b567dc185a756e97c982164fe25859e0d1dcc1475c80a615b2123af1f5f94c11e3e9402c3ac558f500199d95b6d3e301758586281dcd26",
    "2a155ca8b98895a6cd48eea6ecea2a057a14c7700d8e447245a0c43b26b57c053984a4cbedf88bace415e950da4905753987025aeaca4d6102d19260a4a9bd22",
    "e49a4aebc4b9c5b826ea02edca03261b4bd316119093fea8739a54d91ba7470d758df2b9f82a22e9de93a4635d695dc30313272390148278460bfd2d66440d40",
    "91c1bf15c0a4729a0973c83c85e138de78de328f61a9c4fa9f0e225b70a1638c87aab11f99b0781b38d5c83a9840bfe953873b72e961ec327ba39ebb9ebe6c64"
};
const char* TEST_SHA512224_HASH_OUT[] = {
    "6ed0dd02806fa89e25de060c19d3ac86cabb87d6a0ddd05c333b84f4",
    "a19ec057cb2204d8699bfa5de0bd1a79cc4fd1fdf4b483137c5a3e89",
    "92459414347e588f32fd55303b53424fb4cae49befeee4e5ec5eeb2e",
    "e66241fc133f86ce58d4bcdc0e9ec6ebbf8500eca711a0d415e938e6"
};
const char* TEST_SHA512256_HASH_OUT[] = {
    "c672b8d1ef56ed28ab87c3622c5114069bdd3ad7b8f9737498d0c01ecef0967a",
    "26ef5238fd7d7c4ea6d91161163cc3afb7fb707425ca1da371747b9182f0d866",
    "32973a81475b2cef421fbd724e7f0a3641e349b300bc773400be47951207451a",
    "acf61547e76a9409769d170f187a88e0a09ba77d55e519097e444a10bd743eb1"
};

void HashImp(int alg, const char** in, const char** out, int len) {
    int err = 0;
    MyHash hash;
    MyStringA result;

    for (int i = 0; i < len; i++) {
        err = hash.QuickCalc(alg, in[i], strlen(in[i]));
        ASSERT_EQ(err, 0);

        MyEncodings::HexEncode(hash.Result()->Deref(), hash.Result()->Length(), &result, false);
        ASSERT_STREQ(result.Deref(), out[i]);
    }
}

TEST(MyHashTest, MD4Test) {
     HashImp(MYHASH_ALG_MD4, TEST_MD4_HASH_IN, TEST_MD4_HASH_OUT, 7);
}

TEST(MyHashTest, MD5Test) {
    HashImp(MYHASH_ALG_MD5, TEST_MD5_HASH_IN, TEST_MD5_HASH_OUT, 4);
}

TEST(MyHashTest, SHA1Test) {
    HashImp(MYHASH_ALG_SHA1, TEST_SHA_HASH_IN, TEST_SHA1_HASH_OUT, 4);
}

TEST(MyHashTest, SHA224Test) {
    HashImp(MYHASH_ALG_SHA224, TEST_SHA_HASH_IN, TEST_SHA224_HASH_OUT, 4);
}

TEST(MyHashTest, SHA256Test) {
    HashImp(MYHASH_ALG_SHA256, TEST_SHA_HASH_IN, TEST_SHA256_HASH_OUT, 4);
}

TEST(MyHashTest, SHA384Test) {
    HashImp(MYHASH_ALG_SHA384, TEST_SHA_HASH_IN, TEST_SHA384_HASH_OUT, 4);
}
TEST(MyHashTest, SHA512Test) {
    HashImp(MYHASH_ALG_SHA512, TEST_SHA_HASH_IN, TEST_SHA512_HASH_OUT, 4);
}

TEST(MyHashTest, SHA3224Test) {
    HashImp(MYHASH_ALG_SHA3224, TEST_SHA_HASH_IN, TEST_SHA3224_HASH_OUT, 4);
}

TEST(MyHashTest, SHA3256Test) {
    HashImp(MYHASH_ALG_SHA3256, TEST_SHA_HASH_IN, TEST_SHA3256_HASH_OUT, 4);
}
TEST(MyHashTest, SHA3384Test) {
    HashImp(MYHASH_ALG_SHA3384, TEST_SHA_HASH_IN, TEST_SHA3384_HASH_OUT, 4);
}
TEST(MyHashTest, SHA3512Test) {
    HashImp(MYHASH_ALG_SHA3512, TEST_SHA_HASH_IN, TEST_SHA3512_HASH_OUT, 4);
}
TEST(MyHashTest, SHA3512224Test) {
    HashImp(MYHASH_ALG_SHA512224, TEST_SHA_HASH_IN, TEST_SHA512224_HASH_OUT, 4);
}
TEST(MyHashTest, SHA3512256Test) {
    HashImp(MYHASH_ALG_SHA512256, TEST_SHA_HASH_IN, TEST_SHA512256_HASH_OUT, 4);
}
