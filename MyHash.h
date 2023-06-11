#ifndef MY_HASH_H_
#define MY_HASH_H_

#include <openssl/evp.h>
#include <openssl/ossl_typ.h>
#include "MyStringA.h"
#include "MyBuffer.h"

#define MYHASH_ALG_SHA256 1
#define MYHASH_ALG_SHA1   2
#define MYHASH_ALG_MD5    3
#define MYHASH_ALG_MD4    4

class MyHash {
public:
  MyHash();
  virtual ~MyHash();

  int Init(int alg);
  int Update(const char* b, int bLen);
  int Finish();
  MyBuffer* Result() { return &m_HashValue; }

  static int CalcHash(int hashAlg, const char* data, int dataLen, unsigned char** out, unsigned int* outLen);

  static int GetHashSize(int hashAlg);

private:
    EVP_MD_CTX* m_Ctx;
    MyBuffer    m_HashValue;

    MY_LAST_ERROR_DECL;
};

#endif // MY_HASH_H_