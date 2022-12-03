#ifndef _MY_MUTEX_H_
#define _MY_MUTEX_H_

#include "MyCoreDef.h"
#include "MyStringA.h"

class MyMutex {
public:
    MyMutex();
    ~MyMutex();

    int Init();

    int Acquire(int timeoutMS=-1, bool* timedout=NULL);
    void Release();

    int Wait();

private:
    HANDLE m_SysMutex;

    MY_LAST_ERROR_DECL;
};

#endif // _MY_MUTEX_H_
