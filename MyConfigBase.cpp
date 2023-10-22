
#ifndef _MY_CONFIG_BASE_CPP_
#define _MY_CONFIG_BASE_CPP_

#include "MyConfigBase.h"
#include "MyFile.h"

int MyConfigBase::Save() {
    int err = 0;
    MyStringA jsonStr;
    MyFile f;

    if (err = m_Json.ToString(&jsonStr)) return LastError(err, m_Json.LastErrorMessage());

    err = f.Open(
        m_ConfigFilePath.Deref(), 
        MY_FILE_CREATION_DISP_CREATE_ALWAYS, 
        MY_FILE_DESIRED_ACCESS_ALL, 
        MY_FILE_SHARE_MODE_SHARE_ALL
    );
    if (err) return LastError(err, f.LastErrorMessage());

    err = f.Write(jsonStr.Deref(), jsonStr.Length());
    f.Close();
    if (err) return LastError(err, f.LastErrorMessage());

    return 0;
}
int MyConfigBase::Load() {

    int err = 0;
    MyFile f;
    MyBuffer* data = m_Json.JsonStr();
    data->Reset();

    Reset();

    if (err = f.ReadAllBytes(m_ConfigFilePath.Deref(), data)) return LastError(err, f.LastErrorMessage());
    if (err = m_Json.Parse()) return LastError(err, m_Json.LastErrorMessage());

    return 0;
}

int MyConfigBase::CopyFrom(MyConfigBase* cfg) {
    m_ConfigFilePath.Set(cfg->m_ConfigFilePath.Deref(), cfg->m_ConfigFilePath.Length());
    return 0;
}

void MyConfigBase::Reset() {
    // Config File Path is not reset here
    m_Json.Reset();
}

int MyConfigBase::AddStringChild(MyJsonValue* jsonVal, const char* key, MyStringA* val) {
    int err = 0;
    MyStringA* retStr = jsonVal->AddStringChild(key, val->Deref());
    if (retStr == NULL) return LastError(jsonVal->LastErrorCode(), jsonVal->LastErrorMessage());
    return 0;
}
int MyConfigBase::AddStringChild(MyJsonValue* jsonVal, const char* key, MyStringW* val) {
    int err = 0;
    MyStringA* retStr = jsonVal->AddStringChild(key, NULL);
    if (retStr == NULL) return LastError(jsonVal->LastErrorCode(), jsonVal->LastErrorMessage());
    if (err = retStr->SetUnicode(val->Deref(), val->Length())) return LastError(err, "Failed to add string child");
    return 0;
}
int MyConfigBase::AddStringChildEncrypted(MyJsonValue* jsonVal, const char* key, MyStringA* val, const char* encKey, const char* encIV) {
    int err = 0;
    MyStringA encrypted;
    MyAes aes;
    if (err = aes.Init(MY_AES_128_CBC, encKey, encIV, true, MY_PADDING_MODE_PKCS7)) return LastError(err, aes.LastErrorMessage());
    if (err = aes.Update(val->Deref(), val->Length())) return LastError(err, aes.LastErrorMessage());
    if (err = aes.Final()) return LastError(err, aes.LastErrorMessage());
    if (err = MyEncodings::HexEncode(aes.Result()->Deref(), aes.Result()->Length(), &encrypted)) return err;

    return AddStringChild(jsonVal, key, &encrypted);
}
int MyConfigBase::AddBoolChild(MyJsonValue* jsonVal, const char* key, bool val) {
    int err = 0;
    if (err = jsonVal->AddBoolChild(key, val)) return LastError(err, jsonVal->LastErrorMessage());
    return 0;
}
int MyConfigBase::AddIntChild(MyJsonValue* jsonVal, const char* key, int val) {
    int err = 0;
    MyStringA* retStr = jsonVal->AddNumberChild(key, val);
    if (retStr == NULL) return LastError(jsonVal->LastErrorCode(), jsonVal->LastErrorMessage());
    return 0;
}

int MyConfigBase::ParseStringChild(MyJsonValue* jsonVal, MyStringA* val) {
    int err = 0;

    if (jsonVal->ValueType() != MyJsonValueType::stringValue) {
        m_LastErrorMessage.SetWithFormat("\"%s\" is not a string", jsonVal->Key()->Deref());
        return MY_ERR_ENCODING_JSON;
    }

    val->Set(jsonVal->StringValue());
    return 0;
}
int MyConfigBase::ParseStringChild(MyJsonValue* jsonVal, MyStringW* val) {
    int err = 0;

    if (jsonVal->ValueType() != MyJsonValueType::stringValue) {
        m_LastErrorMessage.SetWithFormat("\"%s\" is not a string", jsonVal->Key()->Deref());
        return MY_ERR_ENCODING_JSON;
    }

    val->SetUtf8(jsonVal->StringValue()->Deref(), jsonVal->StringValue()->Length());
    return 0;
}
int MyConfigBase::ParseStringChildDecrypted(MyJsonValue* jsonVal, MyStringA* val, const char* decKey, const char* decIV) {
    int err = 0;
    MyBuffer todec;
    MyAes aes;

    if (err = ParseStringChild(jsonVal, val)) return err;
    if (val->Length() == 0) return 0;

    if (err = MyEncodings::HexDecode(val->Deref(), val->Length(), &todec)) return err;
    if (err = aes.Init(MY_AES_128_CBC, decKey, decIV, false, MY_PADDING_MODE_PKCS7)) return LastError(err, aes.LastErrorMessage());
    if (err = aes.Update(todec.Deref(), todec.Length())) return LastError(err, aes.LastErrorMessage());
    if (err = aes.Final()) return LastError(err, aes.LastErrorMessage());

    if (err = val->Set(aes.Result()->Deref(), aes.Result()->Length())) return err;
    return 0;
}
int MyConfigBase::ParseBoolChild(MyJsonValue* jsonVal, bool* val) {
    int err = 0;

    if (jsonVal->ValueType() != MyJsonValueType::boolValue) {
        m_LastErrorMessage.SetWithFormat("\"%s\" is not a boolean", jsonVal->Key()->Deref());
        return MY_ERR_ENCODING_JSON;
    }

    *val = jsonVal->BoolValue();
    return 0;
}
int MyConfigBase::ParseIntChild(MyJsonValue* jsonVal, int* val) {
    int err = 0;

    if (jsonVal->ValueType() != MyJsonValueType::numberValue) {
        m_LastErrorMessage.SetWithFormat("\"%s\" is not a integer", jsonVal->Key()->Deref());
        return MY_ERR_ENCODING_JSON;
    }

    MyStringA* retStr = jsonVal->NumberValue();
    *val = retStr->DerefAsInt();
    return 0;
}
int MyConfigBase::UnknownKeyVal(MyJsonValue* jsonVal) {
    MyStringA errMsg;
    errMsg.SetWithFormat("Unknown object key: \"%s\"", jsonVal->Key()->Deref());
    return LastError(MY_ERR_ENCODING_JSON, errMsg.Deref());
}

#endif // _MY_CONFIG_BASE_CPP_