#ifndef _MY_THREAD_CPP_
#define _MY_THREAD_CPP_

#include "MyThread.h"
#include "MyWin.h"

DWORD WINAPI MyThreadEntry(LPVOID lpParam) {
    MyThread* t = (MyThread*)lpParam;
    return t->ThreadEntry();
}

MyThread::MyThread() {
    INIT_LAST_ERROR;
    m_ThreadHandle = NULL;
    m_ThreadId = -1;
}
MyThread::~MyThread() {
    this->Abort();
}

int MyThread::Start() {
    m_State.SetState(MY_THREAD_STATE_IDLE);
    HANDLE handle = CreateThread(
        NULL,
        0,
        MyThreadEntry,
        this,
        0,
        &m_ThreadId
    );

    if (handle == NULL) {
        m_LastErrorCode = MyWin::GetSysLastErrorCode();
        MyWin::GetSysLastErrorMessage(&m_LastErrorMessage, m_LastErrorCode);
        return m_LastErrorCode;
    }

    m_ThreadHandle = handle;
    return 0;
}
int MyThread::Abort() {
    m_State.SetAborting();

    if (m_ThreadHandle != NULL) {
        if (!WaitForExit(30 * 1000)) {
            //TerminateThread(m_ThreadHandle, 0);
            assert(false); // all thread should be aborted gracefully
        }
        CloseHandle(m_ThreadHandle);
        m_ThreadHandle = NULL;
    }

    return 0;
}

bool MyThread::WaitForExit(int millis) {
    if (m_ThreadHandle == NULL) return true;

    DWORD result = WaitForSingleObject(m_ThreadHandle, millis);
    if (result == WAIT_OBJECT_0) return true;
    return false;
}

int MyThread::ThreadEntry() {
    if (m_State.IsIdle()) {
        m_State.SetRunning();
    } else if (m_State.IsRunning()) {
        assert(false);
    } else if (m_State.IsAborting()) {
        m_State.SetIdle();
        return 0;
    }
    int err = this->Run();
    m_State.SetIdle();
    return err;
}

UINT64 MyThread::ThreadId() {
    return GetCurrentThreadId();
}

#endif // _MY_THREAD_CPP_
