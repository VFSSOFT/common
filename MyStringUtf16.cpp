#include "MyStringUtf16.h"

MyStringUtf16::MyStringUtf16() : MyBuffer() {}
MyStringUtf16::~MyStringUtf16() {}

int MyStringUtf16::Set(UCHAR* str) {
  return MyBuffer::Set((char*)str, MyStringUtf16::StrLen(str) * sizeof(UCHAR));
}
int MyStringUtf16::Append(UCHAR* str) {
  return MyBuffer::Set((char*)str, MyStringUtf16::StrLen(str) * sizeof(UCHAR));
}