
#ifndef _MY_WIN_SERVICE_CPP_
#define _MY_WIN_SERVICE_CPP_

#include "MyWinService.h"
#include "MyWin.h"

//SERVICE_STATUS        gSvcStatus;
//SERVICE_STATUS_HANDLE gSvcStatusHandle;
//HANDLE                ghSvcStopEvent = NULL;

//VOID ReportSvcStatus(DWORD currentState, DWORD win32ExitCode, DWORD waitHint) {
//    static DWORD checkPoint = 1;
//
//    // Fill in the SERVICE_STATUS structure.
//
//    gSvcStatus.dwCurrentState  = currentState;
//    gSvcStatus.dwWin32ExitCode = win32ExitCode;
//    gSvcStatus.dwWaitHint      = waitHint;
//
//    if (currentState == SERVICE_START_PENDING) {
//        gSvcStatus.dwControlsAccepted = 0;
//    } else {
//        gSvcStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP;
//    }
//
//    if (currentState == SERVICE_RUNNING || currentState == SERVICE_STOPPED) {
//        gSvcStatus.dwCheckPoint = 0; 
//    } else {
//        gSvcStatus.dwCheckPoint = checkPoint++;
//    }
//
//    // Report the status of the service to the SCM.
//    SetServiceStatus(gSvcStatusHandle, &gSvcStatus);
//}
//
//VOID WINAPI SvcCtrlHandler(DWORD dwCtrl) {
//
//    switch (dwCtrl) {
//    case SERVICE_CONTROL_STOP:
//        ReportSvcStatus(SERVICE_STOP_PENDING, NO_ERROR, 0);
//
//        // Signal the service to stop
//        SetEvent(ghSvcStopEvent);
//
//        ReportSvcStatus(gSvcStatus.dwCurrentState, NO_ERROR, 0);
//
//        break;
//
//    case SERVICE_CONTROL_INTERROGATE:
//        break;
//    }
//
//}

//VOID WINAPI SvcMain(DWORD dwArgc, LPTSTR* lpszArgv) {
//
//    assert(dwArgc >= 1);
//    LPTSTR svnName = lpszArgv[0];
//
//    gSvcStatusHandle = RegisterServiceCtrlHandler(svnName, SvcCtrlHandler);
//    if (!gSvcStatusHandle) {
//        return;
//    }
//
//    gSvcStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
//    gSvcStatus.dwServiceSpecificExitCode = 0;
//
//    ReportSvcStatus(SERVICE_START_PENDING, NO_ERROR, 3000);
//
//    ghSvcStopEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
//    if (ghSvcStopEvent == NULL) {
//        ReportSvcStatus(SERVICE_STOPPED, MyWin::GetSysLastErrorCode(), 0);
//        return;
//    }
//
//    ReportSvcStatus(SERVICE_RUNNING, NO_ERROR, 0);
//
//    // TODO: run the custom work in another thread, so we can stop it when the service is stopped
//    // TODO: run the custom service code
//
//    while (1) {
//        WaitForSingleObject(ghSvcStopEvent, INFINITE);
//
//        ReportSvcStatus(SERVICE_STOPPED, NO_ERROR, 0);
//        return;
//    }
//}

//int Install() {
//    WCHAR modulePath[MAX_PATH];
//    if (! GetModuleFileName(NULL, modulePath, MAX_PATH)) {
//        m_LastErrorCode = MyWin::GetSysLastErrorCode();
//        MyWin::GetSysLastErrorMessage(&m_LastErrorMessage, m_LastErrorCode);
//        return m_LastErrorCode;
//    }
//
//    MyServiceManager serviceManager;
//    m_Config.BinaryPathName.Set(modulePath);
//    int err = serviceManager.Install(&m_Config);
//    if (err) return LastError(err, serviceManager.LastErrorMessage());
//
//    return 0;
//}

//int Run() {
//    int err = 0;
//
//    SERVICE_TABLE_ENTRY dispatchTable[] = {
//        { m_Config.DisplayName.Deref(), (LPSERVICE_MAIN_FUNCTION) SvcMain },
//        { NULL, NULL }
//    };
//
//    if (!StartServiceCtrlDispatcher(dispatchTable)) {
//        m_LastErrorCode = MyWin::GetSysLastErrorCode();
//        MyWin::GetSysLastErrorMessage(&m_LastErrorMessage, m_LastErrorCode);
//        return m_LastErrorCode;
//    }
//    return 0;
//}


