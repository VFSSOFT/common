#ifndef _MY_ASN_H_
#define _MY_ASN_H_

#include "MyBuffer.h"
#include "MyStringA.h"
#include "MyArray.h"
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


class MyAsn1Node {
public:
    MyAsn1Node(): m_ID(0), m_UseInfiniteLength(false), m_Parent(NULL) {}

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

    bool UseInfiniteLength() { return m_UseInfiniteLength; }
    void SetUseInfiniteLength(bool val) { m_UseInfiniteLength = val; }
    MyBuffer* Content() { return &m_Content; }
    MyBuffer* Raw() { return &m_Raw; }

    MyAsn1Node* Parent() { return m_Parent; }
    void SetParent(MyAsn1Node* p) { m_Parent = p; }
    int ChildCount() { return m_Children.Size(); }
    MyAsn1Node* Child(int index) { return m_Children.Get(index); }
    MyAsn1Node* AddChild() { return m_Children.AddNew(); }

protected:
    BYTE                m_ID;
    bool                m_UseInfiniteLength;
    MyBuffer            m_Content;
    MyBuffer            m_Raw; // raw asn.1 encoding of current node

    MyAsn1Node*         m_Parent;
    MyArray<MyAsn1Node> m_Children;
};

class MyAsn1Bool : public MyAsn1Node {
public:
    bool Value() { return m_Content.CharAt(0) != 0; }
};
class MyAsn1Integer : public MyAsn1Node {
public:
    INT64 IntValue();
    MyBuffer* IntBytes() { return &m_Content; }
};
class MyAsn1BitString : public MyAsn1Node {
public:
    MyAsn1BitString(): MyAsn1Node(), m_UnusedBits(0) {}

    MyBuffer* Bits() { return &m_Bits;  }
    BYTE UnusedBits() { return m_UnusedBits; }
    void SetUnusedBits(BYTE v) { m_UnusedBits = v; }

private:
    MyBuffer m_Bits;
    BYTE     m_UnusedBits;
};
class MyAsn1OctetString : public MyAsn1Node {
public:
    MyBuffer* Octet() { return &m_Content; }
};
class MyAsn1Null : public MyAsn1Node {};
class MyAsn1OID : public MyAsn1Node {
public:
    MyStringA* OID() { return &m_OID; }

private:
    MyStringA m_OID;
};
class MyAsn1UTF8String: public MyAsn1Node {
public:
    MyBuffer* String() { return &m_Content; }
};
class MyAsn1PrintableString: public MyAsn1Node {
public:
    MyBuffer* String() { return &m_Content; }
};
class MyAsn1IA5String : public MyAsn1Node {
public:
    MyBuffer* String() { return &m_Content; }
};
class MyAsn1BMPString : public MyAsn1Node {
public:
    MyStringW* String() { return &m_String; }

private:
    MyStringW m_String;
};
class MyAsn1UTCTime : public MyAsn1Node {
public:

    MyAsn1UTCTime(): MyAsn1Node(), Year(0), Month(0), Day(0), Hour(0), Minute(0),
        Second(0), HourOffset(0), MinuteOffset(0) {}

    int Year;
    int Month;
    int Day;
    int Hour;
    int Minute;
    int Second;
    int HourOffset;
    int MinuteOffset;
};
class MyAsn1GeneralizedTime : public MyAsn1Node {
public:

    MyAsn1GeneralizedTime(): MyAsn1Node(), Year(0), Month(0), Day(0), Hour(0), Minute(0),
        Second(0), Millisecond(0), HourOffset(0), MinuteOffset(0) {}

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

class MyAsn1 {
public:
    MyAsn1(): m_Root(NULL), m_LastErrorCode(0) {}
    ~MyAsn1() {}

    int Decode(const char* data, int len);
    int Decode(MyDataPacket* buf);

    MyAsn1Node* Root() { return m_Root; }


private:
    int Decode(MyDataPacket* p, MyAsn1Node** node);

    int DecodeBool(MyDataPacket* p, MyAsn1Node** node);
    int DecodeInteger(MyDataPacket* p, MyAsn1Node** node);
    int DecodeBitString(MyDataPacket* p, MyAsn1Node** node);
    int DecodeOctetString(MyDataPacket* p, MyAsn1Node** node);
    int DecodeNull(MyDataPacket* p, MyAsn1Node** node);
    int DecodeOID(MyDataPacket* p, MyAsn1Node** node);
    int DecodeUTF8String(MyDataPacket* p, MyAsn1Node** node);
    int DecodePrintableString(MyDataPacket* p, MyAsn1Node** node);
    int DecodeIA5String(MyDataPacket* p, MyAsn1Node** node);
    int DecodeBMPString(MyDataPacket* p, MyAsn1Node** node);
    int DecodeUTCTime(MyDataPacket* p, MyAsn1Node** node);
    int DecodeGeneralizedTime(MyDataPacket* p, MyAsn1Node** node);

    int DecodeIDLengthContent(MyDataPacket* p, MyAsn1Node* node);

private:
    MyAsn1Node* m_Root;

    MY_LAST_ERROR_DECL;
};

#endif // _MY_ASN_H_
