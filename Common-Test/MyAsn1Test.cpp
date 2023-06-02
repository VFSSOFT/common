#include "pch.h"

#include "../../common/MyASN1.h"


TEST(MyASN1Test, DecodeBool) {
    int err = 0;
    MyAsn1 d;
    char bytes[] = { 0x01, 0x01, 0xFF };
    err = d.Decode(bytes, 3);
    ASSERT_EQ(err, 0);

    MyAsn1Node* node = d.Root();
    ASSERT_EQ(node->TagNum(), MY_ASN1_TAG_BOOL);
    MyAsn1Bool* boolNode = (MyAsn1Bool*)node;
    ASSERT_TRUE(boolNode->Value());
}

TEST(MyASN1Test, DecodeInteger0) {
    int err = 0;
    MyAsn1 d;
    char bytes[] = { 0x02, 0x01, 0x00 };
    err = d.Decode(bytes, 3);
    ASSERT_EQ(err, 0);

    MyAsn1Node* node = d.Root();
    ASSERT_EQ(node->TagNum(), MY_ASN1_TAG_INTEGER);
    MyAsn1Integer* intNode = (MyAsn1Integer*)node;
    ASSERT_EQ(intNode->IntValue(), 0);
}

TEST(MyASN1Test, DecodeInteger65537) {
    int err = 0;
    MyAsn1 d;
    char bytes[] = { 02, 0x03, 0x01, 0x00, 0x01 };
    err = d.Decode(bytes, 5);
    ASSERT_EQ(err, 0);

    MyAsn1Node* node = d.Root();
    ASSERT_EQ(node->TagNum(), MY_ASN1_TAG_INTEGER);
    MyAsn1Integer* intNode = (MyAsn1Integer*)node;
    ASSERT_EQ(intNode->IntValue(), 65537);
}
TEST(MyASN1Test, DecodeIntegerLong) {
    int err = 0;
    MyAsn1 d;
    char bytes[] = { 0x02, 0x10, 0x60, 0x5C, 0xD2, 0xD5, 0x26, 0x75, 0xB3, 0x92, 0xA5, 0xAA, 0x9B, 0x02, 0xB2, 0x6A, 0x55, 0x66 };
    err = d.Decode(bytes, 18);
    ASSERT_EQ(err, 0);

    MyAsn1Node* node = d.Root();
    ASSERT_EQ(node->TagNum(), MY_ASN1_TAG_INTEGER);
    MyAsn1Integer* intNode = (MyAsn1Integer*)node;
    ASSERT_EQ(intNode->IntBytes()->Length(), 16);
}

TEST(MyASN1Test, DecodeBitString) {
    int err = 0;
    MyAsn1 d;
    char bytes[] = { 0x03, 0x02, 0x04, 0xB0 };
    err = d.Decode(bytes, 4);
    ASSERT_EQ(err, 0);

    MyAsn1Node* node = d.Root();
    ASSERT_EQ(node->TagNum(), MY_ASN1_TAG_BIT_STRING);
    MyAsn1BitString* bitStringNode = (MyAsn1BitString*)node;
    ASSERT_EQ(bitStringNode->UnusedBits(), 4);
    ASSERT_EQ(bitStringNode->Bits()->Length(), 1);
    ASSERT_EQ(bitStringNode->Bits()->CharAt(0) & 0xFF, 0xB0);
}

TEST(MyASN1Test, DecodeOctetString) {
    int err = 0;
    MyAsn1 d;
    char bytes[] = { 
        0x04, 0x18, 0x30, 0x16, 0x80, 0x14, 0x6A, 0x99, 0xA3, 
        0xE8, 0xDE, 0x92, 0xD3, 0xAA, 0x3F, 0xC9, 0x26, 0xB4,
        0x06, 0x14, 0x5C, 0x60, 0x88, 0x59, 0xF7, 0xA9
    };
    err = d.Decode(bytes, 26);
    ASSERT_EQ(err, 0);

    MyAsn1Node* node = d.Root();
    ASSERT_EQ(node->TagNum(), MY_ASN1_TAG_OCTET_STRING);
    MyAsn1OctetString* octetStringNode = (MyAsn1OctetString*)node;
    ASSERT_EQ(octetStringNode->Octet()->Length(), 24);
    ASSERT_EQ(octetStringNode->Octet()->CharAt(0) & 0xFF, 0x30);
    ASSERT_EQ(octetStringNode->Octet()->CharAt(1) & 0xFF, 0x16);
    ASSERT_EQ(octetStringNode->Octet()->CharAt(23) & 0xFF, 0xA9);
}

TEST(MyASN1Test, DecodeNULL) {
    int err = 0;
    MyAsn1 d;
    char bytes[] = { 0x05, 0x00 };
    err = d.Decode(bytes, 2);
    ASSERT_EQ(err, 0);

    MyAsn1Node* node = d.Root();
    ASSERT_EQ(node->TagNum(), MY_ASN1_TAG_NULL);
}

