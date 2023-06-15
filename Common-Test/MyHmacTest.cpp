#include "pch.h"

#include "../../common/MyEncodings.h"
#include "../../common/MyHmac.h"

const char* TEST_HMAC_KEY = "myTeStH Ma c key";

const char* TEST_HMAC_IN[] = {
    "abc",
    "ieieiei",
    "123dsazxcvfhnbb3",
    "ad2cv554523t basd asdad2cv554523t basd asd"
};
const char* TEST_HMAC_MD4_OUT[] = {
    "74d47a876e8bee4700aa5b3015fe637e",
    "a4f47700d8cc47e5307952ec7ceef23d",
    "c34280da0d800af77d150bd141241ded",
    "6f6eaa2eebe716d0b6b8e933631eaee2"
};
const char* TEST_HMAC_MD5_OUT[] = {
    "b63d667296ac21e5d1c7e0c735e138bf",
    "43d0f18080c3b1866b76adea303210d1",
    "3d8fee86d19156b90dce1635dc1db2db",
    "0417550ba86e162d2624ed2133249755"
};
const char* TEST_HMAC_SHA1_OUT[] = {
    "f61605a16a6fb07af307eaaa9a27d9400253cc71",
    "296bb1c49c4339c3cc1b2529af8cccfb10cc5e17",
    "f0daf6393aed6d6a02e7ca3749d2934a1d880145",
    "63b1c9841aa9a93c137195775f9b42b77c36e1e6"
};
const char* TEST_HMAC_SHA224_OUT[] = {
    "d44ada20ddbe306b73b4c03d3f924b6452afca145b18c8506f327dbe",
    "d20fb75312328097e530bd8ff6a9d3bfa7aca0fda661a96801c65005",
    "adb355d09e207464cc43d7349e6b6907b302ddb42a0501d8383c52f3",
    "a5afc143f2b240d48136614adc54382f567fbd97aa70abb1af5f4beb"
};
const char* TEST_HMAC_SHA256_OUT[] = {
    "f7071770c87602f59174202e156535a84dffa3fa6c7693e957f02d99cd1f4709",
    "927af59463aaab97a66ba7668176b9e2ce5064be6854db9995704d506cd2374d",
    "b3e95b8c1e96e8f5ed34d33636f3ed3defee142c342d33653f011c696114edf8",
    "1685c73ad1cf6b35187ffe0a88a0b352ee7f92d4e6418d89fe88f23fdf72abe6"
};
const char* TEST_HMAC_SHA384_OUT[] = {
    "da5da2ee28f75683c37e3ea397116b80642640a69a5c9de2a18bbbddd0a272b00b98fd5b7d969983bfba3dc1935d16f6",
    "65f6186837306be15016f3a373da686b810e9413ce97e5143e02ed182d972ec0b796041440a9a814f3220df96f2fdf18",
    "b29e7a2250fd0c67cea0fa76c54806b359dcc4da393c38123db322d47872aface373f375475db60b1046a81e38ff4679",
    "2ce2833619e004c9f0af468f78715b00bb13c919e2c5acc255b63b393f66976228c65334409008d5f778149004bd5953"
};
const char* TEST_HMAC_SHA512_OUT[] = {
    "641c129c18f163c5b137fc0ccf48c2313c928e5463a5528c0cc1e536670700add0155c6197224a4ef670e3f802964f4fe9b8ef3e42ceb1daae0624e8d74967d7",
    "6e23ad484d235c9412b8a48deaf998ea81a02e5d1004c790acbff63d081e3cdb00df6882f95c8b05a7b3fe154ba8dd878d8c6611c74570f3f8d0e838766d2020",
    "7b40cc431b837633ffb4ec8437bad459a966cd2603f2ecfa760506e4f319b2f9737da42fc1e0daa8fbc05ffe6af46aae32a87dd52be44c3b34daad47eecee690",
    "22ec766142a66e168f8c296eb12fda4a1ca48536649989d67a2bcfdac29c97137a0da9e4d44018a8e2454c6ad482710755b9958d23465e8c11c2a26fb6a47686"
};
const char* TEST_HMAC_SHA3224_OUT[] = {
    "73523ef652baca1950c9b9f80f4f203923b3d11ada8d832392deb32a",
    "4445a77c669b1eaf1e71c6c3848adf646799204cf61f0568199c7d33",
    "2aa427a936a93047a56a21c7e984583fcfc1bbd0ab72cacce0d0188e",
    "a913b59a1766326caa1a7535dd66d6c377ad32f4190056fba1f6d58b"
};
const char* TEST_HMAC_SHA3256_OUT[] = {
    "d6682f077105a83fe156f7b560cd0a7262edf17b8eb703bab268a972614e506a",
    "095067fe4cf7a162cd6d003516312a1cdfeeb2ddc3b5708664a9591ab6fb3959",
    "8bd3ad9da7b86ef541b4bc7e872daf066b6c6a18b58bebe004da2790b84cc068",
    "31f3a91b1158f3897185f2c1c4d0aeecf86531dc8f7e39944ef559ac915f9a33"
};
const char* TEST_HMAC_SHA3384_OUT[] = {
    "8a8c11c5938327be46c0e0c72233ada3775ba1ec14a1848d2fddcbd0a7ec57c98ea90378f77e9d2eaf2977965e3ec2d3",
    "c586f0d1ebf8b2d00da8321436df5a17f5e99ab32460b5aa9cb78f28c6c17ad8b32868900d9f65bcf63a0ed581995c68",
    "657da30466d183f3857853085a9ebd0edbcf2ea1afe2d45aa42c650826cc63c30a15130050ad245b5bac622b9bb62560",
    "faa09360ef0a5fda044af814cf972dd30d21e7cfb40acb552a92709ddb08138056278a2bb50d1a5bf5af02054ee423b6"
};
const char* TEST_HMAC_SHA3512_OUT[] = {
    "256b1a05934e99cfec36774453962ac32ba46f53dfd7056659582d9a9212ed827cb73979625d891d047ca475dff76d65f177e37bae977d77db506214545bdd35",
    "f05fcdf482447bec0cf2aa5d229b04f930d4c303263863cc79810e28cc3555a5da4d3b5e2b97579ffcb573683b072c4a041dbc1ff56a98de728e0e639101086b",
    "52740851b0d0a6546e4e3c170210c7945c1f5010b26bcc78dd0653bcd9ce7575405e25dfa84e928a1c8a9829a20828ba26853954c041c2b534b6c2055aa12c37",
    "b2afb89d60f86fd9a5d73ac8f74fdeda2fca35555e707b47af0ce44306f2b0e7df89bcf492d26ff3c4ff27cb44be55e1daf87f94c3b437b6af991af3aecb7332"
};

