#ifndef _MY_ASN_CPP_
#define _MY_ASN_CPP_

#include "MyASN1.h"

int MyAsn1Node::Encode(MyDataPacket* p) {
    int err = 0;
    if (err = p->WriteInt8(m_ID)) return LastError(err, p->LastErrorMessage());

    if (IsSequence() || IsSet() || IsConstructed()) {
        MyDataPacket childPkt;
        for (int i = 0; i < m_Children.Size(); i++) {
            childPkt.Reset();
            MyAsn1Node* child = m_Children.Get(i);
            if (err = child->Encode(&childPkt)) return LastError(err, child->LastErrorMessage());
            m_Content.Append(childPkt.Deref(), childPkt.DerefLength());
        }
    } 
    if (err = EncodeLength(m_Content.Length(), p)) return err;
    if (err = p->Write(m_Content.Deref(), m_Content.Length())) return LastError(err, p->LastErrorMessage());

    return 0;
}
int MyAsn1Node::InitBool(bool val) {
    SetID(MY_ASN1_TAG_BOOL);
    m_Content.Reset();
    m_Content.AppendChar(val ? 0xFF : 0);
    return 0;
}
int MyAsn1Node::InitInteger(INT64 v) {
    SetID(MY_ASN1_TAG_INTEGER);

    char bytes[9];
    if (v < 0) {
        for (int i = 0; i < 8; i++) {
            bytes[i] = ((v >> ((8-i-1) * 8))) & 0xFF;
        }
        int start = 0;
        for (int i = 0; i < 7; i++) {
            if (bytes[i] == 0xFF && (bytes[i + 1] & 0x80) == 0x80) {
                start++;
            } else {
                break;
            }
        }
        m_Content.Set(bytes + start, 8 - start);
    } else {
        if (v < 0x80) {
            m_Content.Reset();
            m_Content.AppendChar(v);
        } else {
            int pos = 8;
            while (v > 0) {
                bytes[pos] = v & 0xFF;
                v >>= 8;
                pos--;
            }
            // If the first byte >= 0x80, add a leading zero byte '0x00'
            if ((bytes[pos + 1] & 0x80) != 0) {
                bytes[pos] = 0;
                pos--;
            }
            m_Content.Set(bytes + pos + 1, 9 - (pos + 1));
        }
    }

    return 0;
}
int MyAsn1Node::InitInteger(const char* raw, int len) {
    SetID(MY_ASN1_TAG_INTEGER);
    m_Content.Set(raw, len);
    return 0;
}
int MyAsn1Node::InitBitstring(const char* bits, int bitsLen) {
    SetID(MY_ASN1_TAG_BIT_STRING);
    if (bitsLen % 8 == 0) {
        SetUnusedBits(0);
    } else {
        SetUnusedBits(8 - bitsLen % 8);
    }
    m_Bits.Set(bits, (bitsLen + 7) / 8);
    m_Content.Reset();
    m_Content.AppendChar(UnusedBits());
    m_Content.Append(bits, (bitsLen + 7) / 8);
    return 0;
}
int MyAsn1Node::InitOctetString(const char* octet, int len) {
    SetID(MY_ASN1_TAG_OCTET_STRING);
    m_Content.Set(octet, len);
    return 0;
}
int MyAsn1Node::InitNull() {
    SetID(MY_ASN1_TAG_NULL);
    return 0;
}
int MyAsn1Node::InitOID(const char* oid, int len) {
    if (len < 0) len = strlen(oid);

    SetID(MY_ASN1_TAG_OID);
    m_OID.Set(oid, len);

    MyAStrArray comps;
    MyValArray<int> intComps;
    comps.Split(m_OID.Deref(), ".");

    if (comps.Size() < 2) 
        return LastError(MY_ERR_BASE_ENCODING, "Invalid ASN.1 OID");

    for (int i = 0; i < comps.Size(); i++) {
        int val = MyStringA::ConvertToInt(comps.Get(i)->Deref(), comps.Get(i)->Length());
        intComps.Add(val);
    }
    
    if (intComps.Get(0) != 0 && intComps.Get(0) != 1 && intComps.Get(0) != 2)
        return LastError(MY_ERR_BASE_ENCODING, "Invalid ASN.1 OID");

    if (intComps.Get(1) <= 0 && intComps.Get(1) > 39)
        return LastError(MY_ERR_BASE_ENCODING, "Invalid ASN.1 OID");
    
    m_Content.Reset();
    m_Content.AppendChar(intComps.Get(0) * 40 + intComps.Get(1));

    for (int i = 2; i < intComps.Size(); i++) {
        int val = intComps.Get(i);
        while (val > 128) {
            int unit = 128;
            while (unit * 128 < val) { unit *= 128; }
            int b = val / unit;
            m_Content.AppendChar(b | 0x80);
            val -= b * unit;
        }
        m_Content.AppendChar(val);
    }

    return 0;
}
int MyAsn1Node::InitUTF8String(const char* str, int len) {
    SetID(MY_ASN1_TAG_UTF8_STRING);
    m_Content.Set(str, len < 0 ? strlen(str) : len);
    return 0;
}
int MyAsn1Node::InitSequence() {
    SetID(MY_ASN1_TAG_SEQUENCE | MY_ASN1_TAG_CLASS_CONSTRUCTED);
    return 0;
}
int MyAsn1Node::InitSet() {
    SetID(MY_ASN1_TAG_SET | MY_ASN1_TAG_CLASS_CONSTRUCTED);
    return 0;
}
int MyAsn1Node::InitPrintableString(const char* str, int len) {
    SetID(MY_ASN1_TAG_PRINTABLE_STRING);
    m_Content.Set(str, len < 0 ? strlen(str) : len);
    return 0;
}
int MyAsn1Node::InitIA5String(const char* str, int len) {
    SetID(MY_ASN1_TAG_IA5_STRING);
    m_Content.Set(str, len < 0 ? strlen(str) : len);
    return 0;
}
int MyAsn1Node::InitUTCTime(MyAsn1Time* time) {

    if (time->Year > 2050) 
        return LastError(MY_ERR_BASE_ENCODING, "ASN.1 UTC Time Year is larger than 2050");

    MyStringA timestr;
    SetID(MY_ASN1_TAG_UTC_TIME);
    m_Time.CopyFrom(time);

    timestr.AppendInt(time->Year % 100, 2);
    timestr.AppendInt(time->Month, 2);
    timestr.AppendInt(time->Day, 2);
    timestr.AppendInt(time->Hour, 2);
    timestr.AppendInt(time->Minute, 2);
    timestr.AppendInt(time->Second, 2);

    if (time->HourOffset == 0 && time->MinuteOffset == 0) {
        timestr.AppendChar('Z');
    } else {
        if (time->HourOffset < 0 || time->MinuteOffset < 0) {
            timestr.AppendChar('-');
            timestr.AppendInt(-1 * time->HourOffset, 2);
            timestr.AppendInt(-1 * time->MinuteOffset, 2);
        } else {
            timestr.AppendChar('+');
            timestr.AppendInt(time->HourOffset, 2);
            timestr.AppendInt(time->MinuteOffset, 2);
        }
    }
    m_Content.Set(timestr.Deref(), timestr.Length());
    return 0;
}
int MyAsn1Node::InitGeneralizedTime(MyAsn1Time* time) {

    MyStringA timestr;
    SetID(MY_ASN1_TAG_GENERALIZED_TIME);
    m_Time.CopyFrom(time);

    timestr.AppendInt(time->Year, 4);
    timestr.AppendInt(time->Month, 2);
    timestr.AppendInt(time->Day, 2);
    timestr.AppendInt(time->Hour, 2);
    timestr.AppendInt(time->Minute, 2);
    timestr.AppendInt(time->Second, 2);

    if (time->Millisecond > 0) {
        timestr.AppendChar('.');
        timestr.AppendInt(time->Millisecond);
    }

    if (time->HourOffset == 0 && time->MinuteOffset == 0) {
        timestr.AppendChar('Z');
    } else {
        if (time->HourOffset < 0 || time->MinuteOffset < 0) {
            timestr.AppendChar('-');
            timestr.AppendInt(-1 * time->HourOffset, 2);
            timestr.AppendInt(-1 * time->MinuteOffset, 2);
        } else {
            timestr.AppendChar('+');
            timestr.AppendInt(time->HourOffset, 2);
            timestr.AppendInt(time->MinuteOffset, 2);
        }
    }
    m_Content.Set(timestr.Deref(), timestr.Length());
    return 0;
}
int MyAsn1Node::InitGeneralString(const char* str, int len) {
    SetID(MY_ASN1_TAG_GENERAL_STRING);
    m_Content.Set(str, len < 0 ? strlen(str) : len);
    return 0;
}
int MyAsn1Node::InitBMPStrign(wchar_t* str, int len) {
    SetID(MY_ASN1_TAG_BMP_STRING);
    m_BMPString.Set(str, len < 0 ? wcslen(str) : len);

    m_Content.Reset();
    for (int i = 0; i < m_BMPString.Length(); i++) {
        wchar_t val = m_BMPString.Deref()[i];
        m_Content.AppendChar((val & 0xFF00) >> 8);
        m_Content.AppendChar(val & 0xFF);
    }

    return 0;
}
int MyAsn1Node::InitExplicit(BYTE tag) {
    SetID(tag);
    return 0;
}
int MyAsn1Node::InitNormalExplicit(BYTE tag) {
    SetTagClass(MY_ASN1_TAG_CLASS_CONTEXT_SPECIFIC | MY_ASN1_TAG_CLASS_CONSTRUCTED);
    SetTagNum(tag);
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
int MyAsn1::Encode(MyBuffer* buf) {
    int err = 0;
    MyDataPacket p;
    if (err = m_Root.Encode(&p)) return LastError(err, m_Root.LastErrorMessage());
    return buf->Set(p.Deref(), p.DerefLength());
}

int MyAsn1Node::Decode(MyDataPacket* p) {
    int err = 0;
    int b = 0;

    if (err = p->PeekByte(&b)) return LastError(err, p->LastErrorMessage());
    
    BYTE tagClass = b & MY_ASN1_TAG_CLASS_MASK;
    bool isConstructed = b & MY_ASN1_TAG_CLASS_CONSTRUCTED;
    BYTE tagNum = b & MY_ASN1_TAG_MASK;

    if (isConstructed && (tagNum != MY_ASN1_TAG_SEQUENCE && tagNum != MY_ASN1_TAG_SET)) {
        if (err = DecodeOtherConstructed(p)) return err;
        return 0;
    }

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
int MyAsn1Node::DecodeGeneralString(MyDataPacket* p) {
    int err = 0;

    if (err = DecodeIDLengthContent(p)) return err;

    if (TagNum() != MY_ASN1_TAG_GENERAL_STRING) {
        return LastError(MY_ERR_BASE_ENCODING, "Invalid ASN.1 GENERAL STRING encoding");
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
int MyAsn1Node::DecodeOtherConstructed(MyDataPacket* p) {
    int err = 0;
    MyDataPacket pkt;

    if (err = DecodeIDLengthContent(p)) return err;

    if (err = pkt.Write(Content()->Deref(), Content()->Length())) return err;

    MyAsn1Node* child = AddChild();
    if (err = child->Decode(&pkt)) return LastError(err, child->LastErrorMessage());

    return err;
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
            length += ((bytes[i] & 0xFF) * (1 << (i * 8)));
        }
    }

    if (err = m_Content.SetLength(length)) return err;
    if (err = p->ReadBytes(length, m_Content.Deref())) return LastError(err, p->LastErrorMessage());

    int rawLen = p->ReadOffset() - startPos;
    if (err = m_Raw.Set(p->Deref() + startPos, rawLen)) return err;

    return 0;
}
int MyAsn1Node::EncodeLength(int len, MyDataPacket* p) {
    int err = 0;
    if (len <= 127) {
        if (err = p->WriteInt8(len)) return LastError(err, p->LastErrorMessage());
    } else {
        char bytes[9];
        int pos = 8;

        while (len > 0) {
            bytes[pos] = (len & 0xFF);
            len = len >> 8;
            pos--;
        }
        bytes[pos] = 0x80 | (8 - pos);
        if (err = p->Write(bytes + pos, 9 - pos)) return LastError(err, p->LastErrorMessage());
    }
    return 0;
}

#endif // _MY_ASN_CPP_

