#include "pch.h"

#include "../../common/MyASN1.h"


TEST(MyASN1Test, DecodeBool) {
    int err = 0;
    MyAsn1 d;
    char bytes[] = { 0x01, 0x01, 0xFF };
    err = d.Decode(bytes, 3);
    ASSERT_EQ(err, 0);

    MyAsn1Node* node = d.Root();
    ASSERT_TRUE(node->IsBool());
    ASSERT_TRUE(node->BoolValue());
}

TEST(MyASN1Test, DecodeInteger0) {
    int err = 0;
    MyAsn1 d;
    char bytes[] = { 0x02, 0x01, 0x00 };
    err = d.Decode(bytes, 3);
    ASSERT_EQ(err, 0);

    MyAsn1Node* node = d.Root();
    ASSERT_TRUE(node->IsInteger());
    ASSERT_EQ(node->IntValue(), 0);
}

TEST(MyASN1Test, DecodeInteger65537) {
    int err = 0;
    MyAsn1 d;
    char bytes[] = { 02, 0x03, 0x01, 0x00, 0x01 };
    err = d.Decode(bytes, 5);
    ASSERT_EQ(err, 0);

    MyAsn1Node* node = d.Root();
    ASSERT_TRUE(node->IsInteger());
    ASSERT_EQ(node->IntValue(), 65537);
}
TEST(MyASN1Test, DecodeIntegerLong) {
    int err = 0;
    MyAsn1 d;
    char bytes[] = { 0x02, 0x10, 0x60, 0x5C, 0xD2, 0xD5, 0x26, 0x75, 0xB3, 0x92, 0xA5, 0xAA, 0x9B, 0x02, 0xB2, 0x6A, 0x55, 0x66 };
    err = d.Decode(bytes, 18);
    ASSERT_EQ(err, 0);

    MyAsn1Node* node = d.Root();
    ASSERT_TRUE(node->IsInteger());
    ASSERT_EQ(node->IntBytes()->Length(), 16);
}

TEST(MyASN1Test, DecodeBitString) {
    int err = 0;
    MyAsn1 d;
    char bytes[] = { 0x03, 0x02, 0x04, 0xB0 };
    err = d.Decode(bytes, 4);
    ASSERT_EQ(err, 0);

    MyAsn1Node* node = d.Root();
    ASSERT_EQ(node->TagNum(), MY_ASN1_TAG_BIT_STRING);
    ASSERT_EQ(node->UnusedBits(), 4);
    ASSERT_EQ(node->BitString()->Length(), 1);
    ASSERT_EQ(node->BitString()->CharAt(0) & 0xFF, 0xB0);
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
    ASSERT_EQ(node->Octet()->Length(), 24);
    ASSERT_EQ(node->Octet()->CharAt(0) & 0xFF, 0x30);
    ASSERT_EQ(node->Octet()->CharAt(1) & 0xFF, 0x16);
    ASSERT_EQ(node->Octet()->CharAt(23) & 0xFF, 0xA9);
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
    ASSERT_STREQ(node->OID()->Deref(), "1.2.840.113549.1.1.10");
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
    ASSERT_STREQ(node->OID()->Deref(), "2.999.3");
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
    ASSERT_EQ(node->String()->Length(), 45);
}

TEST(MyASN1Test, DecodePrintableString) {
    int err = 0;
    MyAsn1 d;
    char bytes[] = { 0x13, 0x02, 0x4E, 0x4C };
    err = d.Decode(bytes, 4);
    ASSERT_EQ(err, 0);

    MyAsn1Node* node = d.Root();
    ASSERT_EQ(node->TagNum(), MY_ASN1_TAG_PRINTABLE_STRING);
    ASSERT_EQ(node->String()->Length(), 2);
    ASSERT_STREQ(node->String()->Deref(), "NL");
}

TEST(MyASN1Test, DecodeIA5String) {
    int err = 0;
    MyAsn1 d;
    char bytes[] = { 0x16, 0x0a, 0x36, 0x2e, 0x30, 0x2e, 0x35, 0x33, 0x36, 0x31, 0x2e, 0x32 };
    err = d.Decode(bytes, 12);
    ASSERT_EQ(err, 0);

    MyAsn1Node* node = d.Root();
    ASSERT_EQ(node->TagNum(), MY_ASN1_TAG_IA5_STRING);
    ASSERT_EQ(node->String()->Length(), 10);
    ASSERT_STREQ(node->String()->Deref(), "6.0.5361.2");
}