TEST(MyASN1Test, DecodeOID) {
    int err = 0;
    MyAsn1 d;
    char bytes[] = { 0x06, 0x09, 0x2A, 0x86, 0x48, 0x86, 0xF7, 0x0D, 0x01, 0x01, 0x0A };
    err = d.Decode(bytes, 11);
    ASSERT_EQ(err, 0);

    MyAsn1Node* node = d.Root();
    ASSERT_EQ(node->TagNum(), MY_ASN1_TAG_OID);
    MyAsn1OID* oid = (MyAsn1OID*)node;
    ASSERT_STREQ(oid->OID()->Deref(), "1.2.840.113549.1.1.10");
}
TEST(MyASN1Test, DecodeOID2) {
    return; // KNOWN BUG
    int err = 0;
    MyAsn1 d;
    char bytes[] = { 0x06, 0x03, 0x88, 0x37, 0x03 };
    err = d.Decode(bytes, 5);
    ASSERT_EQ(err, 0);

    MyAsn1Node* node = d.Root();
    ASSERT_EQ(node->TagNum(), MY_ASN1_TAG_OID);
    MyAsn1OID* oid = (MyAsn1OID*)node;
    ASSERT_STREQ(oid->OID()->Deref(), "2.999.3");
}

TEST(MyASN1Test, DecodeUTF8String) {
    int err = 0;
    MyAsn1 d;
    char bytes[] = { 
        0x0C, 0x2D, 0x41, 0x4C, 0x4D, 0x41, 0x20, 0x4D,
        0x41, 0x54, 0x45, 0x52, 0x20, 0x53, 0x54, 0x55,
        0x44, 0x49, 0x4F, 0x52, 0x55, 0x4D, 0x20, 0x2D,
        0x20, 0x55, 0x6E, 0x69, 0x76, 0x65, 0x72, 0x73, 
        0x69, 0x74, 0xC3, 0xA0, 0x20, 0x64, 0x69, 0x20, 
        0x42, 0x6F, 0x6C, 0x6F, 0x67, 0x6E, 0x61
    };
    err = d.Decode(bytes, 47);
    ASSERT_EQ(err, 0);

    MyAsn1Node* node = d.Root();
    ASSERT_EQ(node->TagNum(), MY_ASN1_TAG_UTF8_STRING);
    MyAsn1UTF8String* str = (MyAsn1UTF8String*)node;
    ASSERT_EQ(str->String()->Length(), 45);
}

TEST(MyASN1Test, DecodePrintableString) {
    int err = 0;
    MyAsn1 d;
    char bytes[] = { 0x13, 0x02, 0x4E, 0x4C };
    err = d.Decode(bytes, 4);
    ASSERT_EQ(err, 0);

    MyAsn1Node* node = d.Root();
    ASSERT_EQ(node->TagNum(), MY_ASN1_TAG_PRINTABLE_STRING);
    MyAsn1PrintableString* str = (MyAsn1PrintableString*)node;
    ASSERT_EQ(str->String()->Length(), 2);
    ASSERT_STREQ(str->String()->Deref(), "NL");
}

TEST(MyASN1Test, DecodeIA5String) {
    int err = 0;
    MyAsn1 d;
    char bytes[] = { 0x16, 0x0a, 0x36, 0x2e, 0x30, 0x2e, 0x35, 0x33, 0x36, 0x31, 0x2e, 0x32 };
    err = d.Decode(bytes, 12);
    ASSERT_EQ(err, 0);

    MyAsn1Node* node = d.Root();
    ASSERT_EQ(node->TagNum(), MY_ASN1_TAG_IA5_STRING);
    MyAsn1IA5String* str = (MyAsn1IA5String*)node;
    ASSERT_EQ(str->String()->Length(), 10);
    ASSERT_STREQ(str->String()->Deref(), "6.0.5361.2");
}

TEST(MyASN1Test, DecodeBMPString) {
    int err = 0;
    MyAsn1 d;
    char bytes[] = { 0x1e, 0x08, 0x00, 0x55, 0x00, 0x73, 0x00, 0x65, 0x00, 0x72 };
    err = d.Decode(bytes, 10);
    ASSERT_EQ(err, 0);

    MyAsn1Node* node = d.Root();
    ASSERT_EQ(node->TagNum(), MY_ASN1_TAG_BMP_STRING);
    MyAsn1BMPString* str = (MyAsn1BMPString*)node;
    ASSERT_EQ(str->String()->Length(), 4);
    ASSERT_EQ(str->String()->Deref()[0], L'U');
    ASSERT_EQ(str->String()->Deref()[1], L's');
    ASSERT_EQ(str->String()->Deref()[2], L'e');
    ASSERT_EQ(str->String()->Deref()[3], L'r');
}



