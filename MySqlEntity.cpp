#ifndef _MY_SQL_ENTITY_CPP_
#define _MY_SQL_ENTITY_CPP_

#include "MySqlEntity.h"

const char* SQL_DATA_TYPE_STR_NAMES[] = { "NULL", "INTEGER", "REAL", "TEXT", "BLOB" };

void MySqlValue::CopyFrom(MySqlValue* value) {
    m_Type = value->m_Type;
    m_IntegerValue = value->m_IntegerValue;
    m_RealValue = value->m_RealValue;
    m_TextValue.Set(value->m_TextValue.Deref(), value->m_TextValue.Length());
    m_BlobValue.Set(value->m_BlobValue.Deref(), value->m_BlobValue.Length());
}

const char* MySqlEntityField::SqlTypeName() {
    return SQL_DATA_TYPE_STR_NAMES[(int)m_Type];
}

void MySqlEntityField::CopyFrom(MySqlEntityField* field) {
    m_Type = field->m_Type;
    m_SqlName.Set(field->m_SqlName.Deref(), field->m_SqlName.Length());
    m_PropName.Set(field->m_PropName.Deref(), field->m_PropName.Length());
    m_Value.CopyFrom(&field->m_Value);
}


MySqlEntityField* MySqlEntityBase::GetFieldBySqlName(const char* sqlName) {
    for (int i = 0; i < m_Fields.Size(); i++) {
        MySqlEntityField* f = m_Fields.Get(i);

        if (STR_EQUALS(f->SqlName(), sqlName)) {
            return f;
        }
    }
    return NULL;
}

const char* MySqlEntityBase::GetText(const char* sqlName) {
    MySqlEntityField* f = GetFieldBySqlName(sqlName);
    return f->TextValue()->Deref();
}
void MySqlEntityBase::SetText(const char* sqlName, const char* val, int valLen) {
    MySqlEntityField* f = GetFieldBySqlName(sqlName);
    f->TextValue()->Set(val, valLen);
}
INT64 MySqlEntityBase::GetInt(const char* sqlName) {
    MySqlEntityField* f = GetFieldBySqlName(sqlName);
    return f->IntegerValue();
}
void MySqlEntityBase::SetInt(const char* sqlName, INT64 val) {
    MySqlEntityField* f = GetFieldBySqlName(sqlName);
    f->SetIntegerValue(val);
}

void MySqlEntityBase::CopyFrom(MySqlEntityBase* entity) {
    this->m_TableName = entity->m_TableName;
    this->m_ID = entity->m_ID;
    
    for (int i = 0; i < entity->m_Fields.Size(); i++) {
        MySqlEntityField* otherField = entity->m_Fields.Get(i);
        MySqlEntityField* curField = m_Fields.Get(i);
        curField->CopyFrom(otherField);
    }
}


#endif // _MY_SQL_ENTITY_CPP_
