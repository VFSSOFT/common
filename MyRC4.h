#ifndef _MY_RC4_H_
#define _MY_RC4_H_

#include <openssl/evp.h>
#include <openssl/rc4.h>

#include "MyBuffer.h"
#include "MyStringA.h"

class MyRC4 {
public:
    int Init(const char* key, int keyLen);
    int Update(const char* data, int dataLen);

    MyBuffer* Result() { return &m_Result; }

private:
    RC4_KEY m_Key;
    MyBuffer m_Result;
    MyBuffer m_TmpBuf;

    MY_LAST_ERROR_DECL;
};

#endif // _MY_RC4_H_
