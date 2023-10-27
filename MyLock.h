#ifndef _MY_LOCK_H_
#define _MY_LOCK_H_

#include "MyCoreDef.h"

#include "MyCriticalSection.h"

class MyLock {


public:
    MyLock();
    ~MyLock();

    void Acquire();
    void Release();

private:

    MyCriticalSection   m_CriticalSection;
};

#endif // _MY_LOCK_H_
