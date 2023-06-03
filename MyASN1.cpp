#ifndef _MY_ASN_CPP_
#define _MY_ASN_CPP_

#include "MyASN1.h"

int MyAsn1Node::InitBool(bool val) {
    return 0;
}
int MyAsn1Node::InitInteger(INT64 v) {
    return 0;
}
int MyAsn1Node::InitInteger(UINT64 v) {
    return 0;
}
int MyAsn1Node::InitInteger(const char* raw, int len) {
    return 0;
}
int MyAsn1Node::InitBitstring(const char* bits, int bitsLen) {
    return 0;
}
int MyAsn1Node::InitOctetString(const char* octet, int len) {
    return 0;
}
int MyAsn1Node::InitNull() {
    return 0;
}
int MyAsn1Node::InitOID(const char* oid, int len) {
    return 0;
}
int MyAsn1Node::InitUTF8String(const char* str, int len) {
    return 0;
}
int MyAsn1Node::InitSequence() {
    return 0;
}
int MyAsn1Node::InitSet() {
    return 0;
}
int MyAsn1Node::InitPrintableString(const char* str, int len) {
    return 0;
}
int MyAsn1Node::InitIA5String(const char* str, int len) {
    return 0;
}
int MyAsn1Node::InitUTCTime() {
    return 0;
}
int MyAsn1Node::InitGeneralizedTime() {
    return 0;
}
int MyAsn1Node::InitBMPStrign(wchar_t* str, int len) {
    return 0;
}
int MyAsn1Node::InitRaw(const char* raw, int len) {
    return 0;
}

int MyAsn1::Decode(const char* data, int len) {
    int err = 0;
    MyDataPacket p;

    if (err = p.Write(data, len)) return LastError(err, p.LastErrorMessage());

    return Decode(&p);
}