int MyServiceManager::Install(MyWinServiceConfig* config) {
    int err = 0;
    SC_HANDLE scManager = NULL;
    SC_HANDLE scService = NULL;
    MyStringW path;

    // In case the path contains a space, it must be quoted so that it is correctly interpreted. 
    path.Set(L"\"");
    path.Append(config->BinaryPathName.Deref(), config->BinaryPathName.Length());
    path.Append(L"\"");

    if (err = MyOpenScManager(&scManager)) return err;

    scService = CreateService(
        scManager,
        config->DisplayName.Deref(),
        config->DisplayName.Deref(),
        SERVICE_ALL_ACCESS,
        config->ServiceType,
        config->StartType,
        config->ErrorControl,
        path.Deref(),
        NULL,
        NULL,
        NULL,
        NULL, // LocalSystem account 
        NULL
    );
    if (scService == NULL) {
        err = HandleSystemCallError("CreateService");
        goto done;
    }

done:
    if (scService) {
        CloseServiceHandle(scService);
    }
    if (scManager) {
        CloseServiceHandle(scManager);
    }

    return err;
}

int MyServiceManager::Start(const wchar_t* serviceName) {
    int err = 0;
    SC_HANDLE scManager = NULL;
    SC_HANDLE scService = NULL;
    SERVICE_STATUS_PROCESS ssStatus;

    if (err = MyOpenScManager(&scManager)) return err;
    if (err = MyOpenService(scManager, serviceName, SERVICE_ALL_ACCESS, &scService)) goto done;

    // Check if the service is already running
    // It would be possible to stop the service here, but for simplicity we just returns.
    if (err = MyQueryServiceStatus(scService, &ssStatus)) goto done;

    if (ssStatus.dwCurrentState != SERVICE_STOPPED && ssStatus.dwCurrentState != SERVICE_STOP_PENDING) {
        goto done;
    }

    if (ssStatus.dwCurrentState == SERVICE_STOP_PENDING) {
        if (err = MyWaitServiceStatus(scService, SERVICE_STOPPED)) goto done;
    }

    // Attempt to start the service
    if (!StartService(scService, 0, NULL)) {
        err = HandleSystemCallError("StartService");
        goto done;
    }

    if (err = MyWaitServiceStatus(scService, SERVICE_START_PENDING)) goto done;

    if (err = MyQueryServiceStatus(scService, &ssStatus)) goto done;
    if (ssStatus.dwCurrentState != SERVICE_RUNNING) {
        m_LastErrorMessage.SetWithFormat("Start service failed: State=%d, ExitCode=%d", ssStatus.dwCurrentState, ssStatus.dwWin32ExitCode);
        err = MY_ERR_INVALID_OPERATION;
        goto done;
    }

done:
    if (scService) {
        CloseServiceHandle(scService);
    }
    if (scManager) {
        CloseServiceHandle(scManager);
    }

    return err;
}
int MyServiceManager::Stop(const wchar_t* serviceName) {
    int err = 0;
    SC_HANDLE scManager = NULL;
    SC_HANDLE scService = NULL;
    SERVICE_STATUS_PROCESS ssStatus;
    DWORD desiredAccess = SERVICE_STOP | SERVICE_QUERY_STATUS | SERVICE_ENUMERATE_DEPENDENTS;

    if (err = MyOpenScManager(&scManager)) goto done;
    if (err = MyOpenService(scManager, serviceName, desiredAccess, &scService)) goto done;

    if (err = MyQueryServiceStatus(scService, &ssStatus)) goto done;
    if (ssStatus.dwCurrentState == SERVICE_STOPPED) {
        goto done;
    }
    if (ssStatus.dwCurrentState == SERVICE_STOP_PENDING) {
        if (err = MyWaitServiceStatus(scService, SERVICE_STOPPED)) goto done;
        goto done;
    }

    // StopDependentServices();

    // Send a stop coe to the service
    if (!ControlService(scService, SERVICE_CONTROL_STOP, (LPSERVICE_STATUS)&ssStatus)) {
        err = HandleSystemCallError("ControlService");
        goto done;
    }

    if (err = MyWaitServiceStatus(scService, SERVICE_STOPPED)) goto done;


done:
    if (scService) {
        CloseServiceHandle(scService);
    }
    if (scManager) {
        CloseServiceHandle(scManager);
    }

    return err;
}
int MyServiceManager::Query(const wchar_t* serviceName, MyWinServiceConfig* retConfig) {
    int err = 0;
    SC_HANDLE scManager = NULL;
    SC_HANDLE scService = NULL;
    LPQUERY_SERVICE_CONFIG lpsc = NULL;
    LPSERVICE_DESCRIPTION lpsd = NULL;
    DWORD bytesNeeded, cbBufSize, dwError;

    if (err = MyOpenScManager(&scManager)) goto done;
    if (err = MyOpenService(scManager, serviceName, SERVICE_QUERY_CONFIG, &scService)) goto done;

    if (!QueryServiceConfig(scService, NULL, 0, &bytesNeeded)) {
        dwError = MyWin::GetSysLastErrorCode();
        if (dwError == ERROR_INSUFFICIENT_BUFFER) {
            // continue
            cbBufSize = bytesNeeded;
            lpsc = (LPQUERY_SERVICE_CONFIG) LocalAlloc(LMEM_FIXED, cbBufSize);
        } else {
            err = HandleSystemCallError("QueryServiceConfig");
            goto done;
        }
    }
    if (!QueryServiceConfig(scService, lpsc, cbBufSize, &bytesNeeded)) {
        err = HandleSystemCallError("QueryServiceConfig");
        goto done;
    }

    if (!QueryServiceConfig2(scService, SERVICE_CONFIG_DESCRIPTION, NULL, 0, &bytesNeeded)) {
        dwError = MyWin::GetSysLastErrorCode();
        if (dwError == ERROR_INSUFFICIENT_BUFFER) {
            // continue
            cbBufSize = bytesNeeded;
            lpsd = (LPSERVICE_DESCRIPTION) LocalAlloc(LMEM_FIXED, cbBufSize);
        } else {
            err = HandleSystemCallError("QueryServiceConfig2");
            goto done;
        }
    }
    if (!QueryServiceConfig2(scService, SERVICE_CONFIG_DESCRIPTION, (LPBYTE)lpsd, cbBufSize, &bytesNeeded)) {
        err = HandleSystemCallError("QueryServiceConfig2");
        goto done;
    }

    retConfig->ServiceType  = (int) lpsc->dwServiceType;
    retConfig->StartType    = (int) lpsc->dwStartType;
    retConfig->ErrorControl = (int) lpsc->dwErrorControl;
    retConfig->BinaryPathName.Set(lpsc->lpBinaryPathName);
    retConfig->LoadOrderGroup.Set(lpsc->lpLoadOrderGroup);
    retConfig->TagId = lpsc->dwTagId;
    retConfig->Dependencies.Set(lpsc->lpDependencies);
    retConfig->ServiceStartName.Set(lpsc->lpServiceStartName);
    retConfig->DisplayName.Set(lpsc->lpDisplayName);
    retConfig->Description.Set(lpsd->lpDescription);

done:
    if (lpsc) {
        LocalFree(lpsc);
    }
    if (lpsd) {
        LocalFree(lpsd);
    }
    if (scService) {
        CloseServiceHandle(scService);
    }
    if (scManager) {
        CloseServiceHandle(scManager);
    }

    return err;
}
int MyServiceManager::Disable(const wchar_t* serviceName) {
    return ChangeServiceStartType(serviceName, SERVICE_DISABLED);
}
int MyServiceManager::Enable(const wchar_t* serviceName) {
    return ChangeServiceStartType(serviceName, SERVICE_START);
}
int MyServiceManager::ChangeServiceStartType(const wchar_t* serviceName, DWORD startType) {
    int err = 0;
    SC_HANDLE scManager = NULL;
    SC_HANDLE scService = NULL;
    BOOL suc = FALSE;

    if (err = MyOpenScManager(&scManager)) goto done;
    if (err = MyOpenService(scManager, serviceName, SERVICE_CHANGE_CONFIG, &scService)) goto done;

    suc = ChangeServiceConfig(
        scService,
        SERVICE_NO_CHANGE, // service type: no change
        startType,         // service start type
        SERVICE_NO_CHANGE, // error control: no change
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL,
        NULL
    );
    if (!suc) {
        err = HandleSystemCallError("ChangeServiceConfig");
        goto done;
    }

done:
    if (scService) {
        CloseServiceHandle(scService);
    }
    if (scManager) {
        CloseServiceHandle(scManager);
    }

    return err;
}
int MyServiceManager::UpdateDescription(const wchar_t* serviceName, const wchar_t* description) {
    int err = 0;
    SC_HANDLE scManager = NULL;
    SC_HANDLE scService = NULL;
    SERVICE_DESCRIPTION sd;
    BOOL suc = FALSE;

    if (err = MyOpenScManager(&scManager)) goto done;
    if (err = MyOpenService(scManager, serviceName, SERVICE_CHANGE_CONFIG, &scService)) goto done;

    sd.lpDescription = (LPWSTR)description;
    suc = ChangeServiceConfig2(
        scService,
        SERVICE_CONFIG_DESCRIPTION,
        &sd
    );
    if (!suc) {
        err = HandleSystemCallError("ChangeServiceConfig2");
        goto done;
    }

done:
    if (scService) {
        CloseServiceHandle(scService);
    }
    if (scManager) {
        CloseServiceHandle(scManager);
    }

    return err;
}
int MyServiceManager::Delete(const wchar_t* serviceName) {
    int err = 0;
    SC_HANDLE scManager = NULL;
    SC_HANDLE scService = NULL;
    BOOL suc = FALSE;

    if (err = MyOpenScManager(&scManager)) goto done;
    if (err = MyOpenService(scManager, serviceName, DELETE, &scService)) goto done;

    suc = DeleteService(scService);
    if (!suc) {
        err = HandleSystemCallError("DeleteService");
        goto done;
    }

done:
    if (scService) {
        CloseServiceHandle(scService);
    }
    if (scManager) {
        CloseServiceHandle(scManager);
    }

    return err;
}

