#ifndef _MY_LOCK_H_
#define _MY_LOCK_H_

#include "MyCoreDef.h"

class MyLock {


public:
    MyLock();
    ~MyLock();

    void Lock();
    void Unlock();

private:

    CRITICAL_SECTION m_CriticalSection;
};

#endif // _MY_LOCK_H_
