#ifndef _MY_ARRAYS_H_
#define _MY_ARRAYS_H_

#include "MyArray.h"
#include "MyStringA.h"

class MyAStrArray {
public:
    MyAStrArray();

    int Size();

    int Add(const char* str, int len=-1);
    int Add(MyStringA* str);

    MyStringA* Get(int index);

private:
    MyArray<MyStringA> m_Arr;

    MY_LAST_ERROR_DECL;
};

#endif // _MY_ARRAYS_H_
