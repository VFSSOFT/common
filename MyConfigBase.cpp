
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

#endif // _MY_CONFIG_BASE_CPP_