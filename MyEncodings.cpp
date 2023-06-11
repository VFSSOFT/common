#ifndef _MY_ENOCDINGS_CPP_
#define _MY_ENCODINGS_CPP_

#include "MyEncodings.h"

#define _UPPER_CASE_HEX_ARRAY "0123456789ABCDEF"
#define _LOWER_CASE_HEX_ARRAY "0123456789abcdef"

const char* _BASE64_CHARS[2] = {
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz"
    "0123456789"
    "+/",

    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz"
    "0123456789"
    "-_"
};

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

// ref: https://github.com/ReneNyffenegger/cpp-base64/blob/master/base64.cpp
int MyEncodings::B64Encode(const char* data, int len, MyStringA* encoded, bool urlEncode, int lineLength) {
    int err = 0;
    int lenEncoded = (len + 2) / 3 * 4;
    char trailingChar = urlEncode ? '.' : '=';
    const char* base64Chars = _BASE64_CHARS[urlEncode ? 1 : 0];

    encoded->Reset();

    int pos = 0;
    while (pos < len) {
        char c = base64Chars[(data[pos + 0] & 0xFC) >> 2];
        if (err = encoded->AppendChar(c)) return err;

        if (pos + 1 < len) {
            c = base64Chars[((data[pos + 0] & 0x03) << 4) + ((data[pos + 1] & 0xF0) >> 4)];
            if (err = encoded->AppendChar(c)) return err;

            if (pos + 2 < len) {
                c = base64Chars[((data[pos + 1] & 0x0F) << 2) + ((data[pos + 2] & 0xC0) >> 6)];
                if (err = encoded->AppendChar(c)) return err;
                c = base64Chars[(data[pos + 2] & 0x3F)];
                if (err = encoded->AppendChar(c)) return err;
            } else {
                c = base64Chars[(data[pos + 1] & 0x0F) << 2];
                if (err = encoded->AppendChar(c)) return err;
                if (err = encoded->AppendChar(trailingChar)) return err;
            }

        } else {
            c = base64Chars[(data[pos + 0] & 0x03) << 4];
            if (err = encoded->AppendChar(c)) return err;
            if (err = encoded->AppendChar(trailingChar)) return err;
            if (err = encoded->AppendChar(trailingChar)) return err;
        }

        pos += 3;
    }

    assert(encoded->Length() == lenEncoded);

    if (lineLength > 0) {
        if (err = B64InsertLineBreaks(encoded, lineLength)) return err;
    }

    return 0;
}
int MyEncodings::B64Decode(const char* encodedData, MyBuffer* decoded, bool removeLineBreaks) {
    if (STR_IS_EMPTY(encodedData)) return 0;

    int err = 0;
    const char* data = encodedData;
    int len = strlen(encodedData);
    int pos = 0;
    char c = 0;

    if (removeLineBreaks) {
        MyStringA newEncoded;
        for (int i = 0; i < len; i++) {
            if (data[i] == '\r' || data[i] == '\n') continue;
            if (err = newEncoded.AppendChar(data[i])) return err;
        }
        return B64Decode(newEncoded.Deref(), decoded, false);
    }

    while (pos < len) {
        int posOfChar0 = B64PosOfChar(data[pos + 0]);
        if (posOfChar0 == MY_ERR_BASE_ENCODING) return MY_ERR_BASE_ENCODING;

        int posOfChar1 = B64PosOfChar(data[pos + 1]);
        if (posOfChar1 == MY_ERR_BASE_ENCODING) return MY_ERR_BASE_ENCODING;

        c = (posOfChar0 << 2) + ((posOfChar1 & 0x30) >> 4);
        if (err = decoded->AppendChar(c)) return err;

        if ((pos + 2 < len)       && // Check for data that is not padded with equal signs (which is allowed by RFC 2045)
             data[pos + 2] != '=' &&
             data[pos + 2] != '.') { // accept URL-safe base 64 strings, too, so check for '.' also.

            int posOfChar2 = B64PosOfChar(data[pos + 2]);
            if (posOfChar2 == MY_ERR_BASE_ENCODING) return MY_ERR_BASE_ENCODING;

            c = ((posOfChar1 & 0x0F) << 4) + ((posOfChar2 & 0x3C) >> 2);
            if (err = decoded->AppendChar(c)) return err;

            if ((pos + 3 < len) && data[pos + 3] != '=' && data[pos + 3] != '.') {
                int posOfChar3 = B64PosOfChar(data[pos + 3]);
                if (posOfChar3 == MY_ERR_BASE_ENCODING) return MY_ERR_BASE_ENCODING;

                c = ((posOfChar2 & 0x03) << 6) + posOfChar3;
                if (err = decoded->AppendChar(c)) return err;
            }
        }

        pos += 4;
    }
    return 0;
}
int MyEncodings::B64PosOfChar(const char c) {
    // Return the position of chr within base64_encode()

    if (c >= 'A' && c <= 'Z') return c - 'A';
    if (c >= 'a' && c <= 'z') return c - 'a' + ('Z' - 'A') + 1;
    if (c >= '0' && c <= '9') return c - '0' + ('Z' - 'A') + ('z' - 'a') + 2;
    if (c == '+' || c == '-') return 62; // Be liberal with input and accept both url ('-') and non-url ('+') base 64 characters
    if (c == '/' || c == '_') return 63; // // Ditto for '/' and '_'
    return MY_ERR_BASE_ENCODING;
}
int MyEncodings::B64InsertLineBreaks(MyStringA* str, int distance) {
    int err = 0;
    int pos = distance;
    while (pos < str->Length()) {
        if (err = str->Insert("\n", 1, pos)) return err;
        pos += distance + 1;
    }
    return 0;
}


