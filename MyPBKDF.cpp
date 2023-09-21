#include "MyPBKDF.h"


int MyPBKDF::PBKDF2(int hmacAlg, const char* pass, int passLen, const char* salt, int saltLen, int iter, int keyLen, char* buf) {
    int success = 0;
    switch (hmacAlg) {
    case MYHASH_ALG_MD4:
        success = PKCS5_PBKDF2_HMAC(pass, passLen, (const unsigned char*)salt, saltLen, iter, EVP_md4(), keyLen, (unsigned char*)buf);
        break;

    case MYHASH_ALG_MD5:
        success = PKCS5_PBKDF2_HMAC(pass, passLen, (const unsigned char*)salt, saltLen, iter, EVP_md5(), keyLen, (unsigned char*)buf);
        break;

    case MYHASH_ALG_SHA1:
        success = PKCS5_PBKDF2_HMAC(pass, passLen, (const unsigned char*)salt, saltLen, iter, EVP_sha1(), keyLen, (unsigned char*)buf);
        break;

    case MYHASH_ALG_SHA224:
        success = PKCS5_PBKDF2_HMAC(pass, passLen, (const unsigned char*)salt, saltLen, iter, EVP_sha224(), keyLen, (unsigned char*)buf);
        break;

    case MYHASH_ALG_SHA256:
        success = PKCS5_PBKDF2_HMAC(pass, passLen, (const unsigned char*)salt, saltLen, iter, EVP_sha256(), keyLen, (unsigned char*)buf);
        break;

    case MYHASH_ALG_SHA384:
        success = PKCS5_PBKDF2_HMAC(pass, passLen, (const unsigned char*)salt, saltLen, iter, EVP_sha384(), keyLen, (unsigned char*)buf);
        break;

    case MYHASH_ALG_SHA512:
        success = PKCS5_PBKDF2_HMAC(pass, passLen, (const unsigned char*)salt, saltLen, iter, EVP_sha512(), keyLen, (unsigned char*)buf);
        break;

    default:
        return MY_ERR_CRYPTO_INVALID_HMAC_ALG;
    }

    if (!success) {
        return MY_ERR_CRYPTO_INVALID_HMAC_ALG;
    }
    return 0;
}