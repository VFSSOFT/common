#ifndef _MY_ASN_CPP_
#define _MY_ASN_CPP_

#include "MyASN1.h"



INT64 MyAsn1Integer::IntValue() {
    INT64 ret = 0;
    for (int i = 0; i < m_Content.Length(); i++) {
        char c = m_Content.CharAt(i);
        ret += (c * (1 << (m_Content.Length() - 1 - i) * 8));
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

    case MY_ASN1_TAG_BIT_STRING:
        if (err = DecodeBitString(p, node)) return err;
        break;

    case MY_ASN1_TAG_OCTET_STRING:
        if (err = DecodeOctetString(p, node)) return err;
        break;

    case MY_ASN1_TAG_NULL:
        if (err = DecodeNull(p, node)) return err;
        break;

    case MY_ASN1_TAG_OID:
        if (err = DecodeOID(p, node)) return err;
        break;

    case MY_ASN1_TAG_UTF8_STRING:
        if (err = DecodeUTF8String(p, node)) return err;
        break;

    case MY_ASN1_TAG_PRINTABLE_STRING:
        if (err = DecodePrintableString(p, node)) return err;
        break;

    case MY_ASN1_TAG_IA5_STRING:
        if (err = DecodeIA5String(p, node)) return err;
        break;

    case MY_ASN1_TAG_BMP_STRING:
        if (err = DecodeBMPString(p, node)) return err;
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
int MyAsn1::DecodeBitString(MyDataPacket* p, MyAsn1Node** node) {
    int err = 0;
    BYTE b;
    MyAsn1BitString* bitStringNode = new MyAsn1BitString();
    
    if (err = DecodeIDLengthContent(p, bitStringNode)) goto done;

    if (bitStringNode->TagNum() != MY_ASN1_TAG_BIT_STRING) {
        err = LastError(MY_ERR_BASE_ENCODING, "Invalid ASN.1 BIT STRING encoding");
        goto done;
    }

    b = bitStringNode->Content()->CharAt(0);
    if (b > 7) {
        err = LastError(MY_ERR_BASE_ENCODING, "Invalid ASN.1 BIT STRING encoding: Invalid Unused Bit flag");
        goto done;
    }
    bitStringNode->SetUnusedBits(b);
    bitStringNode->Bits()->Set(bitStringNode->Content()->Deref(1), bitStringNode->Content()->Length() - 1);
    *node = bitStringNode;

done:
    if (err) delete bitStringNode;
    return err;
}
int MyAsn1::DecodeOctetString(MyDataPacket* p, MyAsn1Node** node) {
    int err = 0;
    MyAsn1OctetString* octetStringNode = new MyAsn1OctetString();

    if (err = DecodeIDLengthContent(p, octetStringNode)) goto done;

    if (octetStringNode->TagNum() != MY_ASN1_TAG_OCTET_STRING) {
        err = LastError(MY_ERR_BASE_ENCODING, "Invalid ASN.1 OCTET STRING encoding");
        goto done;
    }
    *node = octetStringNode;

done:
    if (err) delete octetStringNode;
    return err;
}
int MyAsn1::DecodeNull(MyDataPacket* p, MyAsn1Node** node) {
    int err = 0;
    MyAsn1Null* nullNode = new MyAsn1Null();

    if (err = DecodeIDLengthContent(p, nullNode)) goto done;

    if (nullNode->TagNum() != MY_ASN1_TAG_NULL) {
        err = LastError(MY_ERR_BASE_ENCODING, "Invalid ASN.1 NULL encoding");
        goto done;
    }
    *node = nullNode;

done:
    if (err) delete nullNode;
    return err;
}
int MyAsn1::DecodeOID(MyDataPacket* p, MyAsn1Node** node) {
    int err = 0;
    MyAsn1OID* oidNode = new MyAsn1OID();
    MyBuffer* buf;
    int i = 0;
    int curComp;

    if (err = DecodeIDLengthContent(p, oidNode)) goto done;

    if (oidNode->TagNum() != MY_ASN1_TAG_OID) {
        err = LastError(MY_ERR_BASE_ENCODING, "Invalid ASN.1 OBJECT IDENTIFIER encoding");
        goto done;
    }

    if (oidNode->Content()->Length()) {
        buf = oidNode->Content();

        while (i < buf->Length()) {
            curComp = 0;
            while (i < buf->Length()) {
                curComp <<= 7;
                curComp += buf->CharAt(i) & 0x7F;
                
                if ((buf->CharAt(i) & 0x80) == 0) {
                    i++;
                    break;
                }
                i++;
            }
            
            if (oidNode->OID()->Length() == 0) {
                oidNode->OID()->AppendInt(curComp / 40);
                oidNode->OID()->AppendChar('.');
                oidNode->OID()->AppendInt(curComp % 40);
            } else {
                oidNode->OID()->AppendChar('.');
                oidNode->OID()->AppendInt(curComp);
            }

        }
    }

    *node = oidNode;

done:
    if (err) delete oidNode;
    return err;
}

int MyAsn1::DecodeUTF8String(MyDataPacket* p, MyAsn1Node** node) {
    int err = 0;
    MyAsn1UTF8String* strNode = new MyAsn1UTF8String();

    if (err = DecodeIDLengthContent(p, strNode)) goto done;

    if (strNode->TagNum() != MY_ASN1_TAG_UTF8_STRING) {
        err = LastError(MY_ERR_BASE_ENCODING, "Invalid ASN.1 UTF8 STRING encoding");
        goto done;
    }
    *node = strNode;

done:
    if (err) delete strNode;
    return err;
}

int MyAsn1::DecodePrintableString(MyDataPacket* p, MyAsn1Node** node) {
    int err = 0;
    MyAsn1PrintableString* strNode = new MyAsn1PrintableString();

    if (err = DecodeIDLengthContent(p, strNode)) goto done;

    if (strNode->TagNum() != MY_ASN1_TAG_PRINTABLE_STRING) {
        err = LastError(MY_ERR_BASE_ENCODING, "Invalid ASN.1 PRINTABLE STRING encoding");
        goto done;
    }
    *node = strNode;

done:
    if (err) delete strNode;
    return err;
}
int MyAsn1::DecodeIA5String(MyDataPacket* p, MyAsn1Node** node) {
    int err = 0;
    MyAsn1IA5String* strNode = new MyAsn1IA5String();

    if (err = DecodeIDLengthContent(p, strNode)) goto done;

    if (strNode->TagNum() != MY_ASN1_TAG_IA5_STRING) {
        err = LastError(MY_ERR_BASE_ENCODING, "Invalid ASN.1 IA5 STRING encoding");
        goto done;
    }
    *node = strNode;

done:
    if (err) delete strNode;
    return err;
}
int MyAsn1::DecodeBMPString(MyDataPacket* p, MyAsn1Node** node) {
    int err = 0;
    MyAsn1BMPString* strNode = new MyAsn1BMPString();
    int i = 0;

    if (err = DecodeIDLengthContent(p, strNode)) goto done;

    if (strNode->TagNum() != MY_ASN1_TAG_BMP_STRING) {
        err = LastError(MY_ERR_BASE_ENCODING, "Invalid ASN.1 BMP STRING encoding");
        goto done;
    }
    for (int i = 0; i < strNode->Content()->Length(); i += 2) {
        strNode->String()->AppendChar(strNode->Content()->CharAt(i+1) | (strNode->Content()->CharAt(i) << 8));
    }
    *node = strNode;

done:
    if (err) delete strNode;
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

