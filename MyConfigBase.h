#ifndef MY_CONFIG_BASE_H_
#define MY_CONFIG_BASE_H_

#include "MyStringW.h"
#include "MyJSON.h"

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

    int ParseStringChild(MyJsonValue* jsonVal, MyStringA* val);
    int ParseStringChild(MyJsonValue* jsonVal, MyStringW* val);

protected:

    MyStringW m_ConfigFilePath;
    MyJson    m_Json;

    MY_LAST_ERROR_DECL;
};

#endif // MY_CONFIG_BASE_H_