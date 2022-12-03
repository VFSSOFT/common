#include "pch.h"

#include "../../common/MyArray.h"

class Student {
public:
  Student() {
    this->Name[0] = 'a';
    this->Name[1] = 0;
    this->Age = 20;
    this->IsMan = TRUE;
  }

  char Name[64];
  int  Age;
  BOOL IsMan;
};

TEST(MyArrayTest, DeafultValueTest) {
  MyArray<Student> students;
  EXPECT_EQ(students.Size(), 0);
  EXPECT_EQ(students.Empty(), 1);
}

TEST(MyArrayTest, AddNewTest) {
  MyArray<Student> students;

  Student* student = students.AddNew();
  EXPECT_TRUE(student != NULL);
  EXPECT_EQ(student->Name[0], 'a');
  EXPECT_EQ(student->Name[1], 0);
  EXPECT_EQ(student->Age, 20);
  EXPECT_EQ(student->IsMan, TRUE);
  EXPECT_EQ(students.Size(), 1);
}

TEST(MyArrayTest, SetSizeTest) {
  MyArray<Student> students;

  int errCode = students.SetSize(10);
  EXPECT_EQ(errCode, 0);
  EXPECT_EQ(students.Size(), 10);

  for (int i = 0; i < students.Size(); i++) {
    Student* stu = students.Get(i);
    EXPECT_TRUE(stu != NULL);
    EXPECT_EQ(stu->Name[0], 'a');
    EXPECT_EQ(stu->Name[1], 0);
    EXPECT_EQ(stu->Age, 20);
    EXPECT_EQ(stu->IsMan, TRUE);
  }

  Student* s = students.Get(11);
  EXPECT_TRUE(s == NULL);
  EXPECT_EQ(students.LastError(), MY_ERR_INDEX_OUT_OF_BOUNDARY);
}

TEST(MyArrayTest, DeleteTest) {
  MyArray<Student> students;

  int errCode = students.SetSize(10);
  EXPECT_EQ(errCode, 0);

  Student* stu = students.Get(2);
  EXPECT_TRUE(stu != NULL);
  stu->Age = 52;
  stu->IsMan = FALSE;

  errCode = students.Delete(1);
  EXPECT_EQ(errCode, 0);
  EXPECT_EQ(students.Size(), 9);

  stu = students.Get(1);
  EXPECT_TRUE(stu != NULL);
  EXPECT_EQ(stu->Name[0], 'a');
  EXPECT_EQ(stu->Name[1], 0);
  EXPECT_EQ(stu->Age, 52);
  EXPECT_EQ(stu->IsMan, FALSE);
}

TEST(MyArrayTest, DeleteLastTest) {
  MyArray<Student> students;

  int errCode = students.SetSize(10);
  EXPECT_EQ(errCode, 0);

  for (int i = 9; i >= 0; i--) {
    errCode = students.Delete(i);
    EXPECT_EQ(errCode, 0);
  }
}


