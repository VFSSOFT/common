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
    if (m_Ctx == NULL)
        return LastError(MY_ERR_OUT_OF_MEMORY, "Out of memory");

    switch (alg) {
    case MYHASH_ALG_SHA1:
        if (!(success = EVP_DigestInit_ex(m_Ctx, EVP_sha1(), NULL))) {
            return LastError(MY_ERR_CRYPTO_ERROR, "Init SHA1 failed");
        }
        break;

    case MYHASH_ALG_SHA256:
        if (!(success = EVP_DigestInit_ex(m_Ctx, EVP_sha256(), NULL))) {
          return LastError(MY_ERR_CRYPTO_ERROR, "Init SHA256 failed");
        }
        break;

    case MYHASH_ALG_MD5:
          if (!(success = EVP_DigestInit_ex(m_Ctx, EVP_md5(), NULL))) {
              return LastError(MY_ERR_CRYPTO_ERROR, "Init MD5 failed");
          }
          break;

      case MYHASH_ALG_MD4:
          if (!(success = EVP_DigestInit_ex(m_Ctx, EVP_md4(), NULL))) {
              return LastError(MY_ERR_CRYPTO_ERROR, "Init MD5 failed");
          }
          break;

    default:
        return LastError(MY_ERR_CRYPTO_INVALID_HASH_ALG, "Unsupported hash algorithm");
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

int MyHash::CalcHash(int hashAlg, const char* data, int dataLen, unsigned char** out, unsigned int* outLen) {
  DECLARE_ERRCODE
  BOOL success = FALSE;
  EVP_MD_CTX *mdctx;

  mdctx = EVP_MD_CTX_new();
  if (mdctx == NULL)
    return MY_ERR_OUT_OF_MEMORY;

  switch (hashAlg) {
    case MYHASH_ALG_SHA1:
        if (!(success = EVP_DigestInit_ex(mdctx, EVP_sha1(), NULL))) {
            errCode = MY_ERR_CRYPTO_ERROR;
            goto done;
        }
        break;

  case MYHASH_ALG_SHA256:
    if (!(success = EVP_DigestInit_ex(mdctx, EVP_sha256(), NULL))) {
      errCode = MY_ERR_CRYPTO_ERROR;
      goto done;
    }
    break;

  case MYHASH_ALG_MD5:
      if (!(success = EVP_DigestInit_ex(mdctx, EVP_md5(), NULL))) {
          errCode= MY_ERR_CRYPTO_ERROR;
          goto done;
      }
      break;

  case MYHASH_ALG_MD4:
      if (!(success = EVP_DigestInit_ex(mdctx, EVP_md4(), NULL))) {
          errCode= MY_ERR_CRYPTO_ERROR;
          goto done;
      }
      break;

  default:
    errCode = MY_ERR_CRYPTO_INVALID_HASH_ALG;
    goto done;
  }

  if (!(success = EVP_DigestUpdate(mdctx, data, dataLen))) {
    errCode = MY_ERR_CRYPTO_ERROR;
    goto done;
  }

  if (!(success = EVP_DigestFinal(mdctx, *out, outLen))) {
    errCode = MY_ERR_CRYPTO_ERROR;
    goto done;
  }

done:
  EVP_MD_CTX_free(mdctx);
  return errCode;
  
}
