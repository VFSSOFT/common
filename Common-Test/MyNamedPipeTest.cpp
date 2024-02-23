#include "pch.h"

#include "../../common/win/MyPipe.h"

class TestPipeEventHandler : public MyNamedPipeEventHandler {
public:
    TestPipeEventHandler(): Connected(0), Disconnected(0) {}

    int Connected;
    int Disconnected;

    virtual int OnConnected(MyNamedPipeOpCtx* pipeCtx) { 
        Connected++;
        return 0; 
    }

    virtual int OnDisconnected(MyNamedPipeOpCtx* pipeCtx)  { 
        Disconnected++;
        return 0;
    }

    virtual int OnDataIn(MyNamedPipeOpCtx* pipeCtx, const char* data, int lenData) { return 0; }
    virtual int OnDataOut(MyNamedPipeOpCtx* pipeCtx, const char* data, int lenData, bool writeDone) { return 0; }
};

TEST(MyPipeTest, ConnectToNotExistedPipeTest) {
    int ret = 0;
    TestPipeEventHandler eventHandler;
    MyNamedPipeClient client(&eventHandler);

    client.SetName(L"fakename");
    ret = client.Connect();
    ASSERT_EQ(ret, 2);

    const char* errmsg = client.LastErrorMessage()->Deref();
    ASSERT_STREQ(errmsg, "The system cannot find the file specified.\r\n");
}

TEST(MyPipeTest, ServerWaitOneConnectionTest) {
    int ret = 0;
    TestPipeEventHandler eventHandler;
    MyNamedPipeServer server(&eventHandler);

    server.SetName(L"testpipename");
    ret = server.Init();
    ASSERT_EQ(ret, 0);

    for (int i = 0; i < 10; i++) {
        ret = server.DoEvents(20);
        ASSERT_EQ(ret, 0);
    }
}

TEST(MyPipeTest, ServerWait10ConnectionsTest) {
    int ret = 0;
    TestPipeEventHandler eventHandler;
    MyNamedPipeServer server(&eventHandler);

    server.SetName(L"testpipename");
    server.SetMaxInstances(10);
    ret = server.Init();
    ASSERT_EQ(ret, 0);

    for (int i = 0; i < 10; i++) {
        ret = server.DoEvents(20);
        ASSERT_EQ(ret, 0);
    }
}

TEST(MyPipeTest, ClientConnectTest) {
    int ret = 0;
    TestPipeEventHandler clientEvtHandler;
    TestPipeEventHandler serverEvtHandler;
    MyNamedPipeClient client(&clientEvtHandler);
    MyNamedPipeServer server(&serverEvtHandler);

    client.SetName(L"pipename");
    server.SetName(L"pipename");

    ret = server.Init();
    ASSERT_EQ(ret, 0);

    ret = client.Connect();
    ASSERT_EQ(ret, 0);
    ASSERT_EQ(clientEvtHandler.Connected, 1);
    ASSERT_EQ(clientEvtHandler.Disconnected, 0);

    ret = server.DoEvents(20);
    ASSERT_EQ(ret, 0);
    ASSERT_EQ(serverEvtHandler.Connected, 1);
    ASSERT_EQ(serverEvtHandler.Disconnected, 0);

    ret = client.Disconnect();
    ASSERT_EQ(ret, 0);
    ASSERT_EQ(clientEvtHandler.Connected, 1);
    ASSERT_EQ(clientEvtHandler.Disconnected, 1);

    ret = server.DoEvents(20);
    ASSERT_EQ(ret, 0);
    ASSERT_EQ(serverEvtHandler.Connected, 1);
    ASSERT_EQ(serverEvtHandler.Disconnected, 1);
}

TEST(MyPipeTest, ClientDisconnectTest) {
    int ret = 0;
    TestPipeEventHandler clientEvtHandler;
    TestPipeEventHandler serverEvtHandler;
    MyNamedPipeClient client(&clientEvtHandler);
    MyNamedPipeServer server(&serverEvtHandler);

    client.SetName(L"pipename");
    server.SetName(L"pipename");

    ret = server.Init();
    ASSERT_EQ(ret, 0);

    ret = client.Connect();
    ASSERT_EQ(ret, 0);

    ret = server.DoEvents(20);
    ASSERT_EQ(ret, 0);
    ASSERT_EQ(serverEvtHandler.Connected, 1);
    ASSERT_EQ(serverEvtHandler.Disconnected, 0);

    MyValArray<void*>* pipes = server.Pipes();
    ASSERT_EQ(pipes->Size(), 1);

    ret = client.Disconnect();
    ASSERT_EQ(ret, 0);

    ret = server.DoEvents(20);
    ASSERT_EQ(ret, 0);
    ASSERT_EQ(serverEvtHandler.Connected, 1);
    ASSERT_EQ(serverEvtHandler.Disconnected, 1);
    pipes = server.Pipes();
    ASSERT_EQ(pipes->Size(), 0);
}

TEST(MyPipeTest, ServerDisconnectTest) {
    int ret = 0;
    TestPipeEventHandler clientEvtHandler;
    TestPipeEventHandler serverEvtHandler;
    MyNamedPipeClient client(&clientEvtHandler);
    MyNamedPipeServer server(&serverEvtHandler);

    client.SetName(L"pipename");
    server.SetName(L"pipename");

    ret = server.Init();
    ASSERT_EQ(ret, 0);

    ret = client.Connect();
    ASSERT_EQ(ret, 0);

    ret = server.DoEvents(20);
    ASSERT_EQ(ret, 0);
    ASSERT_EQ(serverEvtHandler.Connected, 1);
    ASSERT_EQ(serverEvtHandler.Disconnected, 0);

    MyValArray<void*>* pipes = server.Pipes();
    ASSERT_EQ(pipes->Size(), 1);

    ret = server.Disconnect(pipes->Get(0));
    ASSERT_EQ(ret, 0);

    ret = server.DoEvents(20);
    ASSERT_EQ(ret, 0);
    ASSERT_EQ(serverEvtHandler.Connected, 1);
    ASSERT_EQ(serverEvtHandler.Disconnected, 1);

    pipes = server.Pipes();
    ASSERT_EQ(pipes->Size(), 0);
}

