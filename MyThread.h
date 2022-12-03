#ifndef _MY_THREAD_H_
#define _MY_THREAD_H_

#include "MyStringA.h"

class MyThread {

public:
    MyThread();
    virtual ~MyThread();

    bool Aborted() { return m_Aborted; }

    int Start();
    int Abort();
    bool WaitForExit(int millis);
    virtual int Run() = 0;

    int ThreadEntry();

private:
    bool   m_Aborted;
    HANDLE m_ThreadHandle;
    MY_LAST_ERROR_DECL;
};

#endif // _MY_THREAD_H_
