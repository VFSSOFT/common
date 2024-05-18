#ifndef _MY_PATH_H_
#define _MY_PATH_H_

#include "MyCoreDef.h"
#include "MyStringA.h"
#include "MyStringW.h"


class MyPath {
public:

#if _WIN32
    static int JoinPath(MyStringW* p1, MyStringW* p2, MyStringW* ret);
    static int JoinPath(const wchar_t* p1, int len1, const wchar_t* p2, int len2, MyStringW* ret);
    static int AddPathSep(MyStringW* ret);
    static int NormalizPath(MyStringW* str);
    static bool IsPathSep(wchar_t c);
#endif

    static int AppendPathComp(MyStringA* str, const char* toAppend, int toAppendLen);
    static int AppendPathComp(MyStringA* str, MyStringA* toAppend);
    static int JoinPath(MyStringA* p1, MyStringA* p2, MyStringA* ret);
    static int JoinPath(const char* p1, int len1, const char* p2, int len2, MyStringA* ret);
    static int AddPathSep(MyStringA* ret);
    static int NormalizPath(MyStringA* str);
    static bool IsPathSep(char c);

    static int JoinPath(const char* p1, const char* p2, char sep, MyStringA* ret);
};







#endif // _MY_PATH_H_

