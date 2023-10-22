#ifndef MY_CONFIG_BASE_H_
#define MY_CONFIG_BASE_H_

#include "MyStringW.h"
#include "MyJSON.h"
#include "MyAes.h"
#include "MyEncodings.h"

class MyConfigBase {

public:
    MyConfigBase() {}
    virtual ~MyConfigBase() {}

    int SetConfigFilePath(MyStringW* configFilePath) { return m_ConfigFilePath.Set(configFilePath); }
    MyStringW* ConfigFilePath() { return &m_ConfigFilePath; }

    virtual int  Save();
    virtual int  Load();
    virtual int  Validate() { return 0; }
    virtual int  CopyFrom(MyConfigBase* cfg);
    virtual void ToString(MyStringA* str) { }
    virtual void Reset();

    int AddStringChild(MyJsonValue* jsonVal, const char* key, MyStringA* val);
    int AddStringChild(MyJsonValue* jsonVal, const char* key, MyStringW* val);
    int AddStringChildEncrypted(MyJsonValue* jsonVal, const char* key, MyStringA* val, const char* encKey, const char* encIV);
    int AddBoolChild(MyJsonValue* jsonVal, const char* key, bool val);
    int AddIntChild(MyJsonValue* jsonVal, const char* key, int val);

    int ParseStringChild(MyJsonValue* jsonVal, MyStringA* val);
    int ParseStringChild(MyJsonValue* jsonVal, MyStringW* val);
    int ParseStringChildDecrypted(MyJsonValue* jsonVal, MyStringA* val, const char* decKey, const char* decIV);
    int ParseBoolChild(MyJsonValue* jsonVal, bool* val);
    int ParseIntChild(MyJsonValue* jsonVal, int* val);

    int UnknownKeyVal(MyJsonValue* jsonVal);

protected:

    MyStringW m_ConfigFilePath;
    MyJson    m_Json;

    MY_LAST_ERROR_DECL;
};

#endif // MY_CONFIG_BASE_H_