int MyAsn1::Decode(MyDataPacket* p) {
    int err = 0;
    if (err = m_Root.Decode(p)) return LastError(err, m_Root.LastErrorMessage());
    return 0;
}
int MyAsn1Node::Decode(MyDataPacket* p) {
    int err = 0;
    int b = 0;

    if (err = p->PeekByte(&b)) return LastError(err, p->LastErrorMessage());
    
    BYTE tagNum = b & MY_ASN1_TAG_MASK;
    switch (tagNum) {
    
    case MY_ASN1_TAG_BOOL:
        if (err = DecodeBool(p)) return err;
        break;
    
    case MY_ASN1_TAG_INTEGER:
        if (err = DecodeInteger(p)) return err;
        break;

    case MY_ASN1_TAG_BIT_STRING:
        if (err = DecodeBitString(p)) return err;
        break;

    case MY_ASN1_TAG_OCTET_STRING:
        if (err = DecodeOctetString(p)) return err;
        break;

    case MY_ASN1_TAG_NULL:
        if (err = DecodeNull(p)) return err;
        break;

    case MY_ASN1_TAG_OID:
        if (err = DecodeOID(p)) return err;
        break;

    case MY_ASN1_TAG_UTF8_STRING:
        if (err = DecodeUTF8String(p)) return err;
        break;

    case MY_ASN1_TAG_SEQUENCE:
        if (err = DecodeSequence(p)) return err;
        break;

    case MY_ASN1_TAG_SET:
        if (err = DecodeSet(p)) return err;
        break;

    case MY_ASN1_TAG_PRINTABLE_STRING:
        if (err = DecodePrintableString(p)) return err;
        break;

    case MY_ASN1_TAG_IA5_STRING:
        if (err = DecodeIA5String(p)) return err;
        break;

    case MY_ASN1_TAG_BMP_STRING:
        if (err = DecodeBMPString(p)) return err;
        break;

    case MY_ASN1_TAG_UTC_TIME:
        if (err = DecodeUTCTime(p)) return err;
        break;

    case MY_ASN1_TAG_GENERALIZED_TIME:
        if (err = DecodeGeneralizedTime(p)) return err;
        break;

    default:
        // unkonwn tag: may be explict/implicit tag
        if (err = DecodeRaw(p)) return err;
        break;
    }

    return 0;
}
int MyAsn1Node::DecodeBool(MyDataPacket* p) {
    int err = 0;

    if (err = DecodeIDLengthContent(p)) return err;

    if (m_Raw.CharAt(0) != 1 || m_Raw.CharAt(1) != 1) {
        return LastError(MY_ERR_BASE_ENCODING, "Invalid ASN.1 BOOL encoding");
    }

    return err;
}
int MyAsn1Node::DecodeInteger(MyDataPacket* p) {
    int err = 0;

    if (err = DecodeIDLengthContent(p)) return err;

    if (TagNum() != MY_ASN1_TAG_INTEGER) {
        return LastError(MY_ERR_BASE_ENCODING, "Invalid ASN.1 INTEGER encoding");
    }
    return err;
}
int MyAsn1Node::DecodeBitString(MyDataPacket* p) {
    int err = 0;
    BYTE b;
    
    if (err = DecodeIDLengthContent(p)) return err;

    if (TagNum() != MY_ASN1_TAG_BIT_STRING) {
        return LastError(MY_ERR_BASE_ENCODING, "Invalid ASN.1 BIT STRING encoding");
    }

    b = m_Content.CharAt(0);
    if (b > 7) {
        return LastError(MY_ERR_BASE_ENCODING, "Invalid ASN.1 BIT STRING encoding: Invalid Unused Bit flag");
    }
    SetUnusedBits(b);
    BitString()->Set(m_Content.Deref(1), m_Content.Length() - 1);

    return err;
}
int MyAsn1Node::DecodeOctetString(MyDataPacket* p) {
    int err = 0;

    if (err = DecodeIDLengthContent(p)) return err;

    if (TagNum() != MY_ASN1_TAG_OCTET_STRING) {
        return LastError(MY_ERR_BASE_ENCODING, "Invalid ASN.1 OCTET STRING encoding");
    }
    return err;
}
int MyAsn1Node::DecodeNull(MyDataPacket* p) {
    int err = 0;

    if (err = DecodeIDLengthContent(p)) return err;

    if (TagNum() != MY_ASN1_TAG_NULL) {
        return LastError(MY_ERR_BASE_ENCODING, "Invalid ASN.1 NULL encoding");
    }

    return err;
}
int MyAsn1Node::DecodeOID(MyDataPacket* p) {
    int err = 0;
    int i = 0;
    int curComp;

    if (err = DecodeIDLengthContent(p)) return err;

    if (TagNum() != MY_ASN1_TAG_OID) {
        return LastError(MY_ERR_BASE_ENCODING, "Invalid ASN.1 OBJECT IDENTIFIER encoding");
    }

    if (m_Content.Length()) {

        while (i < m_Content.Length()) {
            curComp = 0;
            while (i < m_Content.Length()) {
                curComp <<= 7;
                curComp += m_Content.CharAt(i) & 0x7F;
                
                if ((m_Content.CharAt(i) & 0x80) == 0) {
                    i++;
                    break;
                }
                i++;
            }
            
            if (OID()->Length() == 0) {
                OID()->AppendInt(curComp / 40);
                OID()->AppendChar('.');
                OID()->AppendInt(curComp % 40);
            } else {
                OID()->AppendChar('.');
                OID()->AppendInt(curComp);
            }

        }
    }
    return err;
}

int MyAsn1Node::DecodeUTF8String(MyDataPacket* p) {
    int err = 0;

    if (err = DecodeIDLengthContent(p)) return err;

    if (TagNum() != MY_ASN1_TAG_UTF8_STRING) {
        return LastError(MY_ERR_BASE_ENCODING, "Invalid ASN.1 UTF8 STRING encoding");
    }
    return err;
}

