#ifndef MY_HASH_H_
#define MY_HASH_H_

#include <openssl/evp.h>
#include <openssl/ossl_typ.h>
#include "MyStringA.h"
#include "MyBuffer.h"

#define MYHASH_ALG_MD4       0
#define MYHASH_ALG_MD5       1
#define MYHASH_ALG_SHA1      2
#define MYHASH_ALG_SHA224    3
#define MYHASH_ALG_SHA256    4
#define MYHASH_ALG_SHA384    5
#define MYHASH_ALG_SHA512    6
#define MYHASH_ALG_SHA3384   7


class MyHash {
public:
  MyHash();
  virtual ~MyHash();

  int Init(int alg);
  int Update(const char* b, int bLen);
  int Finish();
  
  int QuickCalc(int alg, const char* b, int bLen);

  MyBuffer* Result() { return &m_HashValue; }


  static int GetHashSize(int hashAlg);
  static const EVP_MD* GetEvpMD(int hashAlg);

private:
    EVP_MD_CTX* m_Ctx;
    MyBuffer    m_HashValue;

    MY_LAST_ERROR_DECL;
};

#endif // MY_HASH_H_