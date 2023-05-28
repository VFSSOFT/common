#ifndef MY_STACK_H_
#define MY_STACK_H_

#include "MyArray.h"
#include "MyValArray.h"

template<typename T>
class MyStack {

public:
    MyStack(): m_Count(0), m_Elems(0) {}
    virtual ~MyStack() { }

    T* Pop();
    T* Push();
    T* Peek();

    bool Empty() { return m_Count == 0; }
    void Reset() { 
        m_Count = 0;
        m_Elems.Reset(); 
    }

private:
    int        m_Count;
    MyArray<T> m_Elems;
};


template <typename T>
T* MyStack<T>::Pop() {
    if (Empty()) return NULL;

    T* ret = m_Elems.Get(m_Count - 1);
    for (int i = m_Elems.Size() - 1; i >= m_Count; i--) {
        m_Elems.Delete(i);
    }
    m_Count--;
    return ret;
}

template <typename T>
T* MyStack<T>::Push() {
    for (int i = m_Elems.Size() - 1; i > m_Count; i--) {
        m_Elems.Delete(i);
    }

    T* ret = m_Elems.AddNew();
    m_Count++;
    return ret;
}

template <typename T>
T* MyStack<T>::Peek() {
    if (Empty()) return NULL;
    return m_Elems.Get(m_Count - 1);
}



template<typename T>
class MyValStack {

public:
    MyValStack(): m_Count(0), m_Elems(0) {}
    virtual ~MyValStack() { }

    T    Pop();
    void Push(T val);
    T    Peek();
    T    Bottom();
    T    Top();

    bool Empty() { return m_Count == 0; }
    void Reset() { 
        m_Count = 0;
        m_Elems.Reset(); 
    }

private:
    int           m_Count;
    MyValArray<T> m_Elems;
};


template <typename T>
T MyValStack<T>::Pop() {
    assert(m_Count > 0);

    T ret = m_Elems.Get(m_Count - 1);
    for (int i = m_Elems.Size() - 1; i >= m_Count; i--) {
        m_Elems.Delete(i);
    }
    m_Count--;
    return ret;
}

template <typename T>
void MyValStack<T>::Push(T val) {
    for (int i = m_Elems.Size() - 1; i > m_Count; i--) {
        m_Elems.Delete(i);
    }

    m_Elems.Add(val);
    m_Count++;
}

template <typename T>
T MyValStack<T>::Peek() {
    assert(m_Count > 0);
    return m_Elems.Get(m_Count - 1);
}

template <typename T>
T MyValStack<T>::Bottom() {
    assert(m_Count > 0);
    return m_Elems.Get(0);
}

template <typename T>
T MyValStack<T>::Top() {
    return Peek();
}





#endif // MY_STACK_H_