int MyAsn1Node::DecodePrintableString(MyDataPacket* p) {
    int err = 0;

    if (err = DecodeIDLengthContent(p)) return err;

    if (TagNum() != MY_ASN1_TAG_PRINTABLE_STRING) {
        return LastError(MY_ERR_BASE_ENCODING, "Invalid ASN.1 PRINTABLE STRING encoding");
    }
    return err;
}
int MyAsn1Node::DecodeSequence(MyDataPacket* p) {
    int err = 0;
    MyDataPacket pkt;
    MyAsn1Node* child = NULL;

    if (err = DecodeIDLengthContent(p)) return err;

    if (TagNum() != MY_ASN1_TAG_SEQUENCE) {
        return LastError(MY_ERR_BASE_ENCODING, "Invalid ASN.1 SEQUENCE encoding");
    }

    if (err = pkt.Write(m_Content.Deref(), m_Content.Length()))  return err;
    while (pkt.Available() > 0) {
        MyAsn1Node* child = AddChild();
        if (err = child->Decode(&pkt)) return LastError(err, child->LastErrorMessage());
    }

    return err;
}
int MyAsn1Node::DecodeSet(MyDataPacket* p) {
    int err = 0;
    MyDataPacket pkt;
    MyAsn1Node* child = NULL;

    if (err = DecodeIDLengthContent(p)) return err;

    if (TagNum() != MY_ASN1_TAG_SET) {
        return LastError(MY_ERR_BASE_ENCODING, "Invalid ASN.1 SET encoding");
    }

    if (err = pkt.Write(Content()->Deref(), Content()->Length())) return err;
    while (pkt.Available() > 0) {
        MyAsn1Node* child = AddChild();
        if (err = child->Decode(&pkt)) return LastError(err, child->LastErrorMessage());
    }

    return err;
}
int MyAsn1Node::DecodeIA5String(MyDataPacket* p) {
    int err = 0;

    if (err = DecodeIDLengthContent(p)) return err;

    if (TagNum() != MY_ASN1_TAG_IA5_STRING) {
        return LastError(MY_ERR_BASE_ENCODING, "Invalid ASN.1 IA5 STRING encoding");
    }
    return err;
}
int MyAsn1Node::DecodeBMPString(MyDataPacket* p) {
    int err = 0;
    int i = 0;

    if (err = DecodeIDLengthContent(p)) return err;

    if (TagNum() != MY_ASN1_TAG_BMP_STRING) {
        return LastError(MY_ERR_BASE_ENCODING, "Invalid ASN.1 BMP STRING encoding");
    }
    for (int i = 0; i < Content()->Length(); i += 2) {
        WString()->AppendChar(Content()->CharAt(i+1) | (Content()->CharAt(i) << 8));
    }

    return err;
}
int MyAsn1Node::DecodeUTCTime(MyDataPacket* p) {
    /*
     * YYMMDDhhmmZ
     * YYMMDDhhmm+hh'mm'
     * YYMMDDhhmm-hh'mm'
     * YYMMDDhhmmssZ
     * YYMMDDhhmmss+hh'mm'
     * YYMMDDhhmmss-hh'mm'
    */

    int err = 0;
    int i = 0;
    char c = 0;

    if (err = DecodeIDLengthContent(p)) return err;

    if (TagNum() != MY_ASN1_TAG_UTC_TIME) {
        return LastError(MY_ERR_BASE_ENCODING, "Invalid ASN.1 UTC Time encoding");
    }
    if (Content()->Length() < 11) {
        return LastError(MY_ERR_BASE_ENCODING, "Invalid ASN.1 UTC Time encoding: too short");
    }

    Time()->Year = MyStringA::ConvertToInt(Content()->Deref(), 2) + 2000;
    if (Time()->Year > 2050) {
        // UTCTime only encodes times prior to 2050. See https://tools.ietf.org/html/rfc5280#section-4.1.2.5.1
        Time()->Year -= 100;
    }
    Time()->Month  = MyStringA::ConvertToInt(Content()->Deref(2), 2);
    Time()->Day    = MyStringA::ConvertToInt(Content()->Deref(4), 2);
    Time()->Hour   = MyStringA::ConvertToInt(Content()->Deref(6), 2);
    Time()->Minute = MyStringA::ConvertToInt(Content()->Deref(8), 2);

    i = 10;
    c = Content()->CharAt(i);
    if (c >= '0' && c <= '9') {
        Time()->Second = MyStringA::ConvertToInt(Content()->Deref(i), 2);
        i += 2;
    }

    c = Content()->CharAt(i);
    if (c == '+' || c == '-') {
        Time()->HourOffset = MyStringA::ConvertToInt(Content()->Deref(i + 1), 2);
        Time()->MinuteOffset = MyStringA::ConvertToInt(Content()->Deref(i + 3), 2);
        if (c == '-') {
            Time()->HourOffset *= -1;
            Time()->MinuteOffset *= -1;
        }
    } else {
        if (c != 'z' && c != 'Z') {
            return LastError(MY_ERR_BASE_ENCODING, "Invalid ASN.1 UTC Time encoding: should end with 'Z'");
        }
    }

    return err;
}
int MyAsn1Node::DecodeGeneralizedTime(MyDataPacket* p) {
    /*
     *  ref: https://obj-sys.com/asn1tutorial/node14.html
     * 1.Local time only. ``YYYYMMDDHH[MM[SS[.fff]]]'', where the optional fff is accurate to three decimal places.
     * 2. Universal time (UTC time) only. ``YYYYMMDDHH[MM[SS[.fff]]]Z''.
     * 3. Difference between local and UTC times. ``YYYYMMDDHH[MM[SS[.fff]]]+-HHMM''.
     */

    int err = 0;
    int i = 0;
    char c = 0;
    int len = 0;

    if (err = DecodeIDLengthContent(p)) return err;

    if (TagNum() != MY_ASN1_TAG_GENERALIZED_TIME) {
        return LastError(MY_ERR_BASE_ENCODING, "Invalid ASN.1 GENERALIZED Time encoding");
    }
    if (Content()->Length() < 10) {
        return LastError(MY_ERR_BASE_ENCODING, "Invalid ASN.1 UTC GENERALIZED encoding: too short");
    }

    Time()->Year   = MyStringA::ConvertToInt(Content()->Deref(), 4);
    Time()->Month  = MyStringA::ConvertToInt(Content()->Deref(4), 2);
    Time()->Day    = MyStringA::ConvertToInt(Content()->Deref(6), 2);
    Time()->Hour   = MyStringA::ConvertToInt(Content()->Deref(8), 2);

    i = 10;
    // Possible: [MM[SS[.fff]]]
    if (Content()->Length() >= i + 2 && (MyStringA::IsDigit(Content()->CharAt(i)))) {
        Time()->Minute = MyStringA::ConvertToInt(Content()->Deref(i), 2);
        i += 2;

        if (Content()->Length() >= i + 2 && (MyStringA::IsDigit(Content()->CharAt(i)))) {
            Time()->Second = MyStringA::ConvertToInt(Content()->Deref(i), 2);
            i += 2;
        }
        if (Content()->Length() >= i + 2 && Content()->CharAt(i) == '.') {
            while ((i + 1 + i) < Content()->Length() && Content()->CharAt(i + 1 + i) != '+' && Content()->CharAt(i + 1 + i) != '-' && Content()->CharAt(i + 1 + i) != 'Z') {
                len++;
            }
            Time()->Millisecond = MyStringA::ConvertToInt(Content()->Deref(i), len);
            i += len;
        }
    }

    if (i >= Content()->Length()) {
        return LastError(MY_ERR_BASE_ENCODING, "Invalid ASN.1 UTC GENERALIZED encoding: too short");
    }

    c = Content()->CharAt(i);
    if (c == '+' || c == '-') {
        Time()->HourOffset = MyStringA::ConvertToInt(Content()->Deref(i + 1), 2);
        Time()->MinuteOffset = MyStringA::ConvertToInt(Content()->Deref(i + 3), 2);
        if (c == '-') {
            Time()->HourOffset *= -1;
            Time()->MinuteOffset *= -1;
        }
    } else {
        if (c != 'z' && c != 'Z') {
            return LastError(MY_ERR_BASE_ENCODING, "Invalid ASN.1 GENERALIZED Time encoding: should end with 'Z'");
        }
    }

    return err;
}

