#ifndef _MY_PATH_H_
#define _MY_PATH_H_

#include "MyCoreDef.h"
#include "MyStringA.h"
#include "MyStringW.h"


class MyPath {
public:

#if _WIN32

    const wchar_t* Deref() { return m_Path.Deref(); }
    int Length() { return m_Path.Length(); }

    void Set(MyPath* p) { m_Path.Set(p->Deref(), p->Length()); }
    void Set(const wchar_t* p, int len=-1) { m_Path.Set(p, len < 0 ? wcslen(p) : len); }
    void Set(const char* p, int len) { m_Path.SetUtf8(p, len < 0 ? strlen(p) : len); }

    bool Empty() { return m_Path.Length() == 0; }
    bool HasParent() { return m_Path.IndexOf(L"\\") > 0 || m_Path.IndexOf(L"/") > 0; }

    int GetParent(MyPath* retParent);

#else 
#endif

    


public: // Static 
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

private:
#if _WIN32
    MyStringW m_Path;
#else 
    MyStringA m_Path;
#endif
};







#endif // _MY_PATH_H_

