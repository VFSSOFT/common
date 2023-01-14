#ifndef _MY_JSON_H_
#define _MY_JSON_H_

#include "../common/MyStringA.h"
#include "../common/MyBuffer.h"
#include "../common/MyArray.h"

class MyJsonValue;

class MyJsonObject {
public:
  MyArray<MyJsonValue> Values;
};

class MyJsonArray: public MyJsonObject {};

enum class MyJsonValueType {
  stringValue,
  numberValue,
  objectValue,
  arrayValue,
  boolValue,
  nullValue
};


class MyJsonValue {
public:
  MyJsonValue() {
    m_LastErrorCode = 0;

    m_Key         = NULL;
    m_StringValue = NULL;
    m_ObjectValue = NULL;
    m_ArrayValue  = NULL;
    m_NumberValue = NULL;
    m_BoolValue   = -1;

    m_ValueType   = MyJsonValueType::nullValue;
  }
  virtual ~MyJsonValue() {
    if (m_Key) delete m_Key;
    ResetValue();
  }

  inline MyJsonValueType ValueType() { return m_ValueType; }
  void SetValueType(MyJsonValueType t);

  inline MyStringA* InitKey() { 
    if (m_Key == NULL) 
      m_Key = new MyStringA();
    return m_Key;
  }

  inline MyStringA* Key() { return m_Key; }
  inline MyStringA* StringValue() { return m_StringValue; }
  inline MyStringA* NumberValue() { return m_NumberValue; }
  inline MyJsonObject* ObjectValue() { return m_ObjectValue; }
  inline MyJsonArray* ArrayValue() { return m_ArrayValue; }
  
  inline void MarkAsTrue() { m_BoolValue = 1; }
  inline void MarkAsFalse() { m_BoolValue = 0; }
  inline bool BoolValue() { return m_BoolValue != 0; }

  MyJsonValue* FindChildByName(const char* nodeName);

  MyJsonValue*  AddObjectChild(const char* name);
  MyJsonValue*  AddArrayChild(const char* name);
  MyStringA*    AddStringChild(const char* name, const char* value);
  MyStringA*    AddNumberChild(const char* name, INT64 val);
  MyStringA*    AddNumberChild(const char* name, const char* num);
  int           AddBoolChild(const char* name, bool value);
  int           AddNullChild(const char* name);
  int           ToString(MyStringA* retJson, bool compact=false, int indent=0);

private:
  int EncodeJsonString(MyStringA* ret, MyStringA* str);
  int EncodeJsonObject(MyStringA* ret, MyJsonObject* obj, bool comact, int indent);
  int EncodeJsonArray(MyStringA* ret, MyJsonArray* arr, bool compact, int indent);
  int AddIndent(MyStringA* ret, int indent);

  void ResetValue() {
    if (m_StringValue) delete m_StringValue;
    if (m_ObjectValue) delete m_ObjectValue;
    if (m_ArrayValue)  delete m_ArrayValue;
    if (m_NumberValue) delete m_NumberValue;

    m_ValueType = MyJsonValueType::nullValue;
  }

private:
  MyJsonValueType m_ValueType;

  MyStringA*    m_Key;

  MyStringA*    m_StringValue;
  MyStringA*    m_NumberValue;
  MyJsonObject* m_ObjectValue;
  MyJsonArray*  m_ArrayValue;
  int           m_BoolValue;

  MY_LAST_ERROR_DECL;
};

class MyJson {
public:
  MyJson();
  ~MyJson();

  inline MyBuffer* JsonStr() { return &m_Raw; }
  inline MyJsonValue* Root() { return &m_Root; }

  void Reset();
  int Parse();

  MyJsonValue* Find(const char* path);

  int InitRootNode(MyJsonValueType type) {
    Reset();
    if (type == MyJsonValueType::arrayValue || type == MyJsonValueType::objectValue) {
      m_Root.SetValueType(type);
      return 0;
    } else {
      return LastError(MY_ERR_ENCODING_JSON, "Root can only be Array or Object");
    }
  }

  int ToString(MyStringA* retJson, bool compact=false) {
    int err = m_Root.ToString(retJson, compact, 0);
    if (err != 0) 
      return LastError(err, m_Root.LastErrorMessage()->Deref());
    return 0;
  }

private:

  inline bool IsWhitespace(char c) {
    return c == ' ' || c == '\r' || c == '\n' || c == '\t';
  }

  inline int SkipWhitespace(char* ptr, int startIndex) {
    int index = startIndex;
    while (ptr[index] != 0 && IsWhitespace(ptr[index])) {
      index++;
    }
    return index;
  }

  int ParseObject(char* ptr, int* index, MyJsonObject* obj);
  int ParseArray(char* ptr, int* index, MyJsonArray* arr);
  int ParseString(char* ptr, int* index, MyBuffer* val);
  int ParseValue(char* ptr, int* index, MyJsonValue* val);

  int SetJsonParseError(int index, const char* errMsg) {
    m_LastErrorCode = MY_ERR_ENCODING_JSON;
    m_LastErrorMessage.Set(errMsg);
    if (index >= 0) {
      m_LastErrorMessage.Append("(Index=");
      m_LastErrorMessage.AppendInt(index);
      m_LastErrorMessage.Append(")");
    }
    return m_LastErrorCode;
  }

private:
  MyBuffer    m_Raw;
  MyJsonValue m_Root;

  int         m_ParseState;
  MY_LAST_ERROR_DECL;
};

#endif // _MY_JSON_H_