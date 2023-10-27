#ifndef _MY_LOCK_CPP_
#define _MY_LOCK_CPP_

#include "MyLock.h"


MyLock::MyLock() {
}
MyLock::~MyLock() {}


void MyLock::Acquire() {
    m_CriticalSection.Acquire();
}
void MyLock::Release() {
    m_CriticalSection.Release();
}


#endif // _MY_LOCK_CPP_