int MyAsn1Node::DecodeRaw(MyDataPacket* p) {
    return DecodeIDLengthContent(p);
}

int MyAsn1Node::DecodeIDLengthContent(MyDataPacket* p) {
    int err = 0;
    int b = 0;
    UINT64 length = 0;
    int startPos = p->ReadOffset();

    if (err = p->ReadInt8(&b)) return LastError(err, p->LastErrorMessage());
    SetID(b);

    if (err = p->ReadInt8(&b)) return LastError(err, p->LastErrorMessage());

    if ((b & 0x80) == 0) { // 0 <= l <= 127
        length = b & 0x7F;
    } else {
        int bytesCnt = b & 0x7F;
        if (bytesCnt > 8) return LastError(MY_ERR_BASE_ENCODING, "Invalid ASN.1 Length encoding: Length is too large");
        if (p->Available() < bytesCnt) return LastError(MY_ERR_BASE_ENCODING, "Invalid ASN.1 Length encoding");

        char bytes[8];
        if (err = p->ReadBytes(bytesCnt, bytes)) return LastError(err, p->LastErrorMessage());
        for (int i = 0; i < bytesCnt; i++) {
            length += (bytes[i] * (1 << (bytesCnt * 8)));
        }
    }

    if (err = m_Content.SetLength(length)) return err;
    if (err = p->ReadBytes(length, m_Content.Deref())) return LastError(err, p->LastErrorMessage());

    int rawLen = p->ReadOffset() - startPos;
    if (err = m_Raw.Set(p->Deref() + startPos, rawLen)) return err;

    return 0;
}

#endif // _MY_ASN_CPP_

