#include "MyRC4.h"


int MyRC4::Init(const char* key, int keyLen) {
    RC4_set_key(&m_Key, keyLen, (const unsigned char*)key);
    m_Result.Reset();
    return 0;
}
int MyRC4::Update(const char* data, int dataLen) {
    m_TmpBuf.Set(data, dataLen);
    RC4(&m_Key, dataLen, (const unsigned char*)data, (unsigned char*)m_TmpBuf.Deref());
    m_Result.Append(m_TmpBuf.Deref(), dataLen);
    return 0;
}