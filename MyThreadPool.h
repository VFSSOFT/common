#ifndef _MY_THREAD_POOL_H_
#define _MY_THREAD_POOL_H_

#include "MyArray.h"
#include "MyThread.h"
#include "MyMutex.h"
#include "MyLinkList.h"

class MyThreadPool;
class MyThreadPoolTaskInfo;

typedef void (*MyThreadPoolItemEntry)(MyThreadPoolTaskInfo* ti);

class MyThreadPoolTaskInfo {
public:
    MyThreadPoolTaskInfo() {
        Entry = NULL;
        Param = NULL;
        TaskId = 0;
    }

    void CopyFrom(MyThreadPoolTaskInfo* other) {
        this->Entry = other->Entry;
        this->Param = other->Param;
        this->TaskId = other->TaskId;
    }

    MyThreadPoolItemEntry Entry;
    void*                 Param;
    UINT64                TaskId;
};

class MyThreadPoolItem : public MyThread {
public:
    MyThreadPoolItem();

    int Init(MyThreadPool* pool);
    bool IsBusy() { return m_IsBusy; }

    MyThreadPoolTaskInfo* CurrentTaskInfo();

    virtual int Run();

private:
    bool                 m_IsBusy;
    MyThreadPool*        m_ThreadPool;
    MyThreadPoolTaskInfo m_TaskInfo;
};

class MyThreadPool {
public:
    MyThreadPool();
    ~MyThreadPool();

    int Init(int maxThreadCount);
    int Destroy();

    bool IsFull();
    int QueueTask(MyThreadPoolItemEntry threadEntry, void* param, UINT64* retTaskId);
    int CancelTask(UINT64 taskId);

    PRIVATE_FUNCTION bool GetNextPendingTask(MyThreadPoolTaskInfo* retTaskInfo);

private:
    MyMutex                              m_Mutex;
    int                                  m_MaxThreadCount;
    MyArray<MyThreadPoolItem>            m_Threads;
    UINT64                               m_NextTaskId;
    MyLinkList<MyThreadPoolTaskInfo>     m_PendingTasks;

    MY_LAST_ERROR_DECL;
};

#endif // _MY_THREAD_POOL_H_
