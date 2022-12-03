#ifndef _MY_LOCK_CPP_
#define _MY_LOCK_CPP_

#include "MyLock.h"


MyLock::MyLock() {
    InitializeCriticalSection(&m_CriticalSection);
}
MyLock::~MyLock() {
    DeleteCriticalSection(&m_CriticalSection);
}

void MyLock::Lock() {
    EnterCriticalSection(&m_CriticalSection);
}
void MyLock::Unlock() {
    LeaveCriticalSection(&m_CriticalSection);
}


#endif // _MY_LOCK_CPP_
