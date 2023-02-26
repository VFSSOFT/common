#ifndef _MY_JSON_CPP_
#define _MY_JSON_CPP_

#include "MyJSON.h"

const char* MY_JSON_INDENTS[] {
  "",
  " ",
  "  ",
  "   ",
  "    ",
  "     ",
  "      ",
  "       ",
  "        ",
  "         ",
  "          "
};

void MyJsonValue::SetValueType(MyJsonValueType t) {
  ResetValue();
  switch (t) {
  case MyJsonValueType::stringValue:
    m_StringValue = new MyStringA();
    break;
  case MyJsonValueType::numberValue:
    m_NumberValue = new MyStringA();
    break;
  case MyJsonValueType::objectValue:
    m_ObjectValue = new MyJsonObject();
    break;
  case MyJsonValueType::arrayValue:
    m_ArrayValue = new MyJsonArray();
    break;
  case MyJsonValueType::boolValue:
    break;
  case MyJsonValueType::nullValue:
    break;
  }
  m_ValueType = t;
}

int MyJsonValue::ChildrenCount() {
    MyJsonValueType t = ValueType();
    if (t == MyJsonValueType::objectValue) {
        return m_ObjectValue->Values.Size();
    } else if (t == MyJsonValueType::arrayValue) {
        return m_ArrayValue->Values.Size();
    } else {
        return -1;
    }
}
MyJsonValue* MyJsonValue::ChildAt(int index) {
    MyJsonValueType t = ValueType();
    if (t == MyJsonValueType::objectValue) {
        return m_ObjectValue->Values.Get(index);
    } else if (t == MyJsonValueType::arrayValue) {
        return m_ArrayValue->Values.Get(index);
    } else {
        return NULL;
    }
}

MyJsonValue* MyJsonValue::FindChildByName(const char* nodeName) {
  if (STR_IS_EMPTY(nodeName)) return NULL;

  MyJsonValueType t = ValueType();
  if (t == MyJsonValueType::objectValue) {
    for (int i = 0; i < m_ObjectValue->Values.Size(); i++) {
      MyJsonValue* v = m_ObjectValue->Values.Get(i);
      if (STR_EQUALS(nodeName, v->Key()->Deref())) {
        return v;
      }
    }
  }
  return NULL;
}

