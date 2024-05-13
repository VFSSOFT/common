#ifndef _MY_PIPE_CPP_
#define _MY_PIPE_CPP_

#include "MyPipe.h"

MyNamedPipeBase::MyNamedPipeBase(MyNamedPipeEventHandler* eventHandler) :
    m_LastErrorCode(0), 
    m_OpenMode(MY_PIPE_ACCESS_DUPLEX | MY_PIPE_FILE_FLAG_OVERLAPPED),
    m_PipeMode(MY_PIPE_TYPE_MESSAGE | MY_PIPE_READMODE_MESSAGE | MY_PIPE_WAIT),
    m_EventHandler(eventHandler)
{

}
MyNamedPipeBase::~MyNamedPipeBase() {

}

int MyNamedPipeBase::BuildPipeName(MyStringW* qualifiedName) {
    qualifiedName->Set(L"\\\\.\\pipe\\");
    qualifiedName->Append(m_Name.Deref());
    return 0;
}

int MyNamedPipeBase::MyCreateEvent(HANDLE* evt) {
    *evt = CreateEvent(
        NULL,    // default security attribute
        TRUE,    // manual reset event 
        FALSE,   // initial state = signaled 
        NULL     // unnamed event object 
    );
    if (*evt == NULL) {
        return LastWinError();
    }
    return 0;
}

int MyNamedPipeBase::MyRead(MyNamedPipeOpCtx* ctx) {
    HANDLE evt = ctx->ReadOverlapped.hEvent;
    ZeroMemory(&ctx->ReadOverlapped, sizeof(OVERLAPPED));
    ctx->ReadOverlapped.hEvent = evt;

    ResetEvent(ctx->ReadOverlapped.hEvent);
    if (ReadFile(ctx->PipeHandle, ctx->ReadBuf, MY_PIPE_READ_BUF_SIZE, NULL, &ctx->ReadOverlapped) == 0) {
        if (GetLastError() != ERROR_IO_PENDING) {
            return LastWinError();
        }
    }
    return 0;
}
int MyNamedPipeBase::MyWrite(MyNamedPipeOpCtx* ctx) {
    HANDLE evt = ctx->WriteOverlapped.hEvent;
    ZeroMemory(&ctx->WriteOverlapped, sizeof(OVERLAPPED));
    ctx->WriteOverlapped.hEvent = evt;

    ResetEvent(ctx->WriteOverlapped.hEvent);
    if (WriteFile(ctx->PipeHandle, ctx->WriteBuf.DerefConst(), ctx->WriteBuf.Length(), NULL, &ctx->WriteOverlapped) == 0) {
        if (GetLastError() != ERROR_IO_PENDING) {
            return LastWinError();
        }
    }
    return 0;
}
int MyNamedPipeBase::MyWaitEvents(HANDLE* events, int eventsCount, int timeoutMS, int* retIndex) {
    *retIndex = -1;
    DWORD waitRet = WaitForMultipleObjects(eventsCount, events, FALSE, timeoutMS);
    if (waitRet == WAIT_TIMEOUT) {
        return 0; // NO pipe events is ready
    } else if (waitRet >= WAIT_ABANDONED_0 && waitRet < WAIT_ABANDONED_0 + eventsCount) {
        return 0;
    } else if (waitRet >= WAIT_OBJECT_0 && waitRet < WAIT_OBJECT_0 + eventsCount) {
        *retIndex = waitRet - WAIT_OBJECT_0;
        return 0;
    } else {
        return LastWinError();
    }
}


MyNamedPipeServer::MyNamedPipeServer(MyNamedPipeEventHandler* eventHandler): 
    MyNamedPipeBase(eventHandler),
    m_Ctxs(NULL), 
    m_Events(NULL),
    m_MaxInstances(1),
    m_DefaultTimeout(NMPWAIT_USE_DEFAULT_WAIT)
{
    
}

MyNamedPipeServer::~MyNamedPipeServer() {
    Reset();
}

