#ifndef _MY_SQLITE_H_
#define _MY_SQLITE_H_

#include "MyStringA.h"
#include "MySqlEntity.h"

#include <sqlite3.h>

class MySqlite {
public:
    MySqlite(const char* dbPath);
    virtual ~MySqlite();

    int Open();
    int Close();

    int CreateTable(MySqlEntityBase* ent, bool dropIfExists);
    int DeleteTable(const char* tableName);
    int Insert(MySqlEntityBase* ent);


    static int EscapeText(const char* input, MyStringA* ret);

private:
    int Execute(const char* sql);
    int HandleSqliteError(int err);

private:
    MyStringA m_DBPath;

    sqlite3* m_Handle;

    MY_LAST_ERROR_DECL;
};

#endif // _MY_SQLITE_H_
