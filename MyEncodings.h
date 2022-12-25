#ifndef _MY_ENOCDINGS_H_
#define _MY_ENCODINGS_H_

#include "MyBuffer.h"

class MyEncodings {
public:
    static bool IsAlpha(char c);
    static bool IsDigit(char c);

    static char DigitToHex(char input, bool uppercase=true);
    static char HexToDigit(char input);
    static int UrlEncode(const char* input, int inputLen, MyBuffer* ret, bool uppercase=true);
    static int UrlDecode(const char* input, int inputLen, MyBuffer* ret);
};

#endif // _MY_ENCODIGNS_H_