MyJsonValue* MyJsonValue::AddObjectChild(const char* name) {
  MyJsonValue* val = NULL;
  MyJsonValueType t = this->ValueType();
  if (t == MyJsonValueType::objectValue) {
    val = this->ObjectValue()->Values.AddNew();
    val->InitKey()->Set(name);
  } else if (t == MyJsonValueType::arrayValue) {
    if (!STR_IS_EMPTY(name)) {
      LastError(MY_ERR_ENCODING_JSON, "Name must be empty when adding children to the Array node");
      return NULL;
    }
    val = this->ArrayValue()->Values.AddNew();
  } else {
    LastError(MY_ERR_ENCODING_JSON, "Cannot add Object child node to the current node");
    return NULL;
  }
  val->SetValueType(MyJsonValueType::objectValue);
  return val;
}
MyJsonValue* MyJsonValue::AddArrayChild(const char* name) {
  MyJsonValue* val = NULL;
  MyJsonValueType t = this->ValueType();
  if (t == MyJsonValueType::objectValue) {
    val = this->ObjectValue()->Values.AddNew();
    val->InitKey()->Set(name);
  } else if (t == MyJsonValueType::arrayValue) {
    if (!STR_IS_EMPTY(name)) {
      LastError(MY_ERR_ENCODING_JSON, "Name must be empty when adding children to the Array node");
      return NULL;
    }
    val = this->ArrayValue()->Values.AddNew();
  } else {
    LastError(MY_ERR_ENCODING_JSON, "Cannot add Array child node to the current node");
    return NULL;
  }
  val->SetValueType(MyJsonValueType::arrayValue);
  return val;
}
MyStringA* MyJsonValue::AddStringChild(const char* name, const char* value) {
  MyJsonValue* val = NULL;
  MyJsonValueType t = this->ValueType();
  if (t == MyJsonValueType::objectValue) {
    val = this->ObjectValue()->Values.AddNew();
    val->InitKey()->Set(name);
  } else if (t == MyJsonValueType::arrayValue) {
    if (!STR_IS_EMPTY(name)) {
      LastError(MY_ERR_ENCODING_JSON, "Name must be empty when adding children to the Array node");
      return NULL;
    }
    val = this->ArrayValue()->Values.AddNew();
  } else {
    LastError(MY_ERR_ENCODING_JSON, "Cannot add String child node to the current node");
    return NULL;
  }
  val->SetValueType(MyJsonValueType::stringValue);
  val->StringValue()->Set(value);
  return val->StringValue();
}
MyStringA* MyJsonValue::AddNumberChild(const char* name, INT64 val) {
    MyStringA numStr;
    if (numStr.SetInt64(val) != 0) {
        LastError(MY_ERR_ENCODING_JSON, "Failed to encode integer");
        return NULL;
    }
    return AddNumberChild(name, numStr.Deref());
}
MyStringA* MyJsonValue::AddNumberChild(const char* name, const char* num) {
  MyJsonValue* val = NULL;
  MyJsonValueType t = this->ValueType();
  if (t == MyJsonValueType::objectValue) {
    val = this->ObjectValue()->Values.AddNew();
    val->InitKey()->Set(name);
  } else if (t == MyJsonValueType::arrayValue) {
    if (!STR_IS_EMPTY(name)) {
      LastError(MY_ERR_ENCODING_JSON, "Name must be empty when adding children to the Array node");
      return NULL;
    }
    val = this->ArrayValue()->Values.AddNew();
  } else {
    LastError(MY_ERR_ENCODING_JSON, "Cannot add Number child node to the current node");
    return NULL;
  }
  val->SetValueType(MyJsonValueType::numberValue);
  val->NumberValue()->Set(num);
  return val->NumberValue();
}
int MyJsonValue::AddBoolChild(const char* name, bool value) {
  MyJsonValue* val = NULL;
  MyJsonValueType t = this->ValueType();
  if (t == MyJsonValueType::objectValue) {
    val = this->ObjectValue()->Values.AddNew();
    val->InitKey()->Set(name);
  } else if (t == MyJsonValueType::arrayValue) {
    if (!STR_IS_EMPTY(name)) {
      LastError(MY_ERR_ENCODING_JSON, "Name must be empty when adding children to the Array node");
      return NULL;
    }
    val = this->ArrayValue()->Values.AddNew();
  } else {
    LastError(MY_ERR_ENCODING_JSON, "Cannot add Bool child node to the current node");
    return NULL;
  }
  val->SetValueType(MyJsonValueType::boolValue);
  if (value) val->MarkAsTrue();
  else       val->MarkAsFalse();
  return 0;
}
int MyJsonValue::AddNullChild(const char* name) {
  MyJsonValue* val = NULL;
  MyJsonValueType t = this->ValueType();
  if (t == MyJsonValueType::objectValue) {
    val = this->ObjectValue()->Values.AddNew();
    val->InitKey()->Set(name);
  } else if (t == MyJsonValueType::arrayValue) {
    if (!STR_IS_EMPTY(name)) {
      LastError(MY_ERR_ENCODING_JSON, "Name must be empty when adding children to the Array node");
      return NULL;
    }
    val = this->ArrayValue()->Values.AddNew();
  } else {
    LastError(MY_ERR_ENCODING_JSON, "Cannot add Null child node to the current node");
    return NULL;
  }
  val->SetValueType(MyJsonValueType::nullValue);
  return 0;
}

