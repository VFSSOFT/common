#ifndef _MY_AES_H_
#define _MY_AES_H_

#include <openssl/evp.h>
#include <openssl/aes.h>
#include <openssl/modes.h>
#include <openssl/core_names.h>

#include "MyStringA.h"
#include "MyBuffer.h"


#define MY_AES_128_CBC 1
#define MY_AES_192_CBC 2
#define MY_AES_256_CBC 3

#define MY_AES_128_CFB 4
#define MY_AES_192_CFB 5
#define MY_AES_256_CFB 6

#define MY_AES_128_CFB1 7
#define MY_AES_192_CFB1 8
#define MY_AES_256_CFB1 9

#define MY_AES_128_CFB8 10
#define MY_AES_192_CFB8 11
#define MY_AES_256_CFB8 12

#define MY_AES_128_CFB128 13
#define MY_AES_192_CFB128 14
#define MY_AES_256_CFB128 15

#define MY_AES_128_CTR 16
#define MY_AES_192_CTR 17
#define MY_AES_256_CTR 18

#define MY_AES_128_ECB 19
#define MY_AES_192_ECB 20
#define MY_AES_256_ECB 21

#define MY_AES_128_OFB 22
#define MY_AES_192_OFB 23
#define MY_AES_256_OFB 24

#define MY_AES_128_CBC_HMAC_SHA1 25
#define MY_AES_256_CBC_HMAC_SHA1 26

#define MY_AES_128_CBC_HMAC_SHA256 27
#define MY_AES_256_CBC_HMAC_SHA256 28

#define MY_AES_128_CCM             29
#define MY_AES_192_CCM             30
#define MY_AES_256_CCM             31

#define MY_AES_128_GCM             32
#define MY_AES_192_GCM             33
#define MY_AES_256_GCM             34

#define MY_AES_128_OCB             35
#define MY_AES_192_OCB             36
#define MY_AES_256_OCB             37

#define MY_AES_128_WRAP            38
#define MY_AES_192_WRAP            39
#define MY_AES_256_WRAP            40

#define MY_AES_128_WRAP_PAD        41
#define MY_AES_192_WRAP_PAD        42
#define MY_AES_256_WRAP_PAD        43

#define MY_AES_128_XTS             44
#define MY_AES_256_XTS             45


// ref: https://mailman.mit.edu/pipermail/cvs-krb5/2021-November/015361.html
#define MY_AES_128_CBC_CTS_CS3     101
#define MY_AES_256_CBC_CTS_CS3     102

#define MY_PADDING_MODE_NONE       0
#define MY_PADDING_MODE_PKCS7      1
#define MY_PADDING_MODE_ISO7816    2
#define MY_PADDING_MODE_ANSI923    3
#define MY_PADDING_MODE_ISO10126   4
#define MY_PADDING_MODE_ZERO       5

class MyAes {
public:
    MyAes();
    ~MyAes();

    int Init(int alg, const char* key, const char* iv, bool enc, int paddingMode);
    int Update(const char* data, int dataLen);
    int Final();
    MyBuffer* Result() { return &m_Result; }


private:
    const EVP_CIPHER* GetEvpCipherFromAlg(int alg);
    void Reset();

private:
    EVP_CIPHER_CTX* m_Ctx;
    MyBuffer        m_TmpBuf;
    MyBuffer        m_Result;

    MY_LAST_ERROR_DECL;
    
};


#endif // _MY_AES_H_