int MyEncodings::HexEncode(const char* data, int len, MyStringA* encoded, bool uppercase) {
    char const hex1[16] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b','c','d','e','f' };
    char const hex2[16] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B','C','D','E','F' };

    encoded->SetLength(len * 2);
    for (int i = 0; i < len; i++) {
        int v = data[i] & 0x0FF;
        char c = uppercase ? hex2[(v & 0xF0) >> 4] : hex1[(v & 0xF0) >> 4];
        encoded->SetCharAt(i * 2, c);
        c = uppercase ? hex2[(v & 0x0F)] : hex1[(v & 0x0F)];
        encoded->SetCharAt((i * 2) + 1, c);
    }
    return 0;
}
int MyEncodings::HexDecode(const char* data, int len, MyBuffer* decoded) {
    if (len % 2 != 0) return -1;

    decoded->SetLength(len / 2);
    for (int i = 0; i < len; i += 2) {
        char v1 = 0;
        char c = data[i];

        if (c >= '0' && c <= '9') {
            v1 = c - '0';
        } else if (c >= 'a' && c <= 'f') {
            v1 = c - 'a' + 10;
        } else if (c >= 'A' && c <= 'F') {
            v1 = c - 'A' + 10;
        } else {
            return -1;
        }

        char v2 = 0;
        c = data[i+1];
        if (c >= '0' && c <= '9') {
            v2 = c - '0';
        } else if (c >= 'a' && c <= 'f') {
            v2 = c - 'a' + 10;
        } else if (c >= 'A' && c <= 'F') {
            v2 = c - 'A' + 10;
        } else {
            return -1;
        }

        decoded->SetCharAt(i / 2, (v1 << 4) | v2);
    }

    return 0;
}

void MyEncodings::IntToBytesBigEndian(UINT64 val, int bytesLen, MyBuffer* buf) {
    buf->SetLength(bytesLen);
    IntToBytesBigEndian(val, bytesLen, buf->Deref());
}
void MyEncodings::IntToBytesBigEndian(UINT64 val, int bytesLen, char* buf) {
    char bytes[8];

    bytes[0] = (val & 0xFF00000000000000) >> 56;
    bytes[1] = (val & 0x00FF000000000000) >> 48;
    bytes[2] = (val & 0x0000FF0000000000) >> 40;
    bytes[3] = (val & 0x0000FF0000000000) >> 32;
    bytes[4] = (val & 0x0000FF0000000000) >> 24;
    bytes[5] = (val & 0x0000FF0000000000) >> 16;
    bytes[6] = (val & 0x0000FF0000000000) >> 8 ;
    bytes[7] = (val & 0x0000FF0000000000)      ;

    memcpy(buf, bytes + (8 - bytesLen), bytesLen);
}
UINT64 MyEncodings::BytesToIntBigEndian(const char* b, int bLen) {
    UINT64 ret = 0;
    for (int i = 0; i < bLen; i++) {
        ret += (b[i] & 0xFF) * (1 << (i * 8));
    }
    return ret;
}

#endif // _MY_ENCODIGNS_CPP_