int MyJsonValue::ToString(MyStringA* retJson, bool compact, int indent) {
  int err = 0;

  switch (m_ValueType) {
  case MyJsonValueType::stringValue:
    if (err = EncodeJsonString(retJson, StringValue())) return err;
    break;
  case MyJsonValueType::numberValue:
    retJson->Append(NumberValue()->Deref());
    break;
  case MyJsonValueType::objectValue:
    if (err = EncodeJsonObject(retJson, ObjectValue(), compact, indent)) return err;
    break;
  case MyJsonValueType::arrayValue:
    if (err = EncodeJsonArray(retJson, ArrayValue(), compact, indent)) return err;
    break;
  case MyJsonValueType::boolValue:
    retJson->Append(BoolValue() ? "true" : "false");
    break;
  case MyJsonValueType::nullValue:
    retJson->Append("null");
    break;
  }
  return 0;
}
int MyJsonValue::EncodeJsonString(MyStringA* ret, MyStringA* str) {
  char* ptr = str->Deref();
  int len = str->Length();
  int index = 0;

  ret->AppendChar('\"'); // start quote
  while (index < len) {
    char c = ptr[index];
    if (c == '\"') {
      ret->Append("\\\"");
    } else if (c == '\\') {
      ret->Append("\\\\");
    } else if (c == '/') {
      ret->Append("\\/");
    } else if (c == '\b') {
      ret->Append("\\b");
    } else if (c == '\f') {
      ret->Append("\\f");
    } else if (c == '\n') {
      ret->Append("\\n");
    } else if (c == '\r') {
      ret->Append("\\r");
    } else if (c == '\t') {
      ret->Append("\\t");
    } else {
      ret->AppendChar(c);
    }
    index++;
  }
  ret->AppendChar('\"'); // end quote
  return 0;
}
int MyJsonValue::EncodeJsonObject(MyStringA* ret, MyJsonObject* obj, bool compact, int indent) {
  int err = 0;
  ret->AppendChar('{');
  if (!compact) {
    ret->Append("\r\n");
  }

  int childCount = obj->Values.Size();
  for (int i = 0; i < childCount; i++) {
    if (!compact) AddIndent(ret, indent + 2);
    MyJsonValue* child = obj->Values.Get(i);
    if (err = EncodeJsonString(ret, child->Key())) return err;
    ret->AppendChar(':');
    if (err = child->ToString(ret, compact, indent + 2)) {
      return LastError(err, child->LastErrorMessage()->Deref());
    }
    if (compact) {
      if (i < childCount - 1) ret->Append(",");
    } else {
      ret->Append(i < childCount - 1 ? ",\r\n" : "\r\n");
    }
  }

  if (!compact) AddIndent(ret, indent);
  ret->AppendChar('}');
  return 0;
}
int MyJsonValue::EncodeJsonArray(MyStringA* ret, MyJsonArray* arr, bool compact, int indent) {
  int err = 0;
  ret->AppendChar('[');
  if (!compact) {
    ret->Append("\r\n");
  }

  int childCount = arr->Values.Size();
  for (int i = 0; i < childCount; i++) {
    if (!compact) AddIndent(ret, indent + 2);
    MyJsonValue* child = arr->Values.Get(i);
    if (err = child->ToString(ret, compact, indent + 2)) {
      return LastError(err, child->LastErrorMessage()->Deref());
    }
    if (compact) {
      if (i < childCount - 1) ret->Append(",");
    } else {
      ret->Append(i < childCount - 1 ? ",\r\n" : "\r\n");
    }
  }
  if (!compact) AddIndent(ret, indent);
  ret->AppendChar(']');
  return 0;
}
int MyJsonValue::AddIndent(MyStringA* ret, int indent) {
  if (indent == 0) return 0;
  
  if (indent <= 10)
    return ret->Append(MY_JSON_INDENTS[indent]);

  for (int i = 0; i < indent; i++)
    ret->AppendChar(' ');

  return 0;
}

MyJson::MyJson() {
  Reset();
}
MyJson::~MyJson() {

}

void MyJson::Reset() {
  m_LastErrorCode = 0;
  m_Raw.Reset();
}

int MyJson::Parse() {
  char* ptr = m_Raw.Deref();
  int len = m_Raw.Length();
  int i = 0;

  int err = ParseValue(ptr, &i, &m_Root);
  if (err) return err;

  i = SkipWhitespace(ptr, i);

  if (i != len) {
    return LastError(MY_ERR_ENCODING_JSON, "Extra data after ending of the JSON string");
  }
  return 0;
}

