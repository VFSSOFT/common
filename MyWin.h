#ifndef _MY_WIN_H_
#define _MY_WIN_H_

#include "MyStringA.h"
#include "MyStringW.h"
#include "MyArray.h"
#include "MyValArray.h"

#ifdef _WIN32

#include <windows.h>

class MySystemInfo {
public:
    MyStringW ComputerName;
    int       NumOfProcessors;
    MyStringA ProcsssorArchitecture;
    int       OSMajorVersion;
    int       OSMinorVersion;
    int       OSBuildNumber;
    int       OSPlatformId;
    MyStringW OSServicePack;
    int       OSServicePackMajor;
    int       OSServicePackMinor;
    int       OSSuiteMask;
    int       OSProductType;
    MyStringA OSProductTypeStr;
    MyStringA MachineId;
    
    MySystemInfo(): 
        NumOfProcessors(-1),
        OSMajorVersion(-1),
        OSMinorVersion(-1),
        OSBuildNumber(-1),
        OSPlatformId(-1),
        OSServicePackMajor(-1),
        OSServicePackMinor(-1),
        OSSuiteMask(-1),
        OSProductType(-1)
    {}

    // https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/ns-wdm-_osversioninfoexw
    // https://en.wikipedia.org/wiki/List_of_Microsoft_Windows_versions
    bool IsServerOS();
    bool IsWin11();
    bool IsWin10();
    bool IsWin81();
    bool IsWin8();
    bool IsWin7();
    bool IsVista();
    bool IsXP();
    bool IsWinServer2022();
    bool IsWinServer2019();
    bool IsWinServer2016();
    bool IsWinServer2012R2();
    bool IsWinServer2012();
    bool IsWinServer2008R2();
    bool IsWinServer2008();
    bool IsWinServer2003OrR2();
    bool IsWinServer2000();

};

// Base class that wrap all windows utils functions
class MyWin {
public:
    static int GetSysLastErrorCode();
    static int GetSysLastErrorMessage(MyStringA* msg, int errCode=0);

    static void Milliseconds2LargeIntergerTime(INT64 ms, LARGE_INTEGER* largeInteger);
    static LARGE_INTEGER LongLong2LargeInteger(const UINT64 val);
    static bool IsFilePlaceholder(LPCWSTR path);

    static HRESULT RetrieveWindowsSid(MyStringW& sid);

    static bool IsElevated();

    static void OpenExplorer(const wchar_t* path);
    
    static int MyShellExecute(HWND hwnd, LPCWSTR op, LPCWSTR file, LPCWSTR parameters, LPCWSTR directory, INT showCmd);
    static int MyShellExecuteEx(HWND hwnd, LPCWSTR op, LPCWSTR file, LPCWSTR parameters, LPCWSTR directory, INT showCmd, INT timeoutMS);

    static int MyShellGetCurrentExplorersFolders(MyArray<MyStringW>* dirs);
    
    static void MyGetSystemInfo(MySystemInfo* sysInfo);
    static void MyGetFileSystemName(const wchar_t* rootName, MyStringW* ret);
    static void MyGetFileSystemFlags(const wchar_t* rootName, DWORD* ret);
    
    static int MyGetAppDataFolder(const wchar_t* appName, MyStringW* ret);
    static int GetAppDataSubFolder(MyStringW* subFolder, MyStringW* subSub, bool createIfNotExist, MyStringW* ret, MyStringA* errMsg);
    static int EnsureFolderExist(MyStringW* path, MyStringA* errMsg);

    static bool SetAutoStartAtStartup(const wchar_t* name, const wchar_t* binaryPath, bool enable);
};

class MyWinRegValue {
public:
    DWORD type;
    DWORD valueLen;
    DWORD dwordValue;
    WCHAR strValue[2048];
    BYTE  binaryValue[2048];
};

class MyWinReg {

public:

    MyWinReg();
    ~MyWinReg();

    int Open(HKEY root, LPCWSTR subKey, REGSAM samDesired);
    int OpenExist(HKEY root, LPCWSTR subKey, REGSAM samDesired);
    int Close();
    int AddKeyValueDWORD(LPCWSTR key, DWORD value);
    int AddKeyValueExpandString(LPCWSTR key, LPCWSTR value, int valueLen);
    int AddKeyValueString(LPCWSTR key, LPCWSTR value, int valueLen);
    int GetValue(LPCWSTR key, MyWinRegValue* value);
    int GetSubKeyNames(MyArray<MyStringA>* subKeyNames);

    int QuickReadValue(HKEY root, LPCWSTR subKey, LPCWSTR keyName, MyWinRegValue* value);

    static BOOL KeyExists(HKEY rootKey, LPCWSTR subKey);
    static HRESULT SetKeyValue(HKEY rootKey, LPCWSTR subKey, LPCWSTR valueName, DWORD type, void* value);
    static BOOL DeleteKeyTree(HKEY rootKey, LPCWSTR subKey);
    static BOOL DeleteValue(HKEY rootKey, LPCWSTR subKey, LPCWSTR key);
    static HRESULT WalkSubKeys(HKEY rootKey, LPCWSTR subKey, void* context, HRESULT (*callback)(HKEY, MyStringW&, void*));
    static HRESULT GetKeyValue(HKEY rootKey, LPCWSTR subKey, LPCWSTR valueName, MyWinRegValue& value);

private:
    int HandleSystemError(HRESULT result);

private:
    HKEY m_HKey;
    MY_LAST_ERROR_DECL;
};

class MySingleInstance {
public:
    MySingleInstance(): m_Mutex(NULL) {}
    ~MySingleInstance() { Close(); }

    bool Open(const char* name);

private:
    void Close();

private:
    HANDLE m_Mutex;
};


class MyWinDeafultIcons {
public:
    virtual ~MyWinDeafultIcons();

    int Load();

    HICON GetFolderIcon();
    HICON GetUnknownIcon();
    HICON GetIcon(const char* ext);

    MyArray<MyStringA>* Extensions() { return &m_Exts; }
    MyArray<MyStringA>* IconPaths() { return &m_IconPaths; }

private:
    int FindExt(const char* ext);
    int ParseIconFileAndIndex(MyStringA* iconPath, MyStringW* file, int* index);

    int LoadDefaultIcon(const wchar_t* defaultIconKey, const wchar_t* ext);

private:
    MyArray<MyStringA> m_Exts;
    MyArray<MyStringA> m_IconPaths;
    MyValArray<HICON>  m_Icons; // item will be NULL if not loaded

    MY_LAST_ERROR_DECL;
};

#endif // _WIN32

#endif // _MY_WIN_H_
