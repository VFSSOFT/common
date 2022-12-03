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
    m_Aborted = false;
}
MyThread::~MyThread() {
    this->Abort();
}

int MyThread::Start() {
    DWORD threadId;
    HANDLE handle = CreateThread(
        NULL,
        0,
        MyThreadEntry,
        this,
        0,
        &threadId
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
    m_Aborted = true;
    if (m_ThreadHandle != NULL) {
        TerminateThread(m_ThreadHandle, 0);
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
    int err = 0;

    if (err = this->Run()) goto done;

done:
    if (err) {
        Abort();
    }
    return err;
}

#endif // _MY_THREAD_CPP_