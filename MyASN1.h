#ifndef _MY_ASN_H_
#define _MY_ASN_H_

#include "MyBuffer.h"
#include "MyStringA.h"
#include "MyArray.h"
#include "MyValArray.h"
#include "MyArrays.h"
#include "MyDataPacket.h"


// ASN.1 Tags
#define MY_ASN1_TAG_MASK                    0x1F
#define MY_ASN1_TAG_BOOL                    0x01
#define MY_ASN1_TAG_INTEGER                 0x02
#define MY_ASN1_TAG_BIT_STRING              0x03
#define MY_ASN1_TAG_OCTET_STRING            0x04
#define MY_ASN1_TAG_NULL                    0x05
#define MY_ASN1_TAG_OID                     0x06
#define MY_ASN1_TAG_ENUMERATED              0x0a
#define MY_ASN1_TAG_UTF8_STRING             0x0c
#define MY_ASN1_TAG_SEQUENCE                0x10
#define MY_ASN1_TAG_SET                     0x11
#define MY_ASN1_TAG_PRINTABLE_STRING        0x13
#define MY_ASN1_TAG_T16_STRING              0x14
#define MY_ASN1_TAG_IA5_STRING              0x16
#define MY_ASN1_TAG_UTC_TIME                0x17
#define MY_ASN1_TAG_GENERALIZED_TIME        0x18
#define MY_ASN1_TAG_BMP_STRING              0x1e

// ASN.1 tag class
#define MY_ASN1_TAG_CLASS_MASK              0xc0
#define MY_ASN1_TAG_CLASS_UNIVERSAL         0x00
#define MY_ASN1_TAG_CLASS_CONSTRUCTED       0x20
#define MY_ASN1_TAG_CLASS_APPLICATION       0x40
#define MY_ASN1_TAG_CLASS_CONTEXT_SPECIFIC  0x80
#define MY_ASN1_TAG_CLASS_PRIVATE           0xc0

class MyAsn1Time {
public:
    MyAsn1Time(): Year(0), Month(0), Day(0), Hour(0), Minute(0),
        Second(0), Millisecond(0), HourOffset(0), MinuteOffset(0) {}

    void CopyFrom(MyAsn1Time* t) {
        Year = t->Year;
        Month = t->Month;
        Day = t->Day;
        Hour = t->Hour;
        Minute = t->Minute;
        Second = t->Second;
        HourOffset = t->HourOffset;
        MinuteOffset = t->MinuteOffset;
    }

    int Year;
    int Month;
    int Day;
    int Hour;
    int Minute;
    int Second;
    int Millisecond;
    int HourOffset;
    int MinuteOffset;
};

class MyAsn1Node {
public:
    MyAsn1Node(): m_ID(0), m_UseInfiniteLength(false), m_Parent(NULL), m_UnusedBits(0), m_LastErrorCode(0) {}

    BYTE ID() { return m_ID; }
    void SetID(BYTE v) { m_ID = v; }

    bool IsConstructed() { 
        return IS_FLAG_ON(m_ID, MY_ASN1_TAG_CLASS_CONSTRUCTED);
    }
    void SetIsConstructed(bool val) { 
        SET_FLAG(m_ID, MY_ASN1_TAG_CLASS_CONSTRUCTED, val);
    }

    BYTE TagNum() { 
        return (m_ID & MY_ASN1_TAG_MASK);
    }
    void SetTagNum(BYTE v) {
        m_ID &= ~MY_ASN1_TAG_MASK;
        m_ID |= v;
    }

    BYTE TagClass() { 
        return (m_ID & MY_ASN1_TAG_CLASS_MASK);
    }
    void SetTagClass(BYTE v) { 
        m_ID &= ~MY_ASN1_TAG_CLASS_MASK;
        m_ID |= v;
    }

    bool IsBool() { return TagNum() == MY_ASN1_TAG_BOOL; }
    bool IsInteger() { return TagNum() == MY_ASN1_TAG_INTEGER; }
    bool IsBitstring() { return TagNum() == MY_ASN1_TAG_BIT_STRING; }
    bool IsOctetString() { return TagNum() == MY_ASN1_TAG_OCTET_STRING; }
    bool IsNull() { return TagNum() == MY_ASN1_TAG_NULL; }
    bool IsOID() { return TagNum() == MY_ASN1_TAG_OID; }
    bool IsUTF8String() { return TagNum() == MY_ASN1_TAG_UTF8_STRING; }
    bool IsSequence() { return TagNum() == MY_ASN1_TAG_SEQUENCE; }
    bool IsSet() { return TagNum() == MY_ASN1_TAG_SET; }
    bool IsPrintableString() { return TagNum() == MY_ASN1_TAG_PRINTABLE_STRING; }
    bool IsIA5String() { return TagNum() == MY_ASN1_TAG_IA5_STRING; }
    bool IsUTCTime() { return TagNum() == MY_ASN1_TAG_UTC_TIME; }
    bool IsGeneralizedTime() { return TagNum() == MY_ASN1_TAG_GENERALIZED_TIME; }
    bool IsBMPString() { return TagNum() == MY_ASN1_TAG_BMP_STRING; }

