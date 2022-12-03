#include "MyHMAC.h"

#include <openssl/ossl_typ.h>
#include <openssl/evp.h>
#include <openssl/hmac.h>

#include "../common/MyCoreDef.h"

int MyHmac::CalcHmac(int alg, const char* key, int keyLen, const char* data, int dataLen, unsigned char** out, unsigned int* outLen) {
  DECLARE_ERRCODE
  BOOL success = FALSE;
  HMAC_CTX* ctx;

  ctx = HMAC_CTX_new();
  if (ctx == NULL)
    return MY_ERR_OUT_OF_MEMORY;

  switch (alg) {

  case MY_HMAC_ALG_MD4:
    if (!(success = HMAC_Init_ex(ctx, key, keyLen, EVP_md4(), NULL))) {
      errCode = MY_ERR_CRYPTO_ERROR;
      goto done;
    }
    break;

  case MY_HMAC_ALG_MD5:
    if (!(success = HMAC_Init_ex(ctx, key, keyLen, EVP_md5(), NULL))) {
      errCode = MY_ERR_CRYPTO_ERROR;
      goto done;
    }
    break;

  case MY_HMAC_ALG_SHA1:
    if (!(success = HMAC_Init_ex(ctx, key, keyLen, EVP_sha1(), NULL))) {
      errCode = MY_ERR_CRYPTO_ERROR;
      goto done;
    }
    break;

  case MY_HMAC_ALG_SHA224:
    if (!(success = HMAC_Init_ex(ctx, key, keyLen, EVP_sha224(), NULL))) {
      errCode = MY_ERR_CRYPTO_ERROR;
      goto done;
    }
    break;

	case MY_HMAC_ALG_SHA256:
    if (!(success = HMAC_Init_ex(ctx, key, keyLen, EVP_sha256(), NULL))) {
      errCode = MY_ERR_CRYPTO_ERROR;
      goto done;
    }
    break;

  case MY_HMAC_ALG_SHA384:
    if (!(success = HMAC_Init_ex(ctx, key, keyLen, EVP_sha384(), NULL))) {
      errCode = MY_ERR_CRYPTO_ERROR;
      goto done;
    }
    break;

  case MY_HMAC_ALG_SHA512:
    if (!(success = HMAC_Init_ex(ctx, key, keyLen, EVP_sha512(), NULL))) {
      errCode = MY_ERR_CRYPTO_ERROR;
      goto done;
    }
    break;

  default:
    errCode = MY_ERR_CRYPTO_INVALID_HMAC_ALG;
    goto done;
  }

  if (!(success = HMAC_Update(ctx, (const unsigned char*)data, dataLen))) {
    errCode = MY_ERR_CRYPTO_ERROR;
    goto done;
  }

  if (!(success = HMAC_Final(ctx, *out, outLen))) {
    errCode = MY_ERR_CRYPTO_ERROR;
    goto done;
  }

done:
  HMAC_CTX_free(ctx);
  return errCode;

}