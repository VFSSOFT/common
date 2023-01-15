#ifndef _MY_SQLITE_CPP_
#define _MY_SQLITE_CPP_


#include "MySqlite.h"


MySqlite::MySqlite(const char* dbPath) {
    INIT_LAST_ERROR;
    m_Handle = NULL;
    m_DBPath.Set(dbPath);
}
MySqlite::~MySqlite() {
    Close();
}

int MySqlite::Open() {
    int err = 0;

    err = sqlite3_open_v2(
        m_DBPath.Deref(),
        &m_Handle,
        SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE,
        NULL
    );
    if (err) return HandleSqliteError(err);

    return 0;
}
int MySqlite::Close() {
    sqlite3_close_v2(m_Handle);
    m_Handle = NULL;
    return 0;
}

int MySqlite::CreateTable(MySqlEntityBase* ent, bool dropIfExists) {
    MyStringA sql;
    MyArray<MySqlEntityField>* fields = ent->Fields();

    if (dropIfExists) {
        sql.SetWithFormat("DROP TABLE IF EXISTS %s;\n", ent->TableName());
    }

    sql.AppendWithFormat("CREATE TABLE %s (ID INTEGER PRIMARY KEY AUTOINCREMENT, ", ent->TableName());
    for (int i = 0; i < fields->Size(); i++) {
        MySqlEntityField* f = fields->Get(i);
        sql.AppendWithFormat("%s %s", f->SqlName(), f->SqlTypeName());

        if (i < fields->Size() - 1) {
            sql.Append(", ");
        }
    }
    sql.Append(");");

    return Execute(sql.Deref());
}
int MySqlite::DeleteTable(const char* tableName) {
    MyStringA sql;
    sql.SetWithFormat("DROP TABLE IF EXISTS %s;", tableName);
    return Execute(sql.Deref());
}
int MySqlite::Insert(MySqlEntityBase* ent) {
    int err = 0;
    sqlite3_stmt* stmt = NULL;
    MyStringA sql, namesStr, placeholdersStr, tmp;
    MySqlEntityField* f = NULL;
    MyArray<MySqlEntityField>* fields = ent->Fields();

    for (int i = 0; i < fields->Size(); i++) {
        f = fields->Get(i);

        namesStr.Append(f->SqlName());
        placeholdersStr.Append("?");

        if (i < fields->Size() - 1) {
            namesStr.Append(",");
            placeholdersStr.Append(",");
        }
    }

    sql.SetWithFormat("INSERT INTO %s (%s) VALUES (%s);", ent->TableName(), namesStr.Deref(), placeholdersStr.Deref());
    err = sqlite3_prepare_v2(m_Handle, sql.Deref(), sql.Length(), &stmt, NULL);
    if (err) {
        HandleSqliteError(err);
        goto done;
    }

    for (int i = 0; i < fields->Size(); i++) {
        f = fields->Get(i);

        switch (f->Type()) {
        case SqlDataType::tNull:
            if (err = sqlite3_bind_null(stmt, i + 1)) {
                HandleSqliteError(err);
                goto done;
            }
            break;

        case SqlDataType::tInteger:
            if (err = sqlite3_bind_int64(stmt, i + 1, f->IntegerValue())) {
                HandleSqliteError(err);
                goto done;
            }
            break;

        case SqlDataType::tReal:
            if (err = sqlite3_bind_double(stmt, i + 1, f->RealValue())) {
                HandleSqliteError(err);
                goto done;
            }
            break;

        case SqlDataType::tText:
            EscapeText(f->TextValue()->Deref(), &tmp);
            if (err = sqlite3_bind_text(stmt, i + 1, tmp.Deref(), tmp.Length(), NULL)) {
                HandleSqliteError(err);
                goto done;
            }
            break;

        case SqlDataType::tBlob:
            if (err = sqlite3_bind_blob(stmt, i + 1, tmp.Deref(), tmp.Length(), NULL)) {
                HandleSqliteError(err);
                goto done;
            }
            break;
        }
    }

    err = sqlite3_step(stmt);
    if (err != SQLITE_DONE) {
        HandleSqliteError(err);
        goto done;
    }
    err = 0;

    // TODO: query the ID

done:
    if (stmt) {
        sqlite3_finalize(stmt);
    }

    return err;
}
int MySqlite::Update(MySqlEntityBase* ent) {
    int err = 0;

    if (ent->ID() == 0) {
        return LastError(MY_ERR_INVALID_PARAMETERS, "Entity's ID is empty");
    }

    sqlite3_stmt* stmt = NULL;
    MyStringA sql, setStr, tmp;
    MySqlEntityField* f = NULL;
    MyArray<MySqlEntityField>* fields = ent->Fields();

    for (int i = 0; i < fields->Size(); i++) {
        f = fields->Get(i);

        setStr.AppendWithFormat("%s=?", f->SqlName());

        if (i < fields->Size() - 1) {
            setStr.Append(",");
        }
    }

    sql.SetWithFormat("UPDATE %s SET %s WHERE ID=%d;", ent->TableName(), setStr.Deref(), ent->ID());
    err = sqlite3_prepare_v2(m_Handle, sql.Deref(), sql.Length(), &stmt, NULL);
    if (err) {
        HandleSqliteError(err);
        goto done;
    }

    for (int i = 0; i < fields->Size(); i++) {
        f = fields->Get(i);

        switch (f->Type()) {
        case SqlDataType::tNull:
            if (err = sqlite3_bind_null(stmt, i + 1)) {
                HandleSqliteError(err);
                goto done;
            }
            break;

        case SqlDataType::tInteger:
            if (err = sqlite3_bind_int64(stmt, i + 1, f->IntegerValue())) {
                HandleSqliteError(err);
                goto done;
            }
            break;

        case SqlDataType::tReal:
            if (err = sqlite3_bind_double(stmt, i + 1, f->RealValue())) {
                HandleSqliteError(err);
                goto done;
            }
            break;

        case SqlDataType::tText:
            EscapeText(f->TextValue()->Deref(), &tmp);
            if (err = sqlite3_bind_text(stmt, i + 1, tmp.Deref(), tmp.Length(), NULL)) {
                HandleSqliteError(err);
                goto done;
            }
            break;

        case SqlDataType::tBlob:
            if (err = sqlite3_bind_blob(stmt, i + 1, tmp.Deref(), tmp.Length(), NULL)) {
                HandleSqliteError(err);
                goto done;
            }
            break;
        }
    }

    err = sqlite3_step(stmt);
    if (err != SQLITE_DONE) {
        HandleSqliteError(err);
        goto done;
    }

done:
    if (stmt) {
        sqlite3_finalize(stmt);
    }

    return err;
}

