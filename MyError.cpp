#ifndef _MY_ERROR_CPP_
#define _MY_ERROR_CPP_

#include "MyError.h"


MyError MyError::NoError;

MyError::MyError() { Reset(); }
MyError::MyError(int code, const char* msg) {
    Set(code, msg);
}
MyError::MyError(int code, const wchar_t* msg) {
    Set(code, msg);
}
MyError::MyError(const MyError& e) {
    CopyFrom(e);
}

void MyError::Set(int code, const char* msg) {
    m_Code = code;
    SetMessage(msg);
}
void MyError::Set(int code, const wchar_t* msg) {
    m_Code = code;
    SetMessage(msg);
}

void MyError::Reset() {
    m_Code = 0;
    SetMessage("");
}
void MyError::CopyFrom(MyError* e) {
    Set(e->m_Code, e->m_Message);
}
void MyError::CopyFrom(const MyError& e) {
    Set(e.m_Code, e.m_Message);
}

void MyError::SetMessage(const char* msg) {
    strcpy(m_Message, msg);
}
void MyError::SetMessage(const wchar_t* msg) {
    MyStringA tmp;
    tmp.SetUnicode(msg);
    SetMessage(tmp.Deref());
}


#endif // _MY_ERROR_CPP_
