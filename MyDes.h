#ifndef _MY_DES_H_
#define _MY_DES_H_

#include "MyBuffer.h"
#include "MyStringA.h"

#include <openssl/des.h>

class MyDes {
public:
    
    int InitDES3(const char* iv, const char* key, int keyLen);
    int Des3CbcEncrypt(const char* data, int dataLen, MyBuffer* ret);
    int Des3CbcDecrypt(const char* cipher, int cipherLen, MyBuffer* ret);

private:
    DES_key_schedule m_Des3KSs[3];
    DES_cblock       m_IV;
    DES_cblock       m_Des3CBs[3];

    MY_LAST_ERROR_DECL;
};


#endif // _MY_DES_H_

