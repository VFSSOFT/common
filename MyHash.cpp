#include "MyHash.h"

#include "../common/MyCoreDef.h"
#include <openssl/evp.h>
#include <openssl/ossl_typ.h>


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