MyJsonValue* MyJson::Find(const char* path) {
  MyJsonValue* ret = &m_Root;
  if (STR_IS_EMPTY(path)) return ret;

  int len = strlen(path);
  int startIndex = 0;
  int endIndex = 0;
  char nameBuf[1024] = { 0 };
  
  while (endIndex <= len) {
    if (endIndex == len || path[endIndex] == '/') {
      int nameLen = endIndex - startIndex;
      assert(nameLen < 1024); // ensure the buffer has enough room for the name
      strncpy(nameBuf, path + startIndex, nameLen);
      nameBuf[nameLen] = 0;
      
      ret = ret->FindChildByName(nameBuf);
      if (ret == NULL) return NULL;

      startIndex = endIndex + 1;
    }
    endIndex++;
  }

  return ret;
}

int MyJson::ParseObject(char* ptr, int* index, MyJsonObject* obj) {
  int err = 0;
  int i = *index;
  i = SkipWhitespace(ptr, i);

  char c = ptr[i];
  i++;
  if (c != '{')
    return SetJsonParseError(i, "Object should starts with '{'");
  
  // parse the Object body
  while (true) {
    i = SkipWhitespace(ptr, i);

    c = ptr[i];

    if (c == '}') {
      i++;
      break;
    }

    if (c == 0)
      return SetJsonParseError(i, "Object should ends with }");

    MyJsonValue* item = obj->Values.AddNew();
    item->InitKey();
    if (err = ParseString(ptr, &i, item->Key())) return err;

    i = SkipWhitespace(ptr, i);

    c = ptr[i];
    i++;
    if (c != ':') 
      return SetJsonParseError(i, "Colon is expected after Object key");

    if (err = ParseValue(ptr, &i, item)) return err;
    
    i = SkipWhitespace(ptr, i);

    c = ptr[i];
    if (c == ',') { // consume the possible comma
      i++;
    }

  }

  *index = i;
  return 0;

}

int MyJson::ParseArray(char* ptr, int* index, MyJsonArray* arr) {
  int err = 0;
  int i = *index;
  i = SkipWhitespace(ptr, i);

  char c = ptr[i];
  i++;
  if (c != '[')
    return SetJsonParseError(i, "Array should starts with '['");
  
  // parse the Array body
  while (true) {
    i = SkipWhitespace(ptr, i);

    c = ptr[i];

    if (c == ']') {
      i++;
      break;
    }

    if (c == 0)
      return SetJsonParseError(i, "Array should ends with ]");

    MyJsonValue* item = arr->Values.AddNew();
    item->InitKey();
    if (err = ParseValue(ptr, &i, item)) return err;
    
    i = SkipWhitespace(ptr, i);

    c = ptr[i];
    if (c == ',') { // consume the possible comma
      i++;
    }

  }

  *index = i;
  return 0;

}

