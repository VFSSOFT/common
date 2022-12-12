#ifndef _MY_LOCK_H_
#define _MY_LOCK_H_

#include "MyCoreDef.h"

#include "MyCriticalSection.h"
#include "MyConditionVariable.h"

class MyLock {


public:
    MyLock();
    ~MyLock();

    void Acquire();
    void Release();
    int  Wait(int timeoutMS);
    void Notify();
    void NotifyAll();

private:

    MyCriticalSection   m_CriticalSection;
    MyConditionVariable m_ConditionVariable;

    MY_LAST_ERROR_DECL;
};

#endif // _MY_LOCK_H_
