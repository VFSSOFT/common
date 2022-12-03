#ifndef _MY_CONDITION_VARIABLE_CPP_
#define _MY_CONDITION_VARIABLE_CPP_

#include "MyConditionVariable.h"
#include "MyWin.h"

MyConditionVariable::MyConditionVariable() {
    INIT_LAST_ERROR;
    InitializeConditionVariable(&m_ConditionVariable);
}
MyConditionVariable::~MyConditionVariable() { }

int MyConditionVariable::Wait(MyCriticalSection* cs, int timeoutMS, bool* timeout) {
    if (timeout) *timeout = false;

    BOOL ret = SleepConditionVariableCS(&m_ConditionVariable, cs->CS(), timeoutMS);
    if (ret == 0) {
        DWORD lastErr = ::GetLastError();
        if (lastErr == ERROR_TIMEOUT) {
            if (timeout) *timeout = true;
            return 0;
        }
        m_LastErrorCode = lastErr;
        MyWin::GetSysLastErrorMessage(&m_LastErrorMessage, lastErr);
        return m_LastErrorCode;
    }
    return 0;
}
void MyConditionVariable::Notify(MyCriticalSection* cs) {
    WakeConditionVariable(&m_ConditionVariable);
}
void MyConditionVariable::NotifyAll(MyCriticalSection* cs) {
    WakeAllConditionVariable(&m_ConditionVariable);
}

#endif // _MY_CONDITION_VARIABLE_CPP_
