#ifndef _MY_THREAD_H_
#define _MY_THREAD_H_

#include "MyStringA.h"
#include "MyLock.h"

#define MY_THREAD_STATE_IDLE     0
#define MY_THREAD_STATE_RUNNING  1
#define MY_THREAD_STATE_ABORTING 2

class MyThreadState {
public:
    MyThreadState(): m_State(MY_THREAD_STATE_IDLE) {}

    int State()            { return m_State; }
    void SetState(int val) { m_State = val;  }

    void SetIdle()     { m_State = MY_THREAD_STATE_IDLE;     }
    void SetRunning()  { m_State = MY_THREAD_STATE_RUNNING;  }
    void SetAborting() { m_State = MY_THREAD_STATE_ABORTING; }

    bool IsIdle()     { return m_State == MY_THREAD_STATE_IDLE; }
    bool IsRunning()  { return m_State == MY_THREAD_STATE_RUNNING; }
    bool IsAborting() { return m_State == MY_THREAD_STATE_ABORTING; }

private:
    int  m_State;
};

class MyThread {

public:
    MyThread();
    virtual ~MyThread();

    bool Aborted() { return m_State.IsAborting(); }
    int  State() { return m_State.State(); }

    int Start();
    virtual int Abort();
    virtual int Abort(int timeoutMS);
    bool WaitForExit(int millis);
    virtual int Run() = 0;

    int ThreadEntry();

    UINT64 ThreadId();
    MyStringA* ThreadName() { return &m_ThreadName; }

private:
    MyThreadState m_State;
    HANDLE        m_ThreadHandle;
    DWORD         m_ThreadId; // It helps debug, so keep it.
    MyStringA     m_ThreadName;
    MY_LAST_ERROR_DECL;
};

#endif // _MY_THREAD_H_