int MySqlite::Delete(MySqlEntityBase* ent) {
    int err = 0;

    if (ent->ID() == 0) {
        return LastError(MY_ERR_INVALID_PARAMETERS, "Entity's ID is empty");
    }

    sqlite3_stmt* stmt = NULL;
    MyStringA sql, setStr, tmp;

    sql.SetWithFormat("DELETE FROM %s WHERE ID=%d;", ent->TableName(), ent->ID());
    err = sqlite3_prepare_v2(m_Handle, sql.Deref(), sql.Length(), &stmt, NULL);
    if (err) {
        HandleSqliteError(err);
        goto done;
    }

    err = sqlite3_step(stmt);
    if (err != SQLITE_DONE) {
        HandleSqliteError(err);
        goto done;
    }

done:
    if (stmt) {
        sqlite3_finalize(stmt);
    }

    return err;
}

int MySqlite::ParseEntity(sqlite3_stmt* stmt, MySqlEntityBase* curEntity) {
    int err = 0;

    int colCount = sqlite3_column_count(stmt);
    for (int i = 0; i < colCount; i++) {
        const char* colName = sqlite3_column_name(stmt, i);

        if (STR_EQUALS(colName, "ID")) {
            curEntity->SetID(sqlite3_column_int64(stmt, i));
        } else {
            MySqlEntityField* curField = curEntity->GetFieldBySqlName(colName);

            switch (curField->Type()) {
            case SqlDataType::tNull:
                break;

            case SqlDataType::tInteger:
                curField->SetIntegerValue(sqlite3_column_int64(stmt, i));
                break;

            case SqlDataType::tReal:
                curField->SetRealValue(sqlite3_column_double(stmt, i));
                break;

            case SqlDataType::tText:
                curField->TextValue()->Set((const char*)sqlite3_column_text(stmt, i));
                break;

            case SqlDataType::tBlob:
                curField->BlobValue()->Set((const char*)sqlite3_column_blob(stmt, i), sqlite3_column_bytes(stmt, i));
                break;

            }
        }
    }

    return 0;
}
int MySqlite::Execute(const char* sql) {
    int err = 0;
    char* sqlErrMsg = NULL;

    err = sqlite3_exec(m_Handle, sql, NULL, NULL, &sqlErrMsg);
    if (err) {
        LastError(err, sqlErrMsg);
        sqlite3_free(sqlErrMsg);
        return err;
    }
    return 0;
}
int MySqlite::HandleSqliteError(int err) {
    const char* sqlErr = sqlite3_errmsg(m_Handle);
    return LastError(err, sqlErr);
}


int MySqlite::EscapeText(const char* input, MyStringA* ret) {
    // TODO:
    return ret->Set(input);
}

#endif // _MY_SQLITE_CPP_