int MyServiceManager::MyOpenScManager(SC_HANDLE* retHandle) {
    SC_HANDLE scManager = OpenSCManager(
        NULL, // machine name
        NULL, // data base name
        SC_MANAGER_ALL_ACCESS
    );

    if (scManager == NULL) {
        return HandleSystemCallError("OpenSCManager");
    }

    *retHandle = scManager;
    return 0;
}
int MyServiceManager::MyOpenService(SC_HANDLE scManager, const wchar_t* serviceName, DWORD desiredAccess, SC_HANDLE* retHandle) {
    SC_HANDLE scService = OpenService(
        scManager,
        serviceName,
        desiredAccess
    );

    if (scService == NULL) {
        return HandleSystemCallError("OpenService");
    }

    *retHandle = scService;
    return 0;
}
int MyServiceManager::MyQueryServiceStatus(SC_HANDLE scService, SERVICE_STATUS_PROCESS* retStatus) {

    DWORD bytesNeeded = 0;
    BOOL suc = QueryServiceStatusEx(
        scService,
        SC_STATUS_PROCESS_INFO,
        (LPBYTE) retStatus,
        sizeof(SERVICE_STATUS_PROCESS),
        &bytesNeeded
    );

    if (!suc) {
        return HandleSystemCallError("QueryServiceStatus");
    }

    return 0;
}
int MyServiceManager::MyWaitServiceStatus(SC_HANDLE scService, DWORD expState) {
    int err = 0;
    SERVICE_STATUS_PROCESS ssStatus;
    DWORD oldCheckPoint;
    DWORD startTickCount;
    DWORD waitTime;

    if (err = MyQueryServiceStatus(scService, &ssStatus)) return err;

    startTickCount = GetTickCount();
    oldCheckPoint = ssStatus.dwCheckPoint;

    while (ssStatus.dwCurrentState != expState) {
        // Do not wait longer than the wait hint. A good interval is 
        // one-tenth the wait hint, but no less than 1 second and no 
        // more than 10 seconds.

        waitTime = ssStatus.dwWaitHint / 10;

        if (waitTime < 1000) {
            waitTime = 1000;
        } else if (waitTime > 10000) {
            waitTime = 10000;
        }

        Sleep(waitTime);

        if (err = MyQueryServiceStatus(scService, &ssStatus)) return err;

        if (ssStatus.dwCheckPoint > oldCheckPoint) {
            // Continue to wait and check
            startTickCount = GetTickCount();
            oldCheckPoint = ssStatus.dwCheckPoint;
        } else {
            if (GetTickCount() - startTickCount > ssStatus.dwWaitHint) {
                return LastError(MY_ERR_OP_TIMEOUT, "Wait for service state timeout");
            }
        }
    }

    return 0;
}

int MyServiceManager::HandleSystemCallError(const char* op) {
    MyStringA errMsg;
    m_LastErrorCode = MyWin::GetSysLastErrorCode();
    MyWin::GetSysLastErrorMessage(&errMsg, m_LastErrorCode);
    m_LastErrorMessage.SetWithFormat("%s: %s", op, errMsg.Deref());
    return m_LastErrorCode;
}


#endif // _MY_WIN_SERVICE_CPP_