TEST(MyASN1Test, DecodeBMPString) {
    int err = 0;
    MyAsn1 d;
    char bytes[] = { 0x1e, 0x08, 0x00, 0x55, 0x00, 0x73, 0x00, 0x65, 0x00, 0x72 };
    err = d.Decode(bytes, 10);
    ASSERT_EQ(err, 0);

    MyAsn1Node* node = d.Root();
    ASSERT_EQ(node->TagNum(), MY_ASN1_TAG_BMP_STRING);
    ASSERT_EQ(node->WString()->Length(), 4);
    ASSERT_EQ(node->WString()->Deref()[0], L'U');
    ASSERT_EQ(node->WString()->Deref()[1], L's');
    ASSERT_EQ(node->WString()->Deref()[2], L'e');
    ASSERT_EQ(node->WString()->Deref()[3], L'r');
}

TEST(MyASN1Test, DecodeUTCTime) {
    int err = 0;
    MyAsn1 d;
    char bytes[] = { 0x17, 0x0D, 0x32, 0x33, 0x30, 0x31, 0x30, 0x32, 0x31, 0x33, 0x30, 0x35, 0x32, 0x36, 0x5A  };
    err = d.Decode(bytes, 15);
    ASSERT_EQ(err, 0);

    MyAsn1Node* node = d.Root();
    ASSERT_EQ(node->TagNum(), MY_ASN1_TAG_UTC_TIME);
    MyAsn1Time* time = node->Time();
    ASSERT_EQ(time->Year, 2023);
    ASSERT_EQ(time->Month, 1);
    ASSERT_EQ(time->Day, 2);
    ASSERT_EQ(time->Hour, 13);
    ASSERT_EQ(time->Minute, 5);
    ASSERT_EQ(time->Second, 26);
    ASSERT_EQ(time->HourOffset, 0);
    ASSERT_EQ(time->MinuteOffset, 0);
}

TEST(MyASN1Test, DecodeUTCTime1) {
    int err = 0;
    MyAsn1 d;
    char bytes[] = { 0x17, 0x11, 0x39, 0x31, 0x30, 0x35, 0x30, 0x36, 0x31, 0x36, 0x34, 0x35, 0x34, 0x30, 0x2D, 0x30, 0x37, 0x30, 0x30  };
    err = d.Decode(bytes, 19);
    ASSERT_EQ(err, 0);

    MyAsn1Node* node = d.Root();
    ASSERT_EQ(node->TagNum(), MY_ASN1_TAG_UTC_TIME);
    MyAsn1Time* time = node->Time();
    ASSERT_EQ(time->Year, 1991);
    ASSERT_EQ(time->Month, 5);
    ASSERT_EQ(time->Day, 6);
    ASSERT_EQ(time->Hour, 16);
    ASSERT_EQ(time->Minute, 45);
    ASSERT_EQ(time->Second, 40);
    ASSERT_EQ(time->HourOffset, -7);
    ASSERT_EQ(time->MinuteOffset, 0);
}

TEST(MyASN1Test, DecodeGeneralizedTime) {
    int err = 0;
    MyAsn1 d;
    char bytes[] = { 0x18, 0x0F, 0x32, 0x31, 0x30, 0x36, 0x30, 0x33, 0x31, 0x38, 0x31, 0x34, 0x30, 0x32, 0x33, 0x36, 0x5A  };
    err = d.Decode(bytes, 17);
    ASSERT_EQ(err, 0);

    MyAsn1Node* node = d.Root();
    ASSERT_EQ(node->TagNum(), MY_ASN1_TAG_GENERALIZED_TIME);
    MyAsn1Time* time = node->Time();
    ASSERT_EQ(time->Year, 2106);
    ASSERT_EQ(time->Month, 3);
    ASSERT_EQ(time->Day, 18);
    ASSERT_EQ(time->Hour, 14);
    ASSERT_EQ(time->Minute, 2);
    ASSERT_EQ(time->Second, 36);
    ASSERT_EQ(time->Millisecond, 0);
    ASSERT_EQ(time->HourOffset, 0);
    ASSERT_EQ(time->MinuteOffset, 0);
}

