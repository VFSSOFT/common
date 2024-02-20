#ifndef _MY_PIPE_H_
#define _MY_PIPE_H_

#include "../MyStringA.h"
#include "../MyStringW.h"
#include "../MyWin.h"

// Pipe Names: https://learn.microsoft.com/en-us/windows/win32/ipc/pipe-names
// ref: https://www.winsocketdotnetworkprogramming.com/winsock2programming/winsock2advancednamedpipe15c.html

// Open Mode
#define MY_PIPE_ACCESS_DUPLEX   0x00000003
#define MY_PIPE_ACCESS_INBOUND  0x00000001
#define MY_PIPE_ACCESS_OUTBOUND 0x00000002

#define MY_PIPE_FILE_FLAG_FIRST_PIPE_INSTANCE 0x00080000
#define MY_PIPE_FILE_FLAG_WRITE_THROUGH       0x80000000
#define MY_PIPE_FILE_FLAG_OVERLAPPED          0x40000000

#define MY_PIPE_WRITE_DAC              0x00040000
#define MY_PIPE_WRITE_OWNER            0x00080000
#define MY_PIPE_ACCESS_SYSTEM_SECURITY 0x01000000

// Pipe Mode

#define MY_PIPE_TYPE_BYTE    0x00000000
#define MY_PIPE_TYPE_MESSAGE 0x00000004

#define MY_PIPE_READMODE_BYTE    0x00000000
#define MY_PIPE_READMODE_MESSAGE 0x00000002

#define MY_PIPE_WAIT   0x00000000
#define MY_PIPE_NOWAIT 0x00000001

#define MY_PIPE_ACCEPT_REMOTE_CLIENTS 0x00000000
#define MY_PIPE_REJECT_REMOTE_CLIENTS 0x00000008

// Max instances
#define MY_PIPE_UNLIMITED_INSTANCES 255


#define MY_PIPE_READ_BUF_SIZE 4096

class MyNamedPipeOpCtx;
class MyNamedPipeEventHandler {
public:
    virtual int OnConnected(MyNamedPipeOpCtx* pipeCtx) = 0;
    virtual int OnDisconnected(MyNamedPipeOpCtx* pipeCtx) = 0;
    virtual int OnDataIn(MyNamedPipeOpCtx* pipeCtx, const char* data, int lenData) = 0;
    virtual int OnDataOut(MyNamedPipeOpCtx* pipeCtx, const char* data, int lenData, bool writeDone) = 0;
};

// Don't acquire any unmanaged resource in MyNamedPipeOpCtx
class MyNamedPipeOpCtx {
public:
    HANDLE     PipeHandle;
    OVERLAPPED ReadOverlapped;
    OVERLAPPED WriteOverlapped;
    char*      ReadBuf;
    MyBuffer   WriteBuf;
    bool       Connected; // we're connecting in async mode
};

class MyNamedPipeServer {
public:
    MyNamedPipeServer(MyNamedPipeEventHandler* eventHandler);
    virtual ~MyNamedPipeServer();

    MyStringW* Name() { return &m_Name; }
    void SetName(const wchar_t* name) { m_Name.Set(name); }

    DWORD OpenMode() { return m_OpenMode; }
    void SetOpenMode(DWORD mode) { m_OpenMode = mode; }

    DWORD PipeMode() { return m_PipeMode; }
    void SetPipeMode(DWORD val) { m_PipeMode = val; }

    int MaxInstances() { return m_MaxInstances; }
    void SetMaxInstances(int val) { m_MaxInstances = val; }

    int DefaultTimeout() { return m_DefaultTimeout; }
    void SetDefaultTimeout(int val) { m_DefaultTimeout = val; }

    int Init();
    int DoEvents();
    int Write(void* pipe, const char* data, int lenData);
    void Reset();

private:
    int MyCreateEvent(HANDLE* evt);
    int MyConnectNamedPipe(MyNamedPipeOpCtx* ctx);
    int MyReconnect(MyNamedPipeOpCtx* ctx);
    int MyRead(MyNamedPipeOpCtx* ctx);
    int MyWrite(MyNamedPipeOpCtx* ctx);

private:
    MyStringW m_Name;
    DWORD     m_OpenMode;
    DWORD     m_PipeMode;
    int       m_MaxInstances;
    int       m_DefaultTimeout;

    MyNamedPipeOpCtx* m_Ctxs;
    HANDLE*           m_Events;

    MyNamedPipeEventHandler* m_EventHandler;
    
    MY_LAST_ERROR_DECL;
    MY_LAST_WIN_ERROR_DECL;
};



#endif // _MY_PIPE_H_