void HmacImp(int alg, const char** in, const char** out, int len) {
    int err = 0;
    MyHmac hmac;
    MyStringA result;

    for (int i = 0; i < len; i++) {
        err = hmac.QuickCalc(alg, TEST_HMAC_KEY, strlen(TEST_HMAC_KEY), in[i], strlen(in[i]));
        ASSERT_EQ(err, 0);

        MyEncodings::HexEncode(hmac.Result()->Deref(), hmac.Result()->Length(), &result, false);
        ASSERT_STREQ(result.Deref(), out[i]);
    }
}

TEST(MyHmacTest, MD4Test) {
     HmacImp(MYHASH_ALG_MD4, TEST_HMAC_IN, TEST_HMAC_MD4_OUT, 4);
}

TEST(MyHmacTest, MD5Test) {
    HmacImp(MYHASH_ALG_MD5, TEST_HMAC_IN, TEST_HMAC_MD5_OUT, 4);
}
TEST(MyHmacTest, SHA1Test) {
    HmacImp(MYHASH_ALG_SHA1, TEST_HMAC_IN, TEST_HMAC_SHA1_OUT, 4);
}
TEST(MyHmacTest, SHA224Test) {
    HmacImp(MYHASH_ALG_SHA224, TEST_HMAC_IN, TEST_HMAC_SHA224_OUT, 4);
}
TEST(MyHmacTest, SHA256Test) {
    HmacImp(MYHASH_ALG_SHA256, TEST_HMAC_IN, TEST_HMAC_SHA256_OUT, 4);
}
TEST(MyHmacTest, SHA384Test) {
    HmacImp(MYHASH_ALG_SHA384, TEST_HMAC_IN, TEST_HMAC_SHA384_OUT, 4);
}
TEST(MyHmacTest, SHA512Test) {
    HmacImp(MYHASH_ALG_SHA512, TEST_HMAC_IN, TEST_HMAC_SHA512_OUT, 4);
}
TEST(MyHmacTest, SHA3224Test) {
    HmacImp(MYHASH_ALG_SHA3224, TEST_HMAC_IN, TEST_HMAC_SHA3224_OUT, 4);
}
TEST(MyHmacTest, SHA3256Test) {
    HmacImp(MYHASH_ALG_SHA3256, TEST_HMAC_IN, TEST_HMAC_SHA3256_OUT, 4);
}
TEST(MyHmacTest, SHA33842Test) {
    HmacImp(MYHASH_ALG_SHA3384, TEST_HMAC_IN, TEST_HMAC_SHA3384_OUT, 4);
}
TEST(MyHmacTest, SHA3512Test) {
    HmacImp(MYHASH_ALG_SHA3512, TEST_HMAC_IN, TEST_HMAC_SHA3512_OUT, 4);
}
