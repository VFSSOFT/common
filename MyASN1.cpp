#ifndef _MY_ASN_CPP_
#define _MY_ASN_CPP_

#include "MyASN1.h"



INT64 MyAsn1Integer::IntValue() {
    INT64 ret = 0;
    for (int i = 0; i < m_Content.Length(); i++) {
        char c = m_Content.CharAt(i);
        ret += (c << (m_Content.Length() - 1 - i));
    }
    return ret;
}

int MyAsn1::Decode(const char* data, int len) {
    int err = 0;
    MyDataPacket p;

    if (err = p.Write(data, len)) return LastError(err, p.LastErrorMessage());

    return Decode(&p);
}

int MyAsn1::Decode(MyDataPacket* p) {
    return Decode(p, &m_Root);
    
}
int MyAsn1::Decode(MyDataPacket* p, MyAsn1Node** node) {
    int err = 0;
    int b = 0;

    if (err = p->PeekByte(&b)) return LastError(err, p->LastErrorMessage());
    
    BYTE tagNum = b & MY_ASN1_TAG_MASK;
    switch (tagNum) {
    
    case MY_ASN1_TAG_BOOL:
        if (err = DecodeBool(p, node)) return err;
        break;
    
    case MY_ASN1_TAG_INTEGER:
        if (err = DecodeInteger(p, node)) return err;
        break;

    case MY_ASN1_TAG_NULL:
        break;
    
    }

    return 0;
}
int MyAsn1::DecodeBool(MyDataPacket* p, MyAsn1Node** node) {
    int err = 0;
    MyAsn1Bool* boolNode = new MyAsn1Bool();

    if (err = DecodeIDLengthContent(p, boolNode)) goto done;

    if (boolNode->Raw()->CharAt(0) != 1 || boolNode->Raw()->CharAt(1) != 1) {
        err = LastError(MY_ERR_BASE_ENCODING, "Invalid ASN.1 BOOL encoding");
        goto done;
    }
    *node = boolNode;

done:
    if (err) delete boolNode;
    return err;
}
int MyAsn1::DecodeInteger(MyDataPacket* p, MyAsn1Node** node) {
    int err = 0;
    MyAsn1Integer* intNode = new MyAsn1Integer();

    if (err = DecodeIDLengthContent(p, intNode)) goto done;

    if (intNode->TagNum() != MY_ASN1_TAG_INTEGER) {
        err = LastError(MY_ERR_BASE_ENCODING, "Invalid ASN.1 INTEGER encoding");
        goto done;
    }
    *node = intNode;

done:
    if (err) delete intNode;
    return err;
}

int MyAsn1::DecodeIDLengthContent(MyDataPacket* p, MyAsn1Node* node) {
    int err = 0;
    int b = 0;
    UINT64 length = 0;
    int startPos = p->ReadOffset();

    if (err = p->ReadInt8(&b)) return LastError(err, p->LastErrorMessage());
    node->SetID(b);

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

    MyBuffer* content = node->Content();
    if (err = content->SetLength(length)) return err;
    if (err = p->ReadBytes(length, content->Deref())) return LastError(err, p->LastErrorMessage());

    MyBuffer* raw = node->Raw();
    int rawLen = p->ReadOffset() - startPos;
    if (err = raw->Set(p->Deref() + startPos, rawLen)) return err;

    return 0;
}

#endif // _MY_ASN_CPP_

