#ifndef _MY_ARRAYS_H_
#define _MY_ARRAYS_H_

#include "MyArray.h"
#include "MyStringA.h"

class MyAStrArray {
public:
    MyAStrArray();

    int Add(const char* str, int len=-1);
    int Add(MyStringA* str);

    int Size();
    MyStringA* Get(int index);

    int Index(const char* str, int len=-1);
    int Index(MyStringA* str);
    bool Contains(const char* str, int len=-1);
    bool Contains(MyStringA* str);

    void Split(const char* str, const char* delimiter);

    void Reset() { m_Arr.Reset(); }

private:
    MyArray<MyStringA> m_Arr;

    MY_LAST_ERROR_DECL;
};

#endif // _MY_ARRAYS_H_
