#include "pch.h"

#include "../../common/MyASN1.h"


TEST(MyASN1Test, DecodeNULL) {
    int err = 0;
    MyAsn1 d;
    char nullBytes[] = { 0x01, 0x01, 0xFF };
    err = d.Decode(nullBytes, 3);
    ASSERT_EQ(err, 0);

    MyAsn1Node* node = d.Root();
    ASSERT_EQ(node->TagNum(), MY_ASN1_TAG_BOOL);
    MyAsn1Bool* boolNode = (MyAsn1Bool*)node;
    ASSERT_TRUE(boolNode->Value());
}

TEST(MyASN1Test, DecodeInteger0) {
    int err = 0;
    MyAsn1 d;
    char nullBytes[] = { 0x02, 0x01, 0x00 };
    err = d.Decode(nullBytes, 3);
    ASSERT_EQ(err, 0);

    MyAsn1Node* node = d.Root();
    ASSERT_EQ(node->TagNum(), MY_ASN1_TAG_INTEGER);
    MyAsn1Integer* intNode = (MyAsn1Integer*)node;
    ASSERT_EQ(intNode->IntValue(), 0);
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