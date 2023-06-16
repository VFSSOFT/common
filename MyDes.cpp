#include "MyDES.h"

int MyDes::InitDES3(const char* iv, const char* key, int keyLen) {
    int err = 0;

    memcpy(&m_IV, iv, 8);
    memcpy(&m_Des3CBs[0], key, 8);
    memcpy(&m_Des3CBs[1], key + 8, 8);
    memcpy(&m_Des3CBs[2], key + 16, 8);

    //DES_set_odd_parity(&m_IV);

    err = DES_set_key(&m_Des3CBs[0], &m_Des3KSs[0]);
    err = DES_set_key(&m_Des3CBs[1], &m_Des3KSs[1]);
    err = DES_set_key(&m_Des3CBs[2], &m_Des3KSs[2]);

    return 0;
}
int MyDes::Des3CbcEncrypt(const char* data, int dataLen, MyBuffer* ret) {
    ret->SetLength(dataLen);
    DES_ede3_cbc_encrypt((const unsigned char*)data, (unsigned char*)ret->Deref(), dataLen, &m_Des3KSs[0], &m_Des3KSs[1], &m_Des3KSs[2], &m_IV, DES_ENCRYPT);
    return 0;
}
int MyDes::Des3CbcDecrypt(const char* cipher, int cipherLen, MyBuffer* ret) {
    ret->SetLength(cipherLen);
    DES_ede3_cbc_encrypt((const unsigned char*)cipher, (unsigned char*)ret->Deref(), cipherLen, &m_Des3KSs[0], &m_Des3KSs[1], &m_Des3KSs[2], &m_IV, DES_DECRYPT);
    return 0;
}
