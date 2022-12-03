#ifndef _MY_CRITICAL_SECTION_CPP_
#define _MY_CRITICAL_SECTION_CPP_

#include "MyCriticalSection.h"

MyCriticalSection::MyCriticalSection() {
    InitializeCriticalSectionAndSpinCount(&m_CriticalSection, 0x00000400);
}
MyCriticalSection::~MyCriticalSection() {
    DeleteCriticalSection(&m_CriticalSection);
}

void MyCriticalSection::Acquire() {
    EnterCriticalSection(&m_CriticalSection);
}
void MyCriticalSection::Release() {
    LeaveCriticalSection(&m_CriticalSection);
}

#endif _MY_CRITICAL_SECTION_CPP_