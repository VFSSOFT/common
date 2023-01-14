#ifndef _MY_WIN_SERVICE_H_
#define _MY_WIN_SERVICE_H_

#include "MyCoreDef.h"
#include "MyStringA.h"
#include "MyStringW.h"

class MyWinServiceConfig {
public:
    int       ServiceType;
    int       StartType;
    int       ErrorControl;
    MyStringW BinaryPathName;
    MyStringW LoadOrderGroup;
    UINT64    TagId;
    MyStringW Dependencies;
    MyStringW ServiceStartName;
    MyStringW DisplayName;
    MyStringW Description;

    MyWinServiceConfig() {
        ServiceType = SERVICE_WIN32_OWN_PROCESS;
        StartType = SERVICE_DEMAND_START;
        ErrorControl = SERVICE_ERROR_NORMAL;
        TagId = 0;
    }
};

class MyServiceManager {

public:
    int Install(MyWinServiceConfig* config);
    int Start(const wchar_t* serviceName);
    int Stop(const wchar_t* serviceName);
    int Query(const wchar_t* serviceName, MyWinServiceConfig* retConfig);
    int Disable(const wchar_t* serviceName);
    int Enable(const wchar_t* serviceName);
    int UpdateDescription(const wchar_t* serviceName, const wchar_t* description);
    int Delete(const wchar_t* serviceName);

private:
    int MyOpenScManager(SC_HANDLE* retHandle);
    int MyOpenService(SC_HANDLE scManager, const wchar_t* serviceName, DWORD desiredAccess, SC_HANDLE* retHandle);
    int MyQueryServiceStatus(SC_HANDLE scService, SERVICE_STATUS_PROCESS* retStatus);
    int MyWaitServiceStatus(SC_HANDLE scService, DWORD expState);

    int ChangeServiceStartType(const wchar_t* serviceName, DWORD startType);

    int HandleSystemCallError(const char* op);

    MY_LAST_ERROR_DECL;

};

#endif // _MY_WIN_SERVICE