int MyJson::ParseValue(char* ptr, int* index, MyJsonValue* val) {
  int err = 0;
  char nextC = 0;
  MyStringA* numVal = NULL;

  int i = *index;
  i = SkipWhitespace(ptr, i);

  char c = ptr[i];
  switch (c) {
  case '\"':
    // ++i; // we will consume the " in ParseString
    val->SetValueType(MyJsonValueType::stringValue);
    if (err = ParseString(ptr, &i, val->StringValue())) return err;
    break;

  case '{':
    // ++i; // we will consume the { in ParseObject
    val->SetValueType(MyJsonValueType::objectValue);
    if (err = ParseObject(ptr, &i, val->ObjectValue())) return err;
    break;

  case '[':
    // ++i; // we will consume the [ in ParseArray
    val->SetValueType(MyJsonValueType::arrayValue);
    if (err = ParseArray(ptr, &i, val->ArrayValue())) return err;
    break;

  default:
    // true, false, null, number, or invalid input JSON str
    
    if (c == 't' || c == 'T') {
      nextC = ptr[++i];
      if (nextC != 'r' && nextC != 'R')
        return SetJsonParseError(i, "Invalid character");

      nextC = ptr[++i];
      if (nextC != 'u' && nextC != 'U')
        return SetJsonParseError(i, "Invalid character");

      nextC = ptr[++i];
      if (nextC != 'e' && nextC != 'E')
        return SetJsonParseError(i, "Invalid character");
      
      val->SetValueType(MyJsonValueType::boolValue);
      val->MarkAsTrue();
      i++;

    } else if (c == 'f' || c == 'F') {
      nextC = ptr[++i];
      if (nextC != 'a' && nextC != 'A')
        return SetJsonParseError(i, "Invalid character");

      nextC = ptr[++i];
      if (nextC != 'l' && nextC != 'L')
        return SetJsonParseError(i, "Invalid character");

      nextC = ptr[++i];
      if (nextC != 's' && nextC != 'S')
        return SetJsonParseError(i, "Invalid character");

      nextC = ptr[++i];
      if (nextC != 'e' && nextC != 'E')
        return SetJsonParseError(i, "Invalid character");
      
      val->SetValueType(MyJsonValueType::boolValue);
      val->MarkAsFalse();
      ++i;

    } else if (c == 'n' || c == 'N') {
      nextC = ptr[++i];
      if (nextC != 'u' && nextC != 'U')
        return SetJsonParseError(i, "Invalid character");

      nextC = ptr[++i];
      if (nextC != 'l' && nextC != 'L')
        return SetJsonParseError(i, "Invalid character");

      nextC = ptr[++i];
      if (nextC != 'l' && nextC != 'L')
        return SetJsonParseError(i, "Invalid character");

      val->SetValueType(MyJsonValueType::nullValue);
      ++i;

    } else {

      // number or invalid input
      if (c == '-' || (c >= '0' || c <= '9')) {
        val->SetValueType(MyJsonValueType::numberValue);
        numVal = val->NumberValue();
        numVal->AppendChar(c);
        while ((nextC = ptr[++i]) != 0) {
          if ((nextC >= '0' && nextC <= '9') || nextC == 'e' || nextC == 'E' || nextC == '.') {
            numVal->AppendChar(nextC);
          } else if (IsWhitespace(nextC) || nextC == ',' || nextC == '}' || nextC == ']') {
            break;
          } else {
            return SetJsonParseError(i, "Invalid character in number");
          }
        }
        // no need to increment the i, i points to the end of the input string or ','
      } else {
        return SetJsonParseError(i, "Invalid character");
      }
    }

    break;
  }
  *index = i;
  return 0;
}

int MyJson::ParseString(char* ptr, int* index, MyBuffer* val) {
  int i = *index;
  i = SkipWhitespace(ptr, i);

  char c = ptr[i];
  if (c != '\"')
    return SetJsonParseError(i, "String should starts with '\"'");

  i++;
  while ((c = ptr[i]) != 0) {
    
    if (c == '\\') {
      i++;
      c = ptr[i];

      switch (c) {
      case '\"': 
        val->AppendChar('\"');
        break;
      case '\\':
        val->AppendChar('\\');
        break;
      case '/':
        val->AppendChar('/');
        break;
      case 'b':
        val->AppendChar('\b');
        break;
      case 'f':
        val->AppendChar('\f');
        break;
      case 'n':
        val->AppendChar('\n');
        break;
      case 'r':
        val->AppendChar('\r');
        break;
      case 't':
        val->AppendChar('\t');
        break;
      case 'u':
        return SetJsonParseError(i, "'\\u' escape sequence is not supported");
      default:
        return SetJsonParseError(i, "Unknown escape sequence");
      }

    } else if (c == '\"') {
      break; // end of string
    } else {
      val->AppendChar(c);
    }
    i++;
  }
  if (ptr[i] != '\"')
    return SetJsonParseError(i, "String should starts with '\"'");
  
  *index = i + 1;
  return 0;
}

#endif // _MY_JSON_CPP_
