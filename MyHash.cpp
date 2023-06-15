#include "MyHash.h"



MyHash::MyHash(): m_Ctx(NULL) {
    memset(&m_MD4Ctx, 0, sizeof(MD4_CTX));
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

    if (alg == MYHASH_ALG_MD4) {
        MD4_Init(&m_MD4Ctx);
    } else {
        m_Ctx = EVP_MD_CTX_new();
        const EVP_MD* md = GetEvpMD(alg);

        if (md == NULL) {
            m_LastErrorMessage.SetWithFormat("Init Hash failed: unkonwn algorithm(%d)", alg);
            m_LastErrorCode = MY_ERR_CRYPTO_INVALID_HASH_ALG;
            return m_LastErrorCode;
        }

        if (!EVP_DigestInit_ex(m_Ctx, md, NULL)) {
            //ERR_print_errors_fp(stderr);
            return LastError(MY_ERR_CRYPTO_ERROR, "Init Hash failed");
        }
    }

    return 0;
}
int MyHash::Update(const char* b, int bLen) {
    int success = 0;
    if (m_MD4Ctx.A > 0) {
        success = MD4_Update(&m_MD4Ctx, b, bLen);
    } else {
        success = EVP_DigestUpdate(m_Ctx, b, bLen);
    }
    if (!success) {
        return LastError(MY_ERR_CRYPTO_ERROR, "Failed to calculate hash");
    }
    return 0;
}
int MyHash::Finish() {
    int success;

    if (m_MD4Ctx.A > 0) {
        m_HashValue.SetLength(16);
        success = MD4_Final((unsigned char*)m_HashValue.Deref(), &m_MD4Ctx);
    } else {
        unsigned int size = EVP_MD_CTX_get_size(m_Ctx);
        m_HashValue.SetLength(size);
        success = EVP_DigestFinal(m_Ctx, (unsigned char*)m_HashValue.Deref(), &size);
    }
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
    case MYHASH_ALG_MD4:       return EVP_md4();
    case MYHASH_ALG_MD5:       return EVP_md5();
    case MYHASH_ALG_SHA1:      return EVP_sha1();
    case MYHASH_ALG_SHA224:    return EVP_sha224();
    case MYHASH_ALG_SHA256:    return EVP_sha256();
    case MYHASH_ALG_SHA384:    return EVP_sha384();
    case MYHASH_ALG_SHA512:    return EVP_sha512();
    case MYHASH_ALG_SHA3224:   return EVP_sha3_224();
    case MYHASH_ALG_SHA3256:   return EVP_sha3_256();
    case MYHASH_ALG_SHA3384:   return EVP_sha3_384();
    case MYHASH_ALG_SHA3512:   return EVP_sha3_512();
    case MYHASH_ALG_SHA512224: return EVP_sha512_224();
    case MYHASH_ALG_SHA512256: return EVP_sha512_256();
    }
    return NULL;
}

