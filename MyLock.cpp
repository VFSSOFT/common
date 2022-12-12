#ifndef _MY_LOCK_CPP_
#define _MY_LOCK_CPP_

#include "MyLock.h"


MyLock::MyLock() {
    INIT_LAST_ERROR;
}
MyLock::~MyLock() {}


void MyLock::Acquire() {
    m_CriticalSection.Acquire();
}
void MyLock::Release() {
    m_CriticalSection.Release();
}
int MyLock::Wait(int timeoutMS) {
    int err = 0;
    if (err = m_ConditionVariable.Wait(&m_CriticalSection, timeoutMS, NULL)) {
        return LastError(err, m_ConditionVariable.LastErrorMessage());
    }
    return 0;
}
void MyLock::Notify() {
    m_ConditionVariable.Notify(&m_CriticalSection);
}
void MyLock::NotifyAll() {
    m_ConditionVariable.NotifyAll(&m_CriticalSection);
}


#endif // _MY_LOCK_CPP_
