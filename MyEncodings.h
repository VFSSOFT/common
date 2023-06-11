#ifndef _MY_ENOCDINGS_H_
#define _MY_ENCODINGS_H_

#include "MyBuffer.h"
#include "MyStringA.h"

class MyEncodings {
public:
    static bool IsAlpha(char c);
    static bool IsDigit(char c);

    static char DigitToHex(char input, bool uppercase=true);
    static char HexToDigit(char input);
    static int UrlEncode(const char* input, int inputLen, MyBuffer* ret, bool uppercase=true);
    static int UrlDecode(const char* input, int inputLen, MyBuffer* ret);

    static int B64Encode(const char* data, int len, MyStringA* encoded, bool urlEncode=false, int lineLength=0);
    static int B64Decode(const char* encodedData, MyBuffer* decoded, bool removeLineBreaks=false);

    static int HexEncode(const char* data, int len, MyStringA* encoded, bool uppercase=false);
    static int HexDecode(const char* data, int len, MyBuffer* decoded);

    static void IntToBytesBigEndian(UINT64 val, int bytesLen, MyBuffer* buf);
    static void IntToBytesBigEndian(UINT64 val, int bytesLen, char* buf);
    static UINT64 BytesToIntBigEndian(const char* b, int bLen);

private:
    static int B64PosOfChar(const char c);
    static int B64InsertLineBreaks(MyStringA* str, int distance);
};

#endif // _MY_ENCODIGNS_H_
