#include "pch.h"

#include "../../common/MyLinkList.h"

class MyLinkListTestData {
public:
  int IntVal;
};

TEST(MyLinkListTest, DeafultTest) {
  MyLinkList<MyLinkListTestData> linkList;

  EXPECT_EQ(linkList.Size(), 0);
  EXPECT_EQ(linkList.Empty(), TRUE);
  EXPECT_TRUE(linkList.Head() == NULL);
  EXPECT_TRUE(linkList.Tail() == NULL);
}

TEST(MyLinkListTest, AppendTest) {
  MyLinkList<MyLinkListTestData> linkList;

  MyLinkListNode<MyLinkListTestData>* node = linkList.AppendNew();
  node->Data->IntVal = 123;

  EXPECT_EQ(linkList.Size(), 1);
  EXPECT_EQ(linkList.Empty(), FALSE);
  EXPECT_TRUE(linkList.Head() == node);
  EXPECT_TRUE(linkList.Tail() == node);
  EXPECT_TRUE(linkList.Head()->Data->IntVal == 123);
}

TEST(MyLinkListTest, AppendManyNodesTest) {
  int nodeCount = 100;
  MyLinkList<MyLinkListTestData> linkList;

  for (int i = 0; i < nodeCount; i++) {
    MyLinkListNode<MyLinkListTestData>* node = linkList.AppendNew();
    node->Data->IntVal = i;
  }

  EXPECT_EQ(linkList.Size(), nodeCount);
  EXPECT_EQ(linkList.Empty(), FALSE);

  EXPECT_TRUE(linkList.Head()->Data->IntVal == 0);
  EXPECT_TRUE(linkList.Tail()->Data->IntVal == 99);

  int curIntVal = 0;
  MyLinkListNode<MyLinkListTestData>* node = linkList.Head();
  while (node != NULL) {
    EXPECT_EQ(node->Data->IntVal, curIntVal);
    curIntVal++;
    node = node->Next;
  }
  EXPECT_EQ(curIntVal, 100);

  curIntVal = 99;
  node = linkList.Tail();
  while (node != NULL) {
    EXPECT_EQ(node->Data->IntVal, curIntVal);
    curIntVal--;
    node = node->Prev;
  }
  EXPECT_EQ(curIntVal, -1);
}

TEST(MyLinkListTest, InsertAfterTest) {
  MyLinkList<MyLinkListTestData> linkList;

  MyLinkListNode<MyLinkListTestData>* node = linkList.AppendNew();
  node->Data->IntVal = 1;

  MyLinkListNode<MyLinkListTestData>* insertedNode = linkList.InsertAfter(node);
  insertedNode->Data->IntVal = 3;

  insertedNode = linkList.InsertAfter(node);
  insertedNode->Data->IntVal = 2;

  EXPECT_TRUE(linkList.Size() == 3);

  node = linkList.Head();
  EXPECT_TRUE(node->Data->IntVal == 1);
  node = node->Next;
  EXPECT_TRUE(node->Data->IntVal == 2);
  node = node->Next;
  EXPECT_TRUE(node->Data->IntVal == 3);
}

TEST(MyLinkListTest, InsertBeforeTest) {
  MyLinkList<MyLinkListTestData> linkList;

  MyLinkListNode<MyLinkListTestData>* node = linkList.AppendNew();
  node->Data->IntVal = 1;

  MyLinkListNode<MyLinkListTestData>* insertedNode = linkList.InsertBefore(node);
  insertedNode->Data->IntVal = 3;

  insertedNode = linkList.InsertBefore(node);
  insertedNode->Data->IntVal = 2;

  EXPECT_TRUE(linkList.Size() == 3);

  node = linkList.Head();
  EXPECT_TRUE(node->Data->IntVal == 3);
  node = node->Next;
  EXPECT_TRUE(node->Data->IntVal == 2);
  node = node->Next;
  EXPECT_TRUE(node->Data->IntVal == 1);
}

TEST(MyLinkListTest, RemoveTest) {
  MyLinkList<MyLinkListTestData> linkList;

  MyLinkListNode<MyLinkListTestData>* node = linkList.AppendNew();
  node->Data->IntVal = 1;

  EXPECT_TRUE(linkList.Size() == 1);

  linkList.Remove(node);
  EXPECT_TRUE(linkList.Size() == 0);

  node = linkList.AppendNew();
  node->Data->IntVal = 1;
  node = linkList.AppendNew();
  node->Data->IntVal = 3;
  node = linkList.AppendNew();
  node->Data->IntVal = 4;
  EXPECT_TRUE(linkList.Size() == 3);

  // remove intermediate node
  linkList.Remove(linkList.Head()->Next);
  EXPECT_TRUE(linkList.Size() == 2);
  EXPECT_TRUE(linkList.Head()->Data->IntVal == 1);
  EXPECT_TRUE(linkList.Tail()->Data->IntVal == 4);

  // remove tail node
  linkList.Remove(linkList.Head()->Next);
  EXPECT_TRUE(linkList.Size() == 1);
  EXPECT_TRUE(linkList.Head()->Data->IntVal == 1);
  EXPECT_TRUE(linkList.Tail()->Data->IntVal == 1);
}