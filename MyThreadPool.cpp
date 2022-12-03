#ifndef _MY_THREAD_POOL_CPP_
#define _MY_THREAD_POOL_CPP_

#include "MyThreadPool.h"

MyThreadPoolItem::MyThreadPoolItem() : MyThread() {
    m_IsBusy = false;
    m_ThreadPool = NULL;
}
int MyThreadPoolItem::Init(MyThreadPool* pool) {
    int err = 0;
    m_ThreadPool = pool;
    if (err = Start()) return err;
    return 0;
}

MyThreadPoolTaskInfo* MyThreadPoolItem::CurrentTaskInfo() {
    if (!m_IsBusy) return NULL;

    return &m_TaskInfo;
}

int MyThreadPoolItem::Run() {
    while (!Aborted()) {
        if (m_ThreadPool->GetNextPendingTask(&m_TaskInfo)) {
            m_IsBusy = true;
            m_TaskInfo.Entry(&m_TaskInfo);
            m_IsBusy = false;
        } else {
            m_IsBusy = false;
            Sleep(20);
        }
    }
    return 0;
}

MyThreadPool::MyThreadPool() {
    INIT_LAST_ERROR;
    m_NextTaskId = 1;
    m_MaxThreadCount = 0;
}
MyThreadPool::~MyThreadPool() {
    Destroy();
}

int MyThreadPool::Init(int maxThreadCount) {
    m_MaxThreadCount = maxThreadCount;
    return 0;
}
int MyThreadPool::Destroy() {
    m_Mutex.Acquire();
    for (int i = 0; i < m_Threads.Size(); i++) {
        MyThreadPoolItem* item = m_Threads.Get(i);
        item->Abort();
    }
    m_Threads.Reset();
    m_Mutex.Release();
    return 0;
}

bool MyThreadPool::IsFull() {
    bool isFull = true;

    m_Mutex.Acquire();
    if (m_Threads.Size() < m_MaxThreadCount) {
        isFull = false;
    } else {
        for (int i = 0; i < m_Threads.Size(); i++) {
            if (!m_Threads.Get(i)->IsBusy()) {
                isFull = false;
                break;
            }
        }
    }
    m_Mutex.Release();

    return isFull;
}

int MyThreadPool::QueueTask(MyThreadPoolItemEntry threadEntry, void* param, UINT64* retTaskId) {
    int err = 0;

    m_Mutex.Acquire();

    *retTaskId = m_NextTaskId;
    m_NextTaskId++;

    MyThreadPoolTaskInfo* ti = m_PendingTasks.AppendNew()->Data;
    ti->Entry = threadEntry;
    ti->Param = param;
    ti->TaskId = *retTaskId;

    // increase the thread count if necessary
    // TODO: check the idle thread before add a new one
    if (m_Threads.Size() < m_MaxThreadCount) {
        MyThreadPoolItem* item = m_Threads.AddNew();
        if (err = item->Init(this)) return LastError(err, item->LastErrorMessage());
    }
    m_Mutex.Release();

    return 0;
}

int MyThreadPool::CancelTask(UINT64 taskId) {
    int err = 0;
    m_Mutex.Acquire();

    bool isPending = false;
    MyLinkListNode<MyThreadPoolTaskInfo>* node = m_PendingTasks.Head();
    while (node != NULL) {
        if (node->Data->TaskId == taskId) {
            m_PendingTasks.Remove(node);
            isPending = true;
            break;
        }
    }
    if (!isPending) {
        for (int i = 0; i < m_Threads.Size(); i++) {
            MyThreadPoolTaskInfo* ti = m_Threads.Get(i)->CurrentTaskInfo();
            if (ti != NULL && ti->TaskId == taskId) {
                // TODO: Maybe it's dangerous, we should set the marker, and let the thread quit itself
                if (err = m_Threads.Get(i)->Abort()) {
                    LastError(err, m_Threads.Get(i)->LastErrorMessage());
                    goto done;
                }
                if (err = m_Threads.Delete(i)) goto done;
                break;
            }
        }
    }

done:
    m_Mutex.Release();
    return err;
}

bool MyThreadPool::GetNextPendingTask(MyThreadPoolTaskInfo* retTaskInfo) {

    bool hasMore = false;

    m_Mutex.Acquire();
    if (!m_PendingTasks.Empty()) {
        hasMore = true;
        retTaskInfo->CopyFrom(m_PendingTasks.Head()->Data);
        retTaskInfo->Entry = m_PendingTasks.Head()->Data->Entry;
        m_PendingTasks.Remove(m_PendingTasks.Head());
    }
    m_Mutex.Release();

    return hasMore;
}



#endif // _MY_THREAD_POOL_CPP_
