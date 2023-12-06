#ifndef _MY_TEST_UTILS_H_
#define _MY_TEST_UTILS_H_

#include "../../common/MyBuffer.h"

class TestUtils {
public:
    static void GenText(MyBuffer* buf, int size) {
        const char* chars = "1234567890-=qwertyuiop[]asdfghjkl;'zxcvbnm,\"./?><:{}~!@#$%^&*()_+\n";
        int charsCount = strlen(chars);

        while (size > 0) {
            int lenToCopy = size > charsCount ? charsCount : size;
            int offset = charsCount - lenToCopy;
            buf->Append(chars + offset, lenToCopy);
            size -= charsCount;
        }
    }
};


#endif // _MY_TEST_UTILS_H_
