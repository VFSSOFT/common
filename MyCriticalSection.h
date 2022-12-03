#ifndef _MY_CRITICAL_SECTION_H_
#define _MY_CRITICAL_SECTION_H_

#include "MyCoreDef.h"

/*
 * Critical section objects cannot be shared across processes.
 *
 * A slightly faster, more efficient mechanism for mutual-exclusion synchronization
 *
 * Threads waiting on a critical section do not acquire the critical section on a first-come, first-serve basis. 
 */

class MyCriticalSection {
public:
    MyCriticalSection();
    ~MyCriticalSection();

    void Acquire();
    void Release();

    CRITICAL_SECTION* CS() { return &m_CriticalSection; }

private:
    CRITICAL_SECTION m_CriticalSection;
};


#endif // _MY_CRITICAL_SECTION_H_
