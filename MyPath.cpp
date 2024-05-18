#ifndef _MY_PATH_CPP_
#define _MY_PATH_CPP_


#include "MyPath.h"

#if _WIN32

int MyPath::JoinPath(MyStringW* p1, MyStringW* p2, MyStringW* ret) {
    return JoinPath(p1->Deref(), p1->Length(), p2->Deref(), p2->Length(), ret);
}
int MyPath::JoinPath(const wchar_t* p1, int len1, const wchar_t* p2, int len2, MyStringW* ret) {
    int err = 0;

    assert(len1 > 0 && len2 > 0);
    assert(!IsPathSep(p2[0]));

    if (err = ret->Set(p1, len1)) return err;

    if (!IsPathSep(ret->LastChar())) {
        ret->AppendChar(MY_PATH_SEP_WCHAR);
    }
    
    if (err = ret->Append(p2, len2)) return err;

    return 0;
}
int MyPath::AddPathSep(MyStringW* ret) {
    return ret->AppendChar(MY_PATH_SEP_WCHAR);
}
int MyPath::NormalizPath(MyStringW* str) {
    int err = 0;
    str->Replace(L"//", L"/");
    str->Replace(L"/", L"\\");
    return 0;
}

bool MyPath::IsPathSep(wchar_t c) {
    return c == L'/' || c == L'\\';
}
#endif

int MyPath::AppendPathComp(MyStringA* str, const char* toAppend, int toAppendLen) {
    int err = 0;
    MyStringA result;
    if (err = JoinPath(str->DerefConst(), str->Length(), toAppend, toAppendLen, &result)) return err;
    return str->Set(&result);
}
int MyPath::AppendPathComp(MyStringA* str, MyStringA* toAppend) {
    return AppendPathComp(str, toAppend->DerefConst(), toAppend->Length());
}
int MyPath::JoinPath(MyStringA* p1, MyStringA* p2, MyStringA* ret) {
    return JoinPath(p1->DerefConst(), p1->Length(), p2->DerefConst(), p2->Length(), ret);
}
int MyPath::JoinPath(const char* p1, int len1, const char* p2, int len2, MyStringA* ret) {
    int err = 0;

    assert(len1 > 0 && len2 > 0);
    assert(!IsPathSep(p2[0]));

    if (err = ret->Set(p1, len1)) return err;

    if (!IsPathSep(ret->LastChar())) {
        ret->AppendChar(MY_PATH_SEP_CHAR);
    }
    
    if (err = ret->Append(p2, len2)) return err;

    return 0;
}
int MyPath::JoinPath(const char* p1, const char* p2, char sep, MyStringA* ret) {
    int err = 0;

    if (err = ret->Set(p1)) return err;

    if (ret->LastChar() != sep) {
        ret->AppendChar(sep);
    }
    
    if (err = ret->Append(p2)) return err;

    return 0;
}
int MyPath::AddPathSep(MyStringA* ret) {
    return ret->AppendChar(MY_PATH_SEP_CHAR);
}
int MyPath::NormalizPath(MyStringA* str) { 
    int err = 0;
#if _WIN32
    str->Replace("//", "/");
    str->Replace("/", "\\");
#else
    // Nothing to do 
#endif
    return 0;
}
bool MyPath::IsPathSep(char c) {
#if _WIN32
    return c == '/' || c == '\\';
#else 
    return c == '/';
#endif
}





#endif // _MY_PATH_CPP_
