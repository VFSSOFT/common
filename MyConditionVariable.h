#ifndef _MY_CONDITION_VARIABLE_H_
#define _MY_CONDITION_VARIABLE_H_

#include "MyCoreDef.h"
#include "MyStringA.h"

#include "MyCriticalSection.h"

/*
 * Condition variables are synchronization primitives that enable threads to wait until a particular condition occurs.
 *
 * Condition variables are user-mode objects that cannot be shared across processes.
 *
 *
 * Condition variables enable threads to atomically release a lock and enter the sleeping state.
 * They can be used with critical sections or slim reader/writer (SRW) locks. Condition variables
 * support operations that "wake one" or "wake all" waiting threads. After a thread is woken,
 * it re-acquires the lock it released when the thread entered the sleeping state.
 *
 */

class MyConditionVariable {
public:
    MyConditionVariable();
    ~MyConditionVariable();

    int  Wait(MyCriticalSection* cs, int timeoutMS, bool* timeout);
    void Notify(MyCriticalSection* cs);
    void NotifyAll(MyCriticalSection* cs);

private:

    CONDITION_VARIABLE m_ConditionVariable;
    MY_LAST_ERROR_DECL;
};


#endif // _MY_CONDITION_VARIABLE_H_