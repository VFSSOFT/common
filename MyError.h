#ifndef _MY_ERROR_H_
#define _MY_ERROR_H_

#include "MyStringA.h"

class MyError {
public:
    MyError();
    MyError(int code, const char* msg);
    MyError(int code, const wchar_t* msg);
    MyError(const MyError& e);

    void operator=(const MyError& e) { CopyFrom(e); }

    void Set(int code, const char* msg);
    void Set(int code, const wchar_t* msg);

    void Reset();
    void CopyFrom(MyError* e);
    void CopyFrom(const MyError& e);

    static MyError NoError;

private:
    void SetMessage(const char* msg);
    void SetMessage(const wchar_t* msg);

private:
    int   m_Code;
    char  m_Message[1024];
};


#endif // _MY_ERROR_H_

