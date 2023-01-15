#ifndef _MY_SQL_ENTITY_CPP_
#define _MY_SQL_ENTITY_CPP_

#include "MySqlEntity.h"

const char* SQL_DATA_TYPE_STR_NAMES[] = { "NULL", "INTEGER", "REAL", "TEXT", "BLOB" };

const char* MySqlEntityField::SqlTypeName() {
    return SQL_DATA_TYPE_STR_NAMES[(int)m_Type];
}


#endif // _MY_SQL_ENTITY_CPP_