MyValArray<void*>* MyNamedPipeServer::Pipes() {
    m_ConnectedPipes.Reset();
    for (int i = 0; i < m_MaxInstances; i++) {
        if (m_Ctxs[i].Connected) {
            m_ConnectedPipes.Add(&m_Ctxs[i]);
        }
    }
    return &m_ConnectedPipes;
}

int MyNamedPipeServer::Init() {
    int ret = 0;
    MyStringW pipeName;
    if (ret = BuildPipeName(&pipeName)) return ret;

    m_Ctxs = new MyNamedPipeOpCtx[m_MaxInstances];
    m_Events = new HANDLE[m_MaxInstances * 2];

    for (int i = 0; i < m_MaxInstances; i++) {
        HANDLE pipeHandle = CreateNamedPipeW(
            pipeName.Deref(),
            m_OpenMode,
            m_PipeMode,
            m_MaxInstances,
            0, // OutBufferSize
            0, // InBufferSize
            m_DefaultTimeout,
            NULL // Use defualt security attributes
        );
        if (pipeHandle == NULL || pipeHandle == INVALID_HANDLE_VALUE) {
            return LastWinError();
        }

        if (ret = MyCreateEvent(&m_Events[2*i])) return ret;
        if (ret = MyCreateEvent(&m_Events[2*i+1])) return ret;

        MyNamedPipeOpCtx& ctx = m_Ctxs[i];
        ctx.PipeHandle = pipeHandle;
        ZeroMemory(&ctx.ReadOverlapped, sizeof(OVERLAPPED));
        ZeroMemory(&ctx.WriteOverlapped, sizeof(OVERLAPPED));
        ctx.ReadOverlapped.hEvent = m_Events[2*i];
        ctx.WriteOverlapped.hEvent = m_Events[2*i+1];

        ctx.ReadBuf = new char[MY_PIPE_READ_BUF_SIZE];
        ctx.WriteBuf.Reset();
        ctx.Connected = false;

        if (ret = MyConnectNamedPipe(&ctx)) return ret;
    }
   
    return 0;
}

int MyNamedPipeServer::DoEvents(int timeoutMS) {
    int ret = 0;
    DWORD bytesTransferred = 0;
    int idx = 0;
    int eventsCount = 2 * m_MaxInstances;
    
    if (ret = MyWaitEvents(m_Events, eventsCount, timeoutMS, &idx)) return ret;
    if (idx < 0) return 0; // no events signalled

    MyNamedPipeOpCtx& ctx = m_Ctxs[idx/2];

    if (idx % 2 == 0) { // ReadOverlapped
        if (GetOverlappedResult(ctx.PipeHandle, &ctx.ReadOverlapped, &bytesTransferred, FALSE) == 0) {
            if (ret = Disconnect(&ctx)) return ret;
        } else {
            if (!ctx.Connected) { // we're just connected
                ctx.Connected = true; 
                if (m_EventHandler) {
                    if (ret = m_EventHandler->OnConnected(&ctx)) return ret;
                }
            } else { // we're connected before
                if (m_EventHandler) {
                    if (ret = m_EventHandler->OnDataIn(&ctx, ctx.ReadBuf, bytesTransferred)) return ret;
                }
            }
            if (ret = MyRead(&ctx)) return ret;
        }

    } else { // WriteOverlapped
        if (GetOverlappedResult(ctx.PipeHandle, &ctx.WriteOverlapped, &bytesTransferred, FALSE) == 0) {
            if (ret = Disconnect(&ctx)) return ret;
        } else {
            if (m_EventHandler) {
                if (ret = m_EventHandler->OnDataOut(&ctx, ctx.WriteBuf.DerefConst(), bytesTransferred, bytesTransferred == ctx.WriteBuf.Length())) return ret;
                ctx.WriteBuf.Delete(0, bytesTransferred);
            }
        }
    }

    return 0;
}

bool MyNamedPipeServer::CanWrite(void* pipe) {
    MyNamedPipeOpCtx* ctx = (MyNamedPipeOpCtx*)pipe;
    return ctx->Connected && ctx->WriteBuf.Length() == 0;
}

