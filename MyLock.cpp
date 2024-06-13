#ifndef _MY_LOCK_CPP_
#define _MY_LOCK_CPP_

#include "MyLock.h"


MyLock::MyLock() {
#if _DEBUG
    m_LockCount = 0;
    memset(m_LockThreads, 0, sizeof(DWORD) * 64);
#endif
}
MyLock::~MyLock() {}


void MyLock::Acquire() {
    m_CriticalSection.Acquire();
#if _DEBUG
    MYASSERT(m_LockCount < 64, "LockCount >= 0");
    m_LockThreads[m_LockCount++] = GetCurrentThreadId();
#endif
}
void MyLock::Release() {
#if _DEBUG
    m_LockThreads[--m_LockCount] = 0;
    MYASSERT(m_LockCount >= 0, "LockCount < 0");
#endif
    m_CriticalSection.Release();
}


#endif // _MY_LOCK_CPP_
