#include "pch.h"


#include "../MySqlite.h"

#define TEST_DB "test.db"

class StudentSql : public MySqlEntityBase {
public:
    StudentSql() : MySqlEntityBase("Student") {
        InitFields();
    }

    const char* Name() {
        MySqlEntityField* f = GetFieldBySqlName("Name");
        return f->TextValue()->Deref();
    }
    void SetName(const char* name) {
        MySqlEntityField* f = GetFieldBySqlName("Name");
        f->TextValue()->Set(name);
    }

    const char* Phone() {
        MySqlEntityField* f = GetFieldBySqlName("Phone");
        return f->TextValue()->Deref();
    }
    void SetPhone(const char* phone) {
        MySqlEntityField* f = GetFieldBySqlName("Phone");
        f->TextValue()->Set(phone);
    }

    int Age() {
        MySqlEntityField* f = GetFieldBySqlName("Age");
        return f->IntegerValue();
    }
    void SetAge(int val) {
        MySqlEntityField* f = GetFieldBySqlName("Age");
        f->SetIntegerValue(val);
    }

private:
    void InitFields() {
        MySqlEntityField* f = m_Fields.AddNew();
        f->Init(SqlDataType::tText, "Name", "Name");

        f = m_Fields.AddNew();
        f->Init(SqlDataType::tText, "Phone", "Phone");

        f = m_Fields.AddNew();
        f->Init(SqlDataType::tInteger, "Age", "Age");
    }
};

TEST(MySqliteTest, OpenCloseTest) {
    int err = 0;
    MySqlite sql(TEST_DB);

    err = sql.Open();
    EXPECT_EQ(err, 0);

    err = sql.Close();
    EXPECT_EQ(err, 0);
}

TEST(MySqliteTest, CreateTableTest) {
    int err = 0;
    MySqlite sql(TEST_DB);

    err = sql.Open();
    EXPECT_EQ(err, 0);

    StudentSql stu;
    err = sql.CreateTable(&stu, true);
    EXPECT_EQ(err, 0);

    sql.Close();
}

TEST(MySqliteTest, DeleteTableTest) {
    int err = 0;
    MySqlite sql(TEST_DB);

    err = sql.Open();
    EXPECT_EQ(err, 0);

    StudentSql stu;
    err = sql.CreateTable(&stu, true);
    EXPECT_EQ(err, 0);

    err = sql.DeleteTable(stu.TableName());
    EXPECT_EQ(err, 0);

    sql.Close();
}

TEST(MySqliteTest, InsertTest) {
    int err = 0;
    MySqlite sql(TEST_DB);

    err = sql.Open();
    EXPECT_EQ(err, 0);

    StudentSql stu;
    err = sql.CreateTable(&stu, true);
    EXPECT_EQ(err, 0);

    stu.SetName("Martin");
    stu.SetPhone("123456");
    stu.SetAge(88);

    err = sql.Insert(&stu);
    EXPECT_EQ(err, 0);
    EXPECT_TRUE(stu.ID() > 0);

    err = sql.DeleteTable(stu.TableName());
    EXPECT_EQ(err, 0);

    sql.Close();
}

TEST(MySqliteTest, QueryAllTest) {
    int err = 0;
    MySqlite sql(TEST_DB);

    err = sql.Open();
    EXPECT_EQ(err, 0);

    StudentSql stu;
    err = sql.CreateTable(&stu, true);
    EXPECT_EQ(err, 0);

    stu.SetName("Martin");
    stu.SetPhone("123456");
    stu.SetAge(88);

    err = sql.Insert(&stu);
    EXPECT_EQ(err, 0);

    MyArray<StudentSql> students;
    err = sql.Query<StudentSql>("Student", &students);
    EXPECT_EQ(err, 0);

    EXPECT_EQ(students.Size(), 1);
    EXPECT_STREQ(students.Get(0)->Name(), "Martin");
    EXPECT_STREQ(students.Get(0)->Phone(), "123456");
    EXPECT_EQ(students.Get(0)->Age(), 88);

    err = sql.DeleteTable(stu.TableName());
    EXPECT_EQ(err, 0);

    sql.Close();
}

TEST(MySqliteTest, UpdateTest) {
    int err = 0;
    MySqlite sql(TEST_DB);

    err = sql.Open();
    EXPECT_EQ(err, 0);

    StudentSql stu;
    err = sql.CreateTable(&stu, true);
    EXPECT_EQ(err, 0);

    stu.SetName("Martin");
    stu.SetPhone("123456");
    stu.SetAge(88);

    err = sql.Insert(&stu);
    EXPECT_EQ(err, 0);

    MyArray<StudentSql> students;
    err = sql.Query<StudentSql>("Student", &students);
    EXPECT_EQ(err, 0);

    EXPECT_EQ(students.Size(), 1);
    EXPECT_STREQ(students.Get(0)->Name(), "Martin");
    EXPECT_STREQ(students.Get(0)->Phone(), "123456");
    EXPECT_EQ(students.Get(0)->Age(), 88);

    stu.SetName("OldMartin");
    stu.SetAge(99);

    err = sql.Update(&stu);
    EXPECT_EQ(err, 0);

    students.Reset();
    err = sql.Query<StudentSql>("Student", &students);
    EXPECT_EQ(err, 0);

    EXPECT_EQ(students.Size(), 1);
    EXPECT_STREQ(students.Get(0)->Name(), "OldMartin");
    EXPECT_STREQ(students.Get(0)->Phone(), "123456");
    EXPECT_EQ(students.Get(0)->Age(), 99);


    err = sql.DeleteTable(stu.TableName());
    EXPECT_EQ(err, 0);

    sql.Close();
}