int MyNamedPipeServer::Write(void* pipe, const char* data, int lenData) {
    MyNamedPipeOpCtx* ctx = (MyNamedPipeOpCtx*)pipe;
    if (!ctx->Connected) return LastError(MY_ERR_INVALID_OPERATION, "Pipe is not connected yet");
    if (ctx->WriteBuf.Length() > 0) return LastError(MY_ERR_INVALID_OPERATION, "Previous write operation is not done yet");
    
    ctx->WriteBuf.Set(data, lenData);
    return MyWrite(ctx);
}

int MyNamedPipeServer::Disconnect(void* pipe) {
    int ret = 0;
    MyNamedPipeOpCtx* ctx = (MyNamedPipeOpCtx*)pipe;

    if (ctx->Connected) {
        ctx->Connected = false;
        DisconnectNamedPipe(ctx->PipeHandle);

        if (m_EventHandler) {
            if (ret = m_EventHandler->OnDisconnected(ctx)) return ret;
        }

        return MyConnectNamedPipe(ctx);
    }
    return 0;
}

void MyNamedPipeServer::Reset() {
    if (m_Ctxs) {
        for (int i = 0; i < m_MaxInstances; i++) {
            MyNamedPipeOpCtx& ctx = m_Ctxs[i];
            /*if (ctx.Connected)*/DisconnectNamedPipe(m_Ctxs[i].PipeHandle);
            CloseHandle(m_Ctxs[i].PipeHandle);
            
            if (ctx.ReadOverlapped.hEvent) CloseHandle(ctx.ReadOverlapped.hEvent);
            if (ctx.WriteOverlapped.hEvent) CloseHandle(ctx.WriteOverlapped.hEvent);
            if (ctx.ReadBuf) delete [] ctx.ReadBuf;
        }

        delete [] m_Ctxs;
    }
    delete [] m_Events;

    m_Ctxs = NULL;
    m_Events = NULL; 

    // reinitialize default values
    m_OpenMode = MY_PIPE_ACCESS_DUPLEX | MY_PIPE_FILE_FLAG_OVERLAPPED;
    m_PipeMode = MY_PIPE_TYPE_MESSAGE | MY_PIPE_READMODE_MESSAGE | MY_PIPE_WAIT;
    m_MaxInstances = 1;
    m_DefaultTimeout = NMPWAIT_USE_DEFAULT_WAIT;
}

int MyNamedPipeServer::MyConnectNamedPipe(MyNamedPipeOpCtx* ctx) {
    if (ConnectNamedPipe(ctx->PipeHandle, &ctx->ReadOverlapped) == 0) {
        if (GetLastError() != ERROR_IO_PENDING) {
            return LastWinError();
        }
        return 0;
    } else {
        return LastWinError();
    }
}



MyNamedPipeClient::MyNamedPipeClient(MyNamedPipeEventHandler* eventHandler):
    MyNamedPipeBase(eventHandler)
{
    m_PipeMode = MY_PIPE_READMODE_MESSAGE | MY_PIPE_WAIT;
    m_Events[0] = NULL;
    m_Events[1] = NULL;
}
MyNamedPipeClient::~MyNamedPipeClient() {
    Reset();
}

