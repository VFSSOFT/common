#include "pch.h"

#include "../MyValArray.h"

TEST(MyValArray, InsertAtEndTest) {
    int err = 0;
    MyValArray<int> arr;

    arr.Add(0);
    arr.Add(2);

    err = arr.Insert(1, 1);
    ASSERT_EQ(err, 0);

    ASSERT_EQ(arr.Size(), 3);
    ASSERT_EQ(arr.Get(1), 1);
    ASSERT_EQ(arr.Get(2), 2);
}

TEST(MyValArray, InsertAtStartTest) {
    int err = 0;
    MyValArray<int> arr;

    arr.Add(1);
    arr.Add(2);

    err = arr.Insert(0, 0);
    ASSERT_EQ(err, 0);

    ASSERT_EQ(arr.Size(), 3);
    ASSERT_EQ(arr.Get(1), 1);
    ASSERT_EQ(arr.Get(2), 2);
}

TEST(MyValArray, InsertAtMiddleTest) {
    int err = 0;
    MyValArray<int> arr;

    arr.Add(1);
    arr.Add(3);

    err = arr.Insert(1, 2);
    ASSERT_EQ(err, 0);

    ASSERT_EQ(arr.Size(), 3);
    ASSERT_EQ(arr.Get(0), 1);
    ASSERT_EQ(arr.Get(1), 2);
    ASSERT_EQ(arr.Get(2), 3);
}