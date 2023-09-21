#ifndef _MY_PBKDF_H_
#define _MY_PBKDF_H_

#include <openssl/evp.h>
#include <openssl/sha.h>

#include "MyBuffer.h"
#include "MyHMAC.h"

class MyPBKDF {
public:
    static int PBKDF2(int hmacAlg, const char* pass, int passLen, const char* salt, int saltLen, int iter, int keyLen, char* buf);
};

#endif // _MY_PBKDF_H_
