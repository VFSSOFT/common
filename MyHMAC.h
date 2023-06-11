#ifndef MY_HMAC_H_
#define MY_HMAC_H_

#include <openssl/ossl_typ.h>
#include <openssl/evp.h>
#include <openssl/hmac.h>

#include "MyStringA.h"
#include "MyBuffer.h"
#include "MyHash.h"

class MyHmac {
public:

    MyHmac();
    ~MyHmac();

    int Init(int alg, const char* key, int keyLen);
    int Update(const char* data, int dataLen);
    int Final();
    void Reset();
    MyBuffer* Result() { return &m_Result; }

    int QuickCalc(int alg, const char* key, int keyLen, const char* data, int dataLen);

private:
    HMAC_CTX* m_Ctx;
    MyBuffer  m_Result;

    MY_LAST_ERROR_DECL;
};

#endif // MY_HMAC_H_