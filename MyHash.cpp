#include "MyHash.h"



MyHash::MyHash(): m_Ctx(NULL) {

}
MyHash::~MyHash() {
    if (m_Ctx) {
        EVP_MD_CTX_free(m_Ctx);
    }
    m_Ctx = NULL;
}
int MyHash::Init(int alg) {
    int err = 0;
    int success = 0; 

    m_Ctx = EVP_MD_CTX_new();
    const EVP_MD* md = GetEvpMD(alg);

    if (md == NULL) {
        m_LastErrorMessage.SetWithFormat("Init Hash failed: unkonwn algorithm(%d)", alg);
        m_LastErrorCode = MY_ERR_CRYPTO_INVALID_HASH_ALG;
        return m_LastErrorCode;
    }

    if (!EVP_DigestInit_ex(m_Ctx, md, NULL)) {
        return LastError(MY_ERR_CRYPTO_ERROR, "Init Hash failed");
    }

    return 0;
}
int MyHash::Update(const char* b, int bLen) {
    int success = EVP_DigestUpdate(m_Ctx, b, bLen);
    if (!success) {
        return LastError(MY_ERR_CRYPTO_ERROR, "Failed to calculate hash");
    }
    return 0;
}
int MyHash::Finish() {
    unsigned int size = EVP_MD_CTX_get_size(m_Ctx);
    m_HashValue.SetLength(size);

    int success = EVP_DigestFinal(m_Ctx, (unsigned char*)m_HashValue.Deref(), &size);
    if (!success) {
        return LastError(MY_ERR_CRYPTO_ERROR, "Failed to calculate hash");
    }
    return 0;
}

int MyHash::QuickCalc(int alg, const char* b, int bLen) {
    int err = 0;
    if (err = Init(alg)) return err;
    if (err = Update(b, bLen)) return err;
    if (err = Finish()) return err;
    return 0;
}

int MyHash::GetHashSize(int hashAlg) {
    switch (hashAlg) {
    case MYHASH_ALG_MD4: return 16;
    case MYHASH_ALG_MD5: return 16;
    case MYHASH_ALG_SHA1: return 20;
    case MYHASH_ALG_SHA256: return 32;
    }
    return 0;
}

const EVP_MD* MyHash::GetEvpMD(int hashAlg) {
    switch (hashAlg) {
    case MYHASH_ALG_MD4:     return EVP_md4();
    case MYHASH_ALG_MD5:     return EVP_md5();
    case MYHASH_ALG_SHA1:    return EVP_sha1();
    case MYHASH_ALG_SHA224:  return EVP_sha224();
    case MYHASH_ALG_SHA256:  return EVP_sha256();
    case MYHASH_ALG_SHA384:  return EVP_sha384();
    case MYHASH_ALG_SHA512:  return EVP_sha512();
    case MYHASH_ALG_SHA3384: return EVP_sha3_384();
    }
    return NULL;
}

