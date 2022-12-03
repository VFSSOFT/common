#ifndef _MY_MUTEX_CPP_
#define _MY_MUTEX_CPP_

#include "MyMutex.h"
#include "MyWin.h"

MyMutex::MyMutex() {
    INIT_LAST_ERROR;
    m_SysMutex = NULL;
}

MyMutex::~MyMutex() {
    if (m_SysMutex) {
        CloseHandle(m_SysMutex);
        m_SysMutex = NULL;
    }
}

int MyMutex::Init() {
    int err = 0;
    m_SysMutex = CreateMutex(
        NULL,  // default security attributes
        FALSE, // initially not owned
        NULL   // unmaned mutex 
    );
    if (m_SysMutex == NULL) {
        m_LastErrorCode = MyWin::GetSysLastErrorCode();
        MyWin::GetSysLastErrorMessage(&m_LastErrorMessage, m_LastErrorCode);
        return m_LastErrorCode;
    }
    return 0;
}

int MyMutex::Acquire(int timeoutMS, bool* timedout) {
    DWORD waitResult = 0;
    waitResult = WaitForSingleObject(
        m_SysMutex,
        timeoutMS <= 0 ? INFINITE : timeoutMS
    );

    switch (waitResult) {
    case WAIT_ABANDONED:
        return LastError(WAIT_ABANDONED, "The specified object is a mutex object that was not released by the thread that owned the mutex object before the owning thread terminated.");

    case WAIT_OBJECT_0: // The state of the specified object is signaled.
        if (timedout) *timedout = false;
        return 0;

    case WAIT_TIMEOUT: // The time-out interval elapsed, and the object's state is nonsignaled.
        *timedout = true;
        return 0;

    default:
        m_LastErrorCode = MyWin::GetSysLastErrorCode();
        MyWin::GetSysLastErrorMessage(&m_LastErrorMessage, m_LastErrorCode);
        return m_LastErrorCode;
    }

}
void MyMutex::Release() {
    BOOL releaseResult = ReleaseMutex(m_SysMutex);
    assert(releaseResult == 0);
}

int MyMutex::Wait() {
    int err = 0;

    Release();
    if (err = Acquire()) return err;

    return 0;
}

#endif // _MY_MUTEX_CPP_
