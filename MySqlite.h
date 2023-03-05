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

    bool TableExist(const char* tableName);
    int CreateTable(MySqlEntityBase* ent, bool dropIfExists);
    int DeleteTable(const char* tableName);
    int Insert(MySqlEntityBase* ent);
    int Update(MySqlEntityBase* ent);
    int Delete(MySqlEntityBase* ent);
    template <class T>
    int Query(const char* tableName, MyArray<T>* retEntities);

    static int EscapeText(const char* input, MyStringA* ret);

private:

    int ParseEntity(sqlite3_stmt* stmt, MySqlEntityBase* curEntity);
    int Execute(const char* sql);
    int HandleSqliteError(int err);

private:
    MyStringA m_DBPath;

    sqlite3* m_Handle;

    MY_LAST_ERROR_DECL;
};

template <class T>
int MySqlite::Query(const char* tableName, MyArray<T>* retEntities) {
    int err = 0;
    int stepRet = 0;
    sqlite3_stmt* stmt = NULL;
    MyStringA sql;
    MySqlEntityBase* curEntity;

    retEntities->Reset();

    sql.SetWithFormat("SELECT * FROM %s;", tableName);
    err = sqlite3_prepare_v2(m_Handle, sql.Deref(), sql.Length(), &stmt, NULL);
    if (err) {
        HandleSqliteError(err);
        goto done;
    }

    while (stepRet != SQLITE_DONE) {
        stepRet = sqlite3_step(stmt);

        switch (stepRet) {
        case SQLITE_ROW:
            curEntity = (MySqlEntityBase*) retEntities->AddNew();
            if (err = ParseEntity(stmt, curEntity)) goto done;
            curEntity->SetTableName(tableName);
            break;

        case SQLITE_DONE:
            break;

        default:
            err = HandleSqliteError(stepRet);
            goto done;
        }
    }

done:
    if (stmt) {
        sqlite3_finalize(stmt);
    }

    return err;
}


#endif // _MY_SQLITE_H_
