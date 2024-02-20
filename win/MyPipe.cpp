#ifndef _MY_PIPE_CPP_
#define _MY_PIPE_CPP_

#include "MyPipe.h"


MyNamedPipeServer::MyNamedPipeServer(MyNamedPipeEventHandler* eventHandler): 
    m_Ctxs(NULL), 
    m_Events(NULL),
    m_LastErrorCode(0), 
    m_OpenMode(MY_PIPE_ACCESS_DUPLEX | MY_PIPE_FILE_FLAG_OVERLAPPED),
    m_PipeMode(MY_PIPE_TYPE_MESSAGE | MY_PIPE_READMODE_MESSAGE | MY_PIPE_WAIT),
    m_MaxInstances(1),
    m_DefaultTimeout(NMPWAIT_USE_DEFAULT_WAIT),
    m_EventHandler(eventHandler)
{
    
}

MyNamedPipeServer::~MyNamedPipeServer() {
    Reset();
}

int MyNamedPipeServer::Init() {
    int ret = 0;
    MyStringW pipeName;
    pipeName.Set(L"\\\\.\\pipe\\");
    pipeName.Append(m_Name.Deref());

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

        MyNamedPipeOpCtx ctx = m_Ctxs[i];
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

int MyNamedPipeServer::DoEvents() {
    int ret = 0;
    DWORD waitRet = 0;
    DWORD bytesTransferred = 0;
    int idx = 0;
    int eventsCount = 2 * m_MaxInstances;
    MyNamedPipeOpCtx ctx;
    
    waitRet = WaitForMultipleObjects(eventsCount, m_Events, FALSE, 0);
    if (waitRet == WAIT_TIMEOUT) {
        return 0; // NO pipe events is ready
    } else if (waitRet >= WAIT_ABANDONED_0 && waitRet < WAIT_ABANDONED_0 + eventsCount) {
        return 0;
    } else if (waitRet >= WAIT_OBJECT_0 && waitRet < WAIT_OBJECT_0 + eventsCount) {
        // continue
    } else {
        return LastWinError();
    }

    idx = waitRet - WAIT_OBJECT_0;
    ctx = m_Ctxs[idx/2];

    if (idx % 2 == 0) { // ReadOverlapped
        ResetEvent(ctx.ReadOverlapped.hEvent);

        if (GetOverlappedResult(ctx.PipeHandle, &ctx.ReadOverlapped, &bytesTransferred, FALSE) == 0) {
            if (ret = MyReconnect(&ctx)) return ret;
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
        ResetEvent(ctx.WriteOverlapped.hEvent);

        if (GetOverlappedResult(ctx.PipeHandle, &ctx.WriteOverlapped, &bytesTransferred, FALSE) == 0) {
            if (ret = MyReconnect(&ctx)) return ret;
        } else {
            if (m_EventHandler) {
                if (ret = m_EventHandler->OnDataOut(&ctx, ctx.WriteBuf.DerefConst(), bytesTransferred, bytesTransferred == ctx.WriteBuf.Length())) return ret;
                ctx.WriteBuf.Delete(0, bytesTransferred);
            }
        }
    }

    return 0;
}

int MyNamedPipeServer::Write(void* pipe, const char* data, int lenData) {
    MyNamedPipeOpCtx* ctx = (MyNamedPipeOpCtx*)pipe;
    if (!ctx->Connected) return LastError(MY_ERR_INVALID_OPERATION, "Pipe is not connected yet");
    if (ctx->WriteBuf.Length() > 0) return LastError(MY_ERR_INVALID_OPERATION, "Previous write operation is not done yet");
    
    ctx->WriteBuf.Set(data, lenData);
    return MyWrite(ctx);
}

void MyNamedPipeServer::Reset() {
    if (m_Ctxs) {
        for (int i = 0; i < m_MaxInstances; i++) {
            MyNamedPipeOpCtx ctx = m_Ctxs[i];
            if (ctx.Connected) DisconnectNamedPipe(m_Ctxs[i].PipeHandle);
            
            if (ctx.ReadOverlapped.hEvent) CloseHandle(ctx.ReadOverlapped.hEvent);
            if (ctx.WriteOverlapped.hEvent) CloseHandle(ctx.WriteOverlapped.hEvent);
            if (ctx.ReadBuf) delete [] ctx.ReadBuf;
        }

        delete [] m_Ctxs;
    }

    m_Ctxs = NULL;
    m_Events = NULL; 

    // reinitialize default values
    m_OpenMode = MY_PIPE_ACCESS_DUPLEX | MY_PIPE_FILE_FLAG_OVERLAPPED;
    m_PipeMode = MY_PIPE_TYPE_MESSAGE | MY_PIPE_READMODE_MESSAGE | MY_PIPE_WAIT,
    m_MaxInstances = 1;
    m_DefaultTimeout = NMPWAIT_USE_DEFAULT_WAIT;
}


int MyNamedPipeServer::MyCreateEvent(HANDLE* evt) {
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
int MyNamedPipeServer::MyReconnect(MyNamedPipeOpCtx* ctx) {
    int ret = 0;
    ctx->Connected = false;
    DisconnectNamedPipe(ctx->PipeHandle);

    if (m_EventHandler) {
        if (ret = m_EventHandler->OnDisconnected(ctx)) return ret;
    }

    return MyConnectNamedPipe(ctx);
}
int MyNamedPipeServer::MyRead(MyNamedPipeOpCtx* ctx) {
    HANDLE evt = ctx->ReadOverlapped.hEvent;
    ZeroMemory(&ctx->ReadOverlapped, sizeof(OVERLAPPED));
    ctx->ReadOverlapped.hEvent = evt;

    if (ReadFile(ctx->PipeHandle, ctx->ReadBuf, MY_PIPE_READ_BUF_SIZE, NULL, &ctx->ReadOverlapped) == 0) {
        if (GetLastError() != ERROR_IO_PENDING) {
            return LastWinError();
        }
    }
    return 0;
}
int MyNamedPipeServer::MyWrite(MyNamedPipeOpCtx* ctx) {
    HANDLE evt = ctx->WriteOverlapped.hEvent;
    ZeroMemory(&ctx->WriteOverlapped, sizeof(OVERLAPPED));
    ctx->WriteOverlapped.hEvent = evt;


    if (WriteFile(ctx->PipeHandle, ctx->WriteBuf.DerefConst(), ctx->WriteBuf.Length(), NULL, &ctx->WriteOverlapped) == 0) {
        if (GetLastError() != ERROR_IO_PENDING) {
            return LastWinError();
        }
    }
    return 0;
}




#endif // _MY_PIPE_CPP_
