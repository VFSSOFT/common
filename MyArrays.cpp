#ifndef _MY_ARRAYS_CPP_
#define _MY_ARRAYS_CPP_

#include "MyArrays.h"

MyAStrArray::MyAStrArray() {
    INIT_LAST_ERROR;
}

int MyAStrArray::Size() {
    return m_Arr.Size();
}

int MyAStrArray::Add(const char* str, int len) {
    MyStringA* newStr = m_Arr.AddNew();
    if (newStr == NULL) return LastError(MY_ERR_OUT_OF_MEMORY, "Out of memory");
    int err = newStr->Set(str, len < 0 ? strlen(str) : len);
    return err == 0 ? 0 : LastError(err, "Add fail");
}
int MyAStrArray::Add(MyStringA* str) {
    MyStringA* newStr = m_Arr.AddNew();
    if (newStr == NULL) return LastError(MY_ERR_OUT_OF_MEMORY, "Out of memory");
    int err = newStr->Set(str->Deref(), str->Length());
    return err == 0 ? 0 : LastError(err, "Add fail");
}

MyStringA* MyAStrArray::Get(int index) {
    return m_Arr.Get(index);
}

int MyAStrArray::Index(const char* str, int len) {
    for (int i = 0; i < m_Arr.Size(); i++) {
        MyStringA* tmp = m_Arr.Get(i);
        if (tmp->Equals(str, len, false)) {
            return i;
        }
    }
    return -1;
}
int MyAStrArray::Index(MyStringA* str) {
    return Index(str->Deref(), str->Length());
}
bool MyAStrArray::Contains(const char* str, int len) {
    return Index(str, len) >= 0;
}
bool MyAStrArray::Contains(MyStringA* str) {
    return Contains(str->Deref(), str->Length());
}

#endif // _MY_ARRAYS_CPP_