    int Decode(MyDataPacket* p);
    int DecodeBool(MyDataPacket* p);
    int DecodeInteger(MyDataPacket* p);
    int DecodeBitString(MyDataPacket* p);
    int DecodeOctetString(MyDataPacket* p);
    int DecodeNull(MyDataPacket* p);
    int DecodeOID(MyDataPacket* p);
    int DecodeUTF8String(MyDataPacket* p);
    int DecodePrintableString(MyDataPacket* p);
    int DecodeSequence(MyDataPacket* p);
    int DecodeSet(MyDataPacket* p);
    int DecodeIA5String(MyDataPacket* p);
    int DecodeBMPString(MyDataPacket* p);
    int DecodeUTCTime(MyDataPacket* p);
    int DecodeGeneralizedTime(MyDataPacket* p);
    int DecodeRaw(MyDataPacket* p);

    bool        BoolValue() { return m_Content.CharAt(0) != 0; }
    INT64       IntValue() {
        INT64 ret = 0;
        for (int i = 0; i < m_Content.Length(); i++) {
            char c = m_Content.CharAt(i);
            ret += (c * (1 << (m_Content.Length() - 1 - i) * 8));
        }
        return ret;
    }
    MyBuffer*   IntBytes() { return &m_Content; }
    MyBuffer*   Octet() { return &m_Content; }
    MyStringA*  OID() { return &m_OID; }
    MyBuffer*   String() { return &m_Content; }
    MyStringW*  WString() { return &m_BMPString; }
    MyAsn1Time* Time() { return &m_Time; }
    MyBuffer*   BitString() { return &m_Bits; }
    BYTE        UnusedBits() { return m_UnusedBits; }
    void        SetUnusedBits(BYTE v) { m_UnusedBits = v; }

    int Encode(MyDataPacket* p);
    int InitBool(bool val);
    int InitInteger(INT64 v);
    int InitInteger(const char* raw, int len);
    int InitBitstring(const char* bits, int bitsLen);
    int InitOctetString(const char* octet, int len);
    int InitNull();
    int InitOID(const char* oid, int len=-1);
    int InitUTF8String(const char* str, int len=-1);
    int InitSequence();
    int InitSet();
    int InitPrintableString(const char* str, int len=-1);
    int InitIA5String(const char* str, int len=-1);
    int InitUTCTime(MyAsn1Time* time);
    int InitGeneralizedTime(MyAsn1Time* time);
    int InitBMPStrign(wchar_t* str, int len=-1);
    int InitExplicit(BYTE tag);

    bool UseInfiniteLength() { return m_UseInfiniteLength; }
    void SetUseInfiniteLength(bool val) { m_UseInfiniteLength = val; }
    MyBuffer* Content() { return &m_Content; }
    MyBuffer* Raw() { return &m_Raw; }

    MyAsn1Node* Parent() { return m_Parent; }
    void SetParent(MyAsn1Node* p) { m_Parent = p; }
    int ChildCount() { return m_Children.Size(); }
    MyAsn1Node* Child(int index) { return m_Children.Get(index); }
    MyAsn1Node* AddChild() { return m_Children.AddNew(); }

private:
    int DecodeIDLengthContent(MyDataPacket* p);
    int EncodeLength(int len, MyDataPacket* p);

protected:
    BYTE                m_ID;
    bool                m_UseInfiniteLength;
    MyBuffer            m_Content;
    MyBuffer            m_Raw; // raw asn.1 encoding of current node

    MyAsn1Node*         m_Parent;
    MyArray<MyAsn1Node> m_Children;


    // for specific ASN.1 types
    MyStringA  m_OID;
    MyStringW  m_BMPString;
    MyBuffer   m_Bits;
    BYTE       m_UnusedBits;
    MyAsn1Time m_Time; // For UTCTime and GeneralizedTime

    MY_LAST_ERROR_DECL;
};

class MyAsn1 {
public:
    MyAsn1(): m_LastErrorCode(0) {}
    ~MyAsn1() {}

    int Decode(const char* data, int len);
    int Decode(MyDataPacket* buf);
    MyAsn1Node* Root() { return &m_Root; } // For Decode

    int Encode(MyBuffer* buf);

private:
    MyAsn1Node m_Root;

    MY_LAST_ERROR_DECL;
};

#endif // _MY_ASN_H_