int MyNamedPipeClient::Connect() {
    int ret = 0;
    MyStringW pipeName;

    if (ret = BuildPipeName(&pipeName)) return ret;

    HANDLE pipeHandle = CreateFile(
        pipeName.Deref(),
        GENERIC_READ | GENERIC_WRITE,
        0,
        NULL,
        OPEN_EXISTING,
        FILE_FLAG_OVERLAPPED,
        NULL
    );
    if (pipeHandle == NULL || pipeHandle == INVALID_HANDLE_VALUE) {
        return LastWinError();
    }

    if (ret = MyCreateEvent(&m_Events[0])) return ret;
    if (ret = MyCreateEvent(&m_Events[1])) return ret;

    m_Ctx.PipeHandle = pipeHandle;
    ZeroMemory(&m_Ctx.ReadOverlapped, sizeof(OVERLAPPED));
    ZeroMemory(&m_Ctx.WriteOverlapped, sizeof(OVERLAPPED));
    m_Ctx.ReadOverlapped.hEvent = m_Events[0];
    m_Ctx.WriteOverlapped.hEvent = m_Events[1];

    m_Ctx.ReadBuf = new char[MY_PIPE_READ_BUF_SIZE];
    m_Ctx.WriteBuf.Reset();
    m_Ctx.Connected = false;

    if (!SetNamedPipeHandleState(
        pipeHandle,
        &m_PipeMode,
        NULL,
        NULL
    )) {
        return LastWinError();
    }

    if (m_EventHandler) {
        m_EventHandler->OnConnected(&m_Ctx);
    }
    m_Ctx.Connected = true;

    if (ret = MyRead(&m_Ctx)) return ret;

    return 0;
}

int MyNamedPipeClient::Disconnect() {
    if (m_Ctx.Connected) {
        CloseHandle(m_Ctx.PipeHandle);
    }
    m_Ctx.PipeHandle = NULL;
    m_Ctx.Connected = false;

    if (m_EventHandler) {
        m_EventHandler->OnDisconnected(&m_Ctx);
    }

    return 0;
}

int MyNamedPipeClient::DoEvents(int timeoutMS) {
    int ret = 0;
    DWORD bytesTransferred = 0;
    int idx = 0;
    
    if (ret = MyWaitEvents(m_Events, 2, timeoutMS, &idx)) return ret;
    if (idx < 0) return 0; // no events signalled

    if (idx % 2 == 0) { // ReadOverlapped
        if (GetOverlappedResult(m_Ctx.PipeHandle, &m_Ctx.ReadOverlapped, &bytesTransferred, FALSE) == 0) {
            //if (ret = MyReconnect(&ctx)) return ret;
        } else {
            if (m_EventHandler) {
                if (ret = m_EventHandler->OnDataIn(&m_Ctx, m_Ctx.ReadBuf, bytesTransferred)) return ret;
            }
            if (ret = MyRead(&m_Ctx)) return ret;
        }

    } else { // WriteOverlapped
        if (GetOverlappedResult(m_Ctx.PipeHandle, &m_Ctx.WriteOverlapped, &bytesTransferred, FALSE) == 0) {
            //if (ret = MyReconnect(&ctx)) return ret;
        } else {
            if (m_EventHandler) {
                if (ret = m_EventHandler->OnDataOut(&m_Ctx, m_Ctx.WriteBuf.DerefConst(), bytesTransferred, bytesTransferred == m_Ctx.WriteBuf.Length())) return ret;
                m_Ctx.WriteBuf.Delete(0, bytesTransferred);
            }
        }
    }

    return 0;
}

int MyNamedPipeClient::Write(const char* data, int lenData) {
    if (!m_Ctx.Connected) return LastError(MY_ERR_INVALID_OPERATION, "Pipe is not connected yet");
    if (m_Ctx.WriteBuf.Length() > 0) return LastError(MY_ERR_INVALID_OPERATION, "Previous write operation is not done yet");
    
    m_Ctx.WriteBuf.Set(data, lenData);
    return MyWrite(&m_Ctx);
}

void MyNamedPipeClient::Reset() {
    Disconnect();

    if (m_Ctx.ReadOverlapped.hEvent) CloseHandle(m_Ctx.ReadOverlapped.hEvent);
    if (m_Ctx.WriteOverlapped.hEvent) CloseHandle(m_Ctx.WriteOverlapped.hEvent);
    if (m_Ctx.ReadBuf) delete [] m_Ctx.ReadBuf;

    // reinitialize default values
    m_OpenMode = MY_PIPE_ACCESS_DUPLEX | MY_PIPE_FILE_FLAG_OVERLAPPED;
    m_PipeMode = MY_PIPE_TYPE_MESSAGE | MY_PIPE_READMODE_MESSAGE | MY_PIPE_WAIT;
}





#endif // _MY_PIPE_CPP_
