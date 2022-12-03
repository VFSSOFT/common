#ifndef MY_HMAC_H_
#define MY_HMAC_H_

#define MY_HMAC_ALG_MD4        1
#define MY_HMAC_ALG_MD5        2
#define MY_HMAC_ALG_SHA1       3
#define MY_HMAC_ALG_SHA224     4
#define MY_HMAC_ALG_SHA256     5
#define MY_HMAC_ALG_SHA384     6
#define MY_HMAC_ALG_SHA512     7

class MyHmac {
public:

  static int CalcHmac(int alg, const char* key, int keyLen, const char* data, int dataLen, unsigned char** out, unsigned int* outLen);
};

#endif // MY_HMAC_H_