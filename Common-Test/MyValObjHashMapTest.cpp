#include "pch.h"


#include "../MyValObjHashMap.h"

class MyValObjHashMapObj {
public:
  int Value;
};

TEST(MyValObjHashMapTest, OneEntryTest) {
  MyValObjHashMap<int, MyValObjHashMapObj> map;

  MyValObjHashMapObj* obj = map.Get(0);
  EXPECT_TRUE(obj == NULL);

  MyValObjHashNode<int, MyValObjHashMapObj>* node = map.Put(0);
  node->Value->Value = 123;

  obj = map.Get(0);
  EXPECT_TRUE(obj != NULL);
  EXPECT_TRUE(obj->Value == 123);
}

TEST(MyValObjHashMapTest, ThousandEntriesTest) {
  MyValObjHashMap<int, MyValObjHashMapObj> map;

  for (int i = 1; i <= 1024; i++) {
    MyValObjHashNode<int, MyValObjHashMapObj>* node = map.Put(i);
    node->Value->Value = i * 1024;

    MyValObjHashMapObj* obj = map.Get(i);
    EXPECT_TRUE(obj != NULL);
    EXPECT_TRUE(obj->Value == i * 1024);
  }
  
  for (int i = 1024; i > 0; i--) {
    MyValObjHashMapObj* obj = map.Get(i);
    EXPECT_TRUE(obj != NULL);
    EXPECT_TRUE(obj->Value == i * 1024);
  }
}

TEST(MyValObjHashMapTest, AddDeleteTest) {
  MyValObjHashMap<int, MyValObjHashMapObj> map;

  for (int i = 1; i <= 1024; i++) {
    MyValObjHashNode<int, MyValObjHashMapObj>* node = map.Put(i);
    node->Value->Value = i * 1024;

    MyValObjHashMapObj* obj = map.Get(i);
    EXPECT_TRUE(obj != NULL);
    EXPECT_TRUE(obj->Value == i * 1024);

    if (i % 8 == 0) {
      map.Delete(i);
      obj = map.Get(i);
      EXPECT_TRUE(obj == NULL);
    }
  }
  
  for (int i = 1024; i > 0; i--) {
    MyValObjHashMapObj* obj = map.Get(i);
    if (i % 8 == 0) {
      EXPECT_TRUE(obj == NULL);
    } else {
      EXPECT_TRUE(obj != NULL);
      EXPECT_TRUE(obj->Value == i * 1024);
    }
  }
}