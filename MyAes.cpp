#include "MyAes.h"


MyAes::MyAes(): m_Ctx(NULL) {
    
}
MyAes::~MyAes() {
    Reset();
}

int MyAes::Init(int alg, const char* key, const char* iv, bool enc, int paddingMode) {
    int err = 0;
    const EVP_CIPHER* cipher;
    OSSL_PARAM params[2];
    OSSL_PARAM *pParams = NULL;
    
    Reset();
    m_Ctx = EVP_CIPHER_CTX_new();

    cipher = GetEvpCipherFromAlg(alg);
    if (cipher == NULL) {
        return LastError(MY_ERR_CRYPTO_ERROR, "Failed to init AES algorithm");
    }

    if (alg == MY_AES_128_CBC_CTS_CS3 || alg == MY_AES_256_CBC_CTS_CS3) {
        params[0] = OSSL_PARAM_construct_utf8_string(OSSL_CIPHER_PARAM_CTS_MODE, (char*)"CS3", 0);
        params[1] = OSSL_PARAM_construct_end();
        pParams = params;
    } else {
        pParams = NULL;
    }

    if (!EVP_CipherInit_ex2(m_Ctx, cipher, (const unsigned char*)key, (const unsigned char*)iv, enc ? AES_ENCRYPT : AES_DECRYPT, pParams)) {
        return LastError(MY_ERR_CRYPTO_ERROR, "Failed to init AES algorithm");
    }

    if (paddingMode != MY_PADDING_MODE_NONE) {
        EVP_CIPHER_CTX_set_padding(m_Ctx, paddingMode);
    }

    return 0;
}

int MyAes::Update(const char* data, int dataLen) {
    /* NOTE: CTS mode does not support multiple calls to EVP_CipherUpdate() */
    int outLen = dataLen;
    if (!EVP_CipherUpdate(m_Ctx, (unsigned char*)m_TmpBuf.Deref(), &outLen, (const unsigned char*)data, dataLen)) {
        return LastError(MY_ERR_CRYPTO_ERROR, "Failed to update AES");
    }
    m_Result.Append(m_TmpBuf.Deref(), outLen);
    return 0;
}
int MyAes::Final() {
    int outLen = 0;
    if (!EVP_CipherFinal_ex(m_Ctx, (unsigned char*)m_TmpBuf.Deref(), &outLen)) {
        return LastError(MY_ERR_CRYPTO_ERROR, "Failed to finalize AES");
    }
    m_Result.Append(m_TmpBuf.Deref(), outLen);
    return 0;
}


const EVP_CIPHER* MyAes::GetEvpCipherFromAlg(int alg) {
    switch (alg) {
    case MY_AES_128_CBC: return EVP_aes_128_cbc();
    case MY_AES_192_CBC: return EVP_aes_192_cbc();
    case MY_AES_256_CBC: return EVP_aes_256_cbc();

    case MY_AES_128_CFB: return EVP_aes_128_cfb();
    case MY_AES_192_CFB: return EVP_aes_192_cfb();
    case MY_AES_256_CFB: return EVP_aes_256_cfb();

    case MY_AES_128_CFB1: return EVP_aes_128_cfb1();
    case MY_AES_192_CFB1: return EVP_aes_192_cfb1();
    case MY_AES_256_CFB1: return EVP_aes_256_cfb1();

    case MY_AES_128_CFB8: return EVP_aes_128_cfb8();
    case MY_AES_192_CFB8: return EVP_aes_192_cfb8();
    case MY_AES_256_CFB8: return EVP_aes_256_cfb8();

    case MY_AES_128_CFB128: return EVP_aes_128_cfb128();
    case MY_AES_192_CFB128: return EVP_aes_192_cfb128();
    case MY_AES_256_CFB128: return EVP_aes_256_cfb128();

    case MY_AES_128_CTR: return EVP_aes_128_ctr();
    case MY_AES_192_CTR: return EVP_aes_192_ctr();
    case MY_AES_256_CTR: return EVP_aes_256_ctr();

    case MY_AES_128_ECB: return EVP_aes_128_ecb();
    case MY_AES_192_ECB: return EVP_aes_192_ecb();
    case MY_AES_256_ECB: return EVP_aes_256_ecb();

    case MY_AES_128_OFB: return EVP_aes_128_ofb();
    case MY_AES_192_OFB: return EVP_aes_192_ofb();
    case MY_AES_256_OFB: return EVP_aes_256_ofb();

    case MY_AES_128_CBC_HMAC_SHA1: return EVP_aes_128_cbc_hmac_sha1();
    case MY_AES_256_CBC_HMAC_SHA1: return EVP_aes_256_cbc_hmac_sha1();
    
    case MY_AES_128_CBC_HMAC_SHA256: return EVP_aes_128_cbc_hmac_sha256();
    case MY_AES_256_CBC_HMAC_SHA256: return EVP_aes_256_cbc_hmac_sha256();

    case MY_AES_128_CCM: return EVP_aes_128_ccm();
    case MY_AES_192_CCM: return EVP_aes_192_ccm();
    case MY_AES_256_CCM: return EVP_aes_256_ccm();

    case MY_AES_128_GCM: return EVP_aes_128_gcm();
    case MY_AES_192_GCM: return EVP_aes_192_gcm();
    case MY_AES_256_GCM: return EVP_aes_256_gcm();

    case MY_AES_128_OCB: return EVP_aes_128_ocb();
    case MY_AES_192_OCB: return EVP_aes_192_ocb();
    case MY_AES_256_OCB: return EVP_aes_256_ocb();

    case MY_AES_128_WRAP: return EVP_aes_128_wrap();
    case MY_AES_192_WRAP: return EVP_aes_192_wrap();
    case MY_AES_256_WRAP: return EVP_aes_256_wrap();

    case MY_AES_128_WRAP_PAD: return EVP_aes_128_wrap_pad();
    case MY_AES_192_WRAP_PAD: return EVP_aes_192_wrap_pad();
    case MY_AES_256_WRAP_PAD: return EVP_aes_256_wrap_pad();

    case MY_AES_128_XTS: return EVP_aes_128_xts();
    case MY_AES_256_XTS: return EVP_aes_256_xts();


    case MY_AES_128_CBC_CTS_CS3:
        return EVP_CIPHER_fetch(NULL, "AES-128-CBC-CTS", NULL);

    case MY_AES_256_CBC_CTS_CS3:
        return EVP_CIPHER_fetch(NULL, "AES-256-CBC-CTS", NULL);
    }
}
void MyAes::Reset() {
    m_Result.Reset();
    m_TmpBuf.Reset();
    m_TmpBuf.SetLength(65536);

    if (m_Ctx) {
        EVP_CIPHER_CTX_free(m_Ctx);
    }
    m_Ctx = NULL;
}
