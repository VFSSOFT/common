#ifndef _MY_ENOCDINGS_CPP_
#define _MY_ENCODINGS_CPP_

#include "MyEncodings.h"

#define _UPPER_CASE_HEX_ARRAY "0123456789ABCDEF"
#define _LOWER_CASE_HEX_ARRAY "0123456789abcdef"

// rfc: 3986
bool MyEncodings::IsAlpha(char c) {
    return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z');
}
bool MyEncodings::IsDigit(char c) {
    return '0' <= c && c <= '9';
}

char MyEncodings::DigitToHex(char input, bool uppercase) {
    return uppercase ? _UPPER_CASE_HEX_ARRAY[input & 15] : _LOWER_CASE_HEX_ARRAY[input & 15];
}
char MyEncodings::HexToDigit(char input) {
    if (IsDigit(input))
        return input - '0';

    if (input >= 'a' && input <= 'f')
        return input - 'a' + 10;

    if (input >= 'A' && input <= 'F')
        return input - 'A' + 10;

    assert(false);
    return -1;
}

int MyEncodings::UrlEncode(const char* input, int inputLen, MyBuffer* ret, bool uppercase) {
    int err = 0;
    ret->Reset();

    for (int i = 0; i < inputLen; i++) {
        char c = input[i];

        if (IsAlpha(c) || IsDigit(c) || c == '-' || c == '_' || c == '.' || c == '~') {
            if (err = ret->AppendChar(c)) return err;
            continue;
        }

        if (err = ret->AppendChar('%')) return err;
        if (err = ret->AppendChar(DigitToHex((c & 0xF0) >> 4, uppercase))) return err;
        if (err = ret->AppendChar(DigitToHex(c & 0x0F, uppercase))) return err;
    }

    return 0;
}
int MyEncodings::UrlDecode(const char* input, int inputLen, MyBuffer* ret) {
    int err = 0;
    ret->Reset();

    int index = 0;
    while (index < inputLen) {
        if (input[index] == '%') {
            if (index + 2 >= inputLen) 
                return MY_ERR_INVALID_URL;

            char hb = HexToDigit(input[index+1]);
            char lb = HexToDigit(input[index + 2]);
            ret->AppendChar(hb << 4 | lb);

            index += 3;
        } else {
            ret->AppendChar(input[index]);
            index++;
        }
    }

    return 0;
}

#endif // _MY_ENCODIGNS_CPP_

