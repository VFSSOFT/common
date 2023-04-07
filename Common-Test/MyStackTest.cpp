#include "pch.h"


#include "../MyStack.h"

class MyStackElem {
public:
    MyStackElem() : IntValue(-1), BoolValue(false) {}
    int  IntValue;
    bool BoolValue;
};

TEST(MyStackTest, EmptyTest) {
    MyStack<MyStackElem> stack;

    ASSERT_TRUE(stack.Empty());

    MyStackElem* elem = stack.Peek();
    ASSERT_TRUE(elem == NULL);

    elem = stack.Pop();
    ASSERT_TRUE(elem == NULL);
}

TEST(MyStackTest, PushTest) {
    MyStack<MyStackElem> stack;
    ASSERT_TRUE(stack.Empty());

    MyStackElem* elem = stack.Push();
    ASSERT_TRUE(elem != NULL);
    elem->IntValue = 1;
    elem->BoolValue = true;
     ASSERT_FALSE(stack.Empty());

    elem = stack.Push();
    ASSERT_TRUE(elem != NULL);
    elem->IntValue = 2;
    elem->BoolValue = true;
    ASSERT_FALSE(stack.Empty());
}

TEST(MyStackTest, PushPopPeekTest) {
    MyStack<MyStackElem> stack;
    ASSERT_TRUE(stack.Empty());

    MyStackElem* elem = stack.Push();
    ASSERT_TRUE(elem != NULL);
    elem->IntValue = 1;
    elem->BoolValue = true;
    ASSERT_FALSE(stack.Empty());

    elem = stack.Push();
    ASSERT_TRUE(elem != NULL);
    elem->IntValue = 2;
    elem->BoolValue = true;
    ASSERT_FALSE(stack.Empty());

    elem = stack.Push();
    elem->IntValue = 3;
    elem->BoolValue = true;

    for (int i = 3; i > 0; i--) {
        ASSERT_FALSE(stack.Empty());

        elem = stack.Peek();
        ASSERT_EQ(elem->IntValue, i);
        ASSERT_TRUE(elem->BoolValue);

        elem = stack.Pop();
        ASSERT_EQ(elem->IntValue, i);
        ASSERT_TRUE(elem->BoolValue);
    }

    ASSERT_TRUE(stack.Empty());

}










TEST(MyValStackTest, EmptyTest) {
    MyValStack<int> stack;
    ASSERT_TRUE(stack.Empty());
}

TEST(MyValStackTest, PushTest) {
    MyValStack<int> stack;
    ASSERT_TRUE(stack.Empty());

    stack.Push(1);
    ASSERT_FALSE(stack.Empty());

    stack.Push(2);
    ASSERT_FALSE(stack.Empty());
}

TEST(MyValStackTest, PushPopPeekTest) {
    MyValStack<int> stack;
    ASSERT_TRUE(stack.Empty());

    stack.Push(1);
    ASSERT_FALSE(stack.Empty());

    stack.Push(2);
    ASSERT_FALSE(stack.Empty());

    stack.Push(3);

    for (int i = 3; i > 0; i--) {
        ASSERT_FALSE(stack.Empty());

        int elem = stack.Peek();
        ASSERT_EQ(elem, i);

        elem = stack.Pop();
        ASSERT_EQ(elem, i);
    }

    ASSERT_TRUE(stack.Empty());

}
