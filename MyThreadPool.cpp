#ifndef _MY_THREAD_POOL_CPP_
#define _MY_THREAD_POOL_CPP_

#include "MyThreadPool.h"

MyThreadPoolItem::MyThreadPoolItem() : MyThread() {
    m_IsBusy = false;
    m_ThreadPool = NULL;
}
int MyThreadPoolItem::Init(MyThreadPool* pool, const char* name) {
    int err = 0;
    m_ThreadPool = pool;
    ThreadName()->Set(name);
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
            m_TaskInfo.Thread = this;
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
    m_State.SetIdle();
    return 0;
}
int MyThreadPool::Destroy() {
    int err = 0;
    m_Lock.Acquire();
    m_State.SetAborting();
    m_PendingTasks.Reset(); // TODO: should we quit before processing pending tasks?
    for (int i = 0; i < m_Threads.Size(); i++) {
        MyThreadPoolItem* item = m_Threads.Get(i);
        if (err = item->Abort(30 * 1000)) {
            return LastError(err, item->LastErrorMessage());
        }
    }
    m_State.SetIdle();
    m_Threads.Reset();
    m_Lock.Release();
    return 0;
}

bool MyThreadPool::IsFull() {
    bool isFull = true;

    m_Lock.Acquire();
    if (m_State.IsAborting()) return true; // don't add more task here if we're aboring
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
    m_Lock.Release();

    return isFull;
}

int MyThreadPool::QueueTask(MyThreadPoolItemEntry threadEntry, void* param, UINT64* retTaskId, const char* tname) {
    int err = 0;

    m_Lock.Acquire();

    if (m_State.IsAborting()) {
        m_Lock.Release();
        return LastError(MY_ERR_INVALID_OPERATION, "Trying to add new tasks when the thread pool is aborted");
    }

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
        if (err = item->Init(this, tname)) return LastError(err, item->LastErrorMessage());
    }
    m_Lock.Release();

    return 0;
}

int MyThreadPool::CancelTask(UINT64 taskId) {
    int err = 0;
    m_Lock.Acquire();

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
    m_Lock.Release();
    return err;
}

bool MyThreadPool::GetNextPendingTask(MyThreadPoolTaskInfo* retTaskInfo) {

    // Quick check, so that when the Destroy is being calling, GetNextPendingTask will return without dead lock
    // Calling Empty is safe here, because Empty() will only checks the integer, it will not cause an memory crash issue
    if (m_PendingTasks.Empty()) return false;

    bool hasMore = false;

    m_Lock.Acquire();
    if (!m_PendingTasks.Empty()) {
        hasMore = true;
        retTaskInfo->CopyFrom(m_PendingTasks.Head()->Data);
        retTaskInfo->Entry = m_PendingTasks.Head()->Data->Entry;
        m_PendingTasks.Remove(m_PendingTasks.Head());
    }
    m_Lock.Release();

    return hasMore;
}



#endif // _MY_THREAD_POOL_CPP_