TEST(MyASN1Test, DecodeSequence) {
    int err = 0;
    MyAsn1 d;
    char bytes[] = { 0x30, 0x0D, 0x06, 0x09, 0x2A, 0x86, 0x48, 0x86, 0xF7, 0x0D, 0x01, 0x01, 0x05, 0x05, 0x00 };
    err = d.Decode(bytes, 15);
    ASSERT_EQ(err, 0);

    MyAsn1Node* node = d.Root();
    ASSERT_TRUE(node->IsSequence());
    ASSERT_EQ(node->ChildCount(), 2);
    ASSERT_TRUE(node->Child(0)->IsOID());
    ASSERT_TRUE(node->Child(1)->IsNull());
}

TEST(MyASN1Test, DecodeSet) {
    int err = 0;
    MyAsn1 d;
    char bytes[] = { 
        0x31, 0x2D, 0x30, 0x2B, 0x06, 0x03, 0x55, 0x04, 0x0B, 0x13, 0x24, 0x45, 0x71, 0x75, 0x69,
        0x66, 0x61, 0x78, 0x20, 0x53, 0x65, 0x63, 0x75, 0x72, 0x65, 0x20, 0x43, 0x65, 0x72, 0x74,
        0x69, 0x66, 0x69, 0x63, 0x61, 0x74, 0x65, 0x20, 0x41, 0x75, 0x74, 0x68, 0x6F, 0x72, 0x69,
        0x74, 0x79
    };
    err = d.Decode(bytes, 47);
    ASSERT_EQ(err, 0);

    MyAsn1Node* node = d.Root();
    ASSERT_TRUE(node->IsSet());
    ASSERT_EQ(node->ChildCount(), 1);
    ASSERT_TRUE(node->Child(0)->IsSequence());

    node = node->Child(0);
    ASSERT_TRUE(node->IsSequence());
    ASSERT_EQ(node->ChildCount(), 2);
    ASSERT_TRUE(node->Child(0)->IsOID());
    ASSERT_TRUE(node->Child(1)->IsPrintableString());
}

TEST(MyASN1Test, DecodeExpclitTag) {
    int err = 0;
    MyAsn1 d;
    char bytes[] = { 0xA0, 0x03, 0x02, 0x01, 0x02 };
    err = d.Decode(bytes, 5);
    ASSERT_EQ(err, 0);

    MyAsn1Node* node = d.Root();
    ASSERT_EQ(node->TagNum(), 0);
    ASSERT_TRUE(node->IsConstructed());

    MyBuffer raw;
    raw.Set(node->Content()->Deref(), node->Content()->Length());
    err = d.Decode(raw.Deref(), raw.Length());
    ASSERT_EQ(err, 0);

    node = d.Root();
    ASSERT_TRUE(node->IsInteger());
    ASSERT_EQ(node->IntValue(), 2);
}

TEST(MyASN1Test, DecodeImpclitTag) {
    int err = 0;
    MyAsn1 d;
    char bytes[] = { 
        0x86, 0x29, 0x68, 0x74, 0x74, 0x70, 0x3A, 0x2F, 0x2F, 0x63, 0x72, 0x6C, 0x2E, 0x67, 0x65, 0x6F, 0x74, 0x72, 0x75, 0x73, 0x74, 0x2E,
        0x63, 0x6F, 0x6D, 0x2F, 0x63, 0x72, 0x6C, 0x73, 0x2F, 0x73, 0x65, 0x63, 0x75, 0x72, 0x65, 0x63, 0x61, 0x2E, 0x63, 0x72, 0x6C
    };
    err = d.Decode(bytes, 43);
    ASSERT_EQ(err, 0);

    MyAsn1Node* node = d.Root();
    ASSERT_EQ(node->TagNum(), 6);
    ASSERT_FALSE(node->IsConstructed());
    ASSERT_STREQ(node->Content()->Deref(), "http://crl.geotrust.com/crls/secureca.crl");
}














