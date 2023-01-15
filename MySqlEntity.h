#ifndef _MY_SQL_ENTITY_H_
#define _MY_SQL_ENTITY_H_

#include "MyArray.h"
#include "MyStringA.h"

enum class SqlDataType {
    tNull = 0,
    tInteger = 1,
    tReal = 2,
    tText = 3,
    tBlob = 4
};

class MySqlEntityField {
public:
    MySqlEntityField(): m_Type(SqlDataType::tNull), m_IntegerValue(0), m_RealValue(0.0) {}

    int Init(SqlDataType t, const char* propName, const char* sqlName) {
        m_Type = t;
        m_PropName.Set(propName);
        m_SqlName.Set(sqlName);
        return 0;
    }
    int Init(SqlDataType t, const char* propName, const char* sqlName, INT64 val) {
        this->Init(t, propName, sqlName);
        m_IntegerValue = val;
        return 0;
    }
    int Init(SqlDataType t, const char* propName, const char* sqlName, double val) {
        this->Init(t, propName, sqlName);
        m_RealValue = val;
        return 0;
    }
    int Init(SqlDataType t, const char* propName, const char* sqlName, const char* str) {
        this->Init(t, propName, sqlName);
        m_TextValue.Set(str);
        return 0;
    }
    int Init(SqlDataType t, const char* propName, const char* sqlName, const char* blob, int blobLen) {
        this->Init(t, propName, sqlName);
        m_BlobValue.Set(blob, blobLen);
        return 0;
    }

    const char* SqlTypeName();

    SqlDataType Type() { return m_Type; }
    const char* PropName() { return m_PropName.Deref(); }
    const char* SqlName() { return m_SqlName.Deref(); }

    INT64 IntegerValue() { return m_IntegerValue; }
    double RealValue() { return m_RealValue; }
    void SetIntegerValue(INT64 val) { m_IntegerValue = val; }
    void SetRealValue(double val) { m_RealValue = val; }
    MyStringA* TextValue() { return &m_TextValue; }
    MyBuffer* BlobValue() { return &m_BlobValue; }

private:
    SqlDataType m_Type;

    MyStringA   m_PropName;
    MyStringA   m_SqlName;

    // values
    INT64       m_IntegerValue;
    double      m_RealValue;
    MyStringA   m_TextValue;
    MyBuffer    m_BlobValue;

    // other limitation or relationship
};

class MySqlEntityBase {
public:
    MySqlEntityBase(): m_ID(0) {}
    MySqlEntityBase(const char* tableName): m_ID(0) {
        m_TableName.Set(tableName);
    }

    UINT64 ID() { return m_ID; }
    void SetID(UINT64 val) { m_ID = val; }

    const char* TableName() { return m_TableName.Deref(); }
    void SetTableName(const char* tableName) { m_TableName.Set(tableName); }
    MyArray<MySqlEntityField>* Fields() { return &m_Fields; }

    MySqlEntityField* GetFieldBySqlName(const char* sqlName);

protected:
    MyStringA m_TableName;
    UINT64    m_ID;
    MyArray<MySqlEntityField> m_Fields;
};

#endif // _MY_SQL_ENTITY_H_
