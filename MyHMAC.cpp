#include "MyHMAC.h"

MyHmac::MyHmac(): m_Ctx(NULL) {

}
MyHmac::~MyHmac() {
    Reset();
}

int MyHmac::Init(int alg, const char* key, int keyLen) {
    int err = 0;

    Reset();
    m_Ctx = HMAC_CTX_new();
    const EVP_MD* md = MyHash::GetEvpMD(alg);

    if (md == NULL) {
        m_LastErrorMessage.SetWithFormat("Invalid hmac algorithm: %d", alg);
        m_LastErrorCode = MY_ERR_CRYPTO_INVALID_HMAC_ALG;
        return m_LastErrorCode;
    }

    if (!HMAC_Init_ex(m_Ctx, key, keyLen, md, NULL)) {
        return LastError(MY_ERR_CRYPTO_ERROR, "Failed to init HMAC");
    }

    return 0;
}
int MyHmac::Update(const char* data, int dataLen) {
    if (!HMAC_Update(m_Ctx, (const unsigned char*)data, dataLen)) {
        return LastError(MY_ERR_CRYPTO_ERROR, "Failed to calc HMAC");
    }
    return 0;
}
int MyHmac::Final() {
    unsigned int hmacSize = HMAC_size(m_Ctx);
    m_Result.SetLength(hmacSize);
    if (!HMAC_Final(m_Ctx, (unsigned char*)m_Result.Deref(), &hmacSize)) {
        return LastError(MY_ERR_CRYPTO_ERROR, "Failed to calc HMAC");
    }
    return 0;
}
void MyHmac::Reset() {
    m_Result.Reset();
    if (m_Ctx) {
        HMAC_CTX_free(m_Ctx);
    }
    m_Ctx = NULL;
}

int MyHmac::QuickCalc(int alg, const char* key, int keyLen, const char* data, int dataLen) {
    int err = 0;
    if (err = Init(alg, key, keyLen)) return err;
    if (err = Update(data, dataLen)) return err;
    if (err = Final()) return err;
    return 0;
}

