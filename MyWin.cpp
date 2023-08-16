#ifndef _MY_WIN_CPP_
#define _MY_WIN_CPP_

#ifdef _WIN32

#include "MyWin.h"
#include "MyFile.h"

#include "sddl.h"
#include "shlobj_core.h"

bool MySystemInfo::IsServerOS() {
    return OSProductType != VER_NT_WORKSTATION;
}
bool MySystemInfo::IsWin11() {
    return OSMajorVersion == 10 && OSMinorVersion == 0 && OSProductType == VER_NT_WORKSTATION;
}
bool MySystemInfo::IsWin10() {
    return OSMajorVersion == 10 && OSMinorVersion == 0 && OSProductType == VER_NT_WORKSTATION;
}
bool MySystemInfo::IsWin81() {
    return OSMajorVersion == 6 && OSMinorVersion == 3 && OSProductType == VER_NT_WORKSTATION;
}
bool MySystemInfo::IsWin8() {
    return OSMajorVersion == 6 && OSMinorVersion == 2 && OSProductType == VER_NT_WORKSTATION;
}
bool MySystemInfo::IsWin7() {
    return OSMajorVersion == 6 && OSMinorVersion == 1 && OSProductType == VER_NT_WORKSTATION;
}
bool MySystemInfo::IsVista() {
    return OSMajorVersion == 6 && OSMinorVersion == 0 && OSProductType == VER_NT_WORKSTATION;
}
bool MySystemInfo::IsXP() {
    return OSMajorVersion == 5 && OSMinorVersion == 2;
}
bool MySystemInfo::IsWinServer2022() {
    return OSMajorVersion == 10 && OSMinorVersion == 0 && OSProductType != VER_NT_WORKSTATION;
}
bool MySystemInfo::IsWinServer2019() {
    return OSMajorVersion == 10 && OSMinorVersion == 0 && OSProductType != VER_NT_WORKSTATION;
}

bool MySystemInfo::IsWinServer2016() {
    return OSMajorVersion == 10 && OSMinorVersion == 0 && OSProductType != VER_NT_WORKSTATION;
}
bool MySystemInfo::IsWinServer2012R2() {
    return OSMajorVersion == 6 && OSMinorVersion == 3 && OSProductType != VER_NT_WORKSTATION;
}
bool MySystemInfo::IsWinServer2012() {
    return OSMajorVersion == 6 && OSMinorVersion == 2 && OSProductType != VER_NT_WORKSTATION;
}
bool MySystemInfo::IsWinServer2008R2() {
    return OSMajorVersion == 6 && OSMinorVersion == 1 && OSProductType != VER_NT_WORKSTATION;
}
bool MySystemInfo::IsWinServer2008() {
    return OSMajorVersion == 6 && OSMinorVersion == 0 && OSProductType != VER_NT_WORKSTATION;
}
bool MySystemInfo::IsWinServer2003OrR2() {
    return OSMajorVersion == 5 && OSMinorVersion == 2;
}
bool MySystemInfo::IsWinServer2000() {
    return OSMajorVersion == 5 && OSMinorVersion == 0;
}

int MyWin::GetSysLastErrorCode() {
  return (int)::GetLastError();
}
int MyWin::GetSysLastErrorMessage(MyStringA* msg, int errCode) {
  if (errCode == 0) {
    errCode = GetSysLastErrorCode();
  }

  char buf[1024];
  size_t size = FormatMessageA(
    FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
    NULL,
    (DWORD)errCode,
    MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
    (LPSTR)buf,
    (sizeof(buf) / sizeof(char)),
    NULL
  );

  return msg->Set(buf, (int)size);
}

void MyWin::Milliseconds2LargeIntergerTime(INT64 ms, LARGE_INTEGER* largeInteger) {
  largeInteger->QuadPart = ms * 10000;
}
LARGE_INTEGER MyWin::LongLong2LargeInteger(const UINT64 val) {
    LARGE_INTEGER largeInteger;
    largeInteger.QuadPart = val;
    return largeInteger;
}

bool MyWin::IsFilePlaceholder(LPCWSTR path) {
    // ref: https://learn.microsoft.com/en-us/windows/win32/w8cookbook/placeholder-files
    bool isPlaceholder = false;
    WIN32_FIND_DATA findData;
    HANDLE hFind = FindFirstFile(path, &findData);
    if (hFind != INVALID_HANDLE_VALUE) {
        isPlaceholder = (findData.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT) &&
                        (findData.dwReserved0 == IO_REPARSE_TAG_FILE_PLACEHOLDER);
        FindClose(hFind);
    }
    return isPlaceholder;
}


HRESULT MyWin::RetrieveWindowsSid(MyStringW &sid) {
  
    int         err = 0;
    HANDLE      tokenHandle;
    TOKEN_USER* tokenInfo = NULL;
    DWORD       tokenInfoSize = 0;
    BOOL        getTokenInfoResult;
    LPWSTR      stringSid = NULL;

    tokenHandle = GetCurrentThreadEffectiveToken();

    getTokenInfoResult = ::GetTokenInformation(tokenHandle, TokenUser, NULL, 0, &tokenInfoSize);
    if (!getTokenInfoResult) {
        if (GetLastError() != ERROR_INSUFFICIENT_BUFFER) {
            err = GetLastError();
            goto done;
        }
    }

    tokenInfo = (TOKEN_USER*)malloc(tokenInfoSize);
    getTokenInfoResult = ::GetTokenInformation(tokenHandle, TokenUser, tokenInfo, tokenInfoSize, &tokenInfoSize);
    if (!getTokenInfoResult) {
        err = GetLastError();
        goto done;
    }

    if (!ConvertSidToStringSid(tokenInfo->User.Sid, &stringSid)) {
        err = GetLastError();
        goto done;
    }

    sid.Set(stringSid);

done:
    if (tokenInfo != NULL) free(tokenInfo);
    if (stringSid != NULL) LocalFree(stringSid);

    return err;
}

int MyWin::MyShellExecute(HWND hwnd, LPCWSTR op, LPCWSTR file, LPCWSTR parameters, LPCWSTR directory, INT showCmd) {
    int ret = (int) ShellExecute(
        hwnd,
        op,
        file,
        parameters,
        directory,
        showCmd
    );
    if (ret > 32) {
        return 0;
    }
    return ret;
}
int MyWin::MyShellExecuteEx(HWND hwnd, LPCWSTR op, LPCWSTR file, LPCWSTR parameters, LPCWSTR directory, INT showCmd, INT timeoutMS) {
    int err = 0;

    SHELLEXECUTEINFO info;
    memset(&info, 0, sizeof(SHELLEXECUTEINFO));
    info.cbSize = sizeof(SHELLEXECUTEINFO);
    info.fMask = SEE_MASK_NOCLOSEPROCESS;
    info.hwnd = hwnd;
    info.lpVerb = op;
    info.lpFile = file;
    info.lpParameters = parameters;
    info.nShow = showCmd;
    info.hInstApp = NULL;
    BOOL suc = ShellExecuteEx(&info);
    if (!suc) {
        err = ::GetLastError();
        return err;
    }

    DWORD waitRet = WaitForSingleObject(info.hProcess, timeoutMS);
    if (waitRet == WAIT_OBJECT_0) {
        DWORD exitCode = (DWORD)(info.hInstApp);
        if (exitCode < 32) return exitCode;
        GetExitCodeProcess(info.hProcess, &exitCode);
        return exitCode;
    } else if (waitRet == WAIT_TIMEOUT) {
        return -1;
    } else if (waitRet == WAIT_FAILED) {
        return -1;
    } else {
        return -1;
    }
}

void MyWin::MyGetSystemInfo(MySystemInfo* sysInfo) {
    WCHAR computerNameBuffer[MAX_COMPUTERNAME_LENGTH + 1];
    DWORD computerNameBufferLen = MAX_COMPUTERNAME_LENGTH + 1;
    
    if (GetComputerName(computerNameBuffer, &computerNameBufferLen)) {
        sysInfo->ComputerName.Set(computerNameBuffer, computerNameBufferLen);
    } else {
        sysInfo->ComputerName.Set(L"Unknown");
    }
    
    SYSTEM_INFO si;
    GetSystemInfo(&si);
    
    sysInfo->NumOfProcessors = (int) si.dwNumberOfProcessors;
    
    switch (si.wProcessorArchitecture) {
    case PROCESSOR_ARCHITECTURE_AMD64:
        sysInfo->ProcsssorArchitecture.Set("x64 (AMD or Intel)");
        break;
    case PROCESSOR_ARCHITECTURE_ARM:
        sysInfo->ProcsssorArchitecture.Set("ARM");
        break;
    case PROCESSOR_ARCHITECTURE_ARM64:
        sysInfo->ProcsssorArchitecture.Set("ARM64");
        break;
    case PROCESSOR_ARCHITECTURE_IA64:
        sysInfo->ProcsssorArchitecture.Set("Intel Itanium-based");
        break;
    case PROCESSOR_ARCHITECTURE_INTEL:
        sysInfo->ProcsssorArchitecture.Set("x86");
        break;
    case PROCESSOR_ARCHITECTURE_UNKNOWN:
        sysInfo->ProcsssorArchitecture.Set("Unknown architecture");
        break;
    default:
        sysInfo->ProcsssorArchitecture.SetWithFormat("Unkown architecture: %d", (int)si.wProcessorArchitecture);
        break;
    }
    
    NTSTATUS(WINAPI *RtlGetVersion)(LPOSVERSIONINFOEXW);
    OSVERSIONINFOEX osInfo;
    
    *(FARPROC*)&RtlGetVersion = GetProcAddress(GetModuleHandleA("ntdll"), "RtlGetVersion");
    assert(RtlGetVersion != NULL);
    
    osInfo.dwOSVersionInfoSize = sizeof(osInfo);
    RtlGetVersion(&osInfo);
    
    sysInfo->OSMajorVersion = osInfo.dwMajorVersion;
    sysInfo->OSMinorVersion = osInfo.dwMinorVersion;
    sysInfo->OSBuildNumber  = osInfo.dwBuildNumber;
    sysInfo->OSPlatformId   = osInfo.dwPlatformId;
    sysInfo->OSServicePack.Set(osInfo.szCSDVersion);
    sysInfo->OSServicePackMajor = osInfo.wServicePackMajor;
    sysInfo->OSServicePackMinor = osInfo.wServicePackMinor;
    sysInfo->OSSuiteMask        = osInfo.wSuiteMask;
    sysInfo->OSProductType      = osInfo.wProductType;

    switch (sysInfo->OSProductType) {
    case VER_NT_WORKSTATION: 
        sysInfo->OSProductTypeStr.Set("WORKSTATION");
        break;
    case VER_NT_DOMAIN_CONTROLLER:
        sysInfo->OSProductTypeStr.Set("DOMAIN CONTROLLER");
        break;
    case VER_NT_SERVER:
        sysInfo->OSProductTypeStr.Set("SERVER");
        break;
    default:
        sysInfo->OSProductTypeStr.Set("Unkonwn Product Type");
        break;
    }
    
    MyWinRegValue machineIdRegValue;
    MyWinReg::GetKeyValue(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\SQMClient", L"MachineId", machineIdRegValue);
    sysInfo->MachineId.SetUnicode(machineIdRegValue.strValue + 1, wcslen(machineIdRegValue.strValue) - 2);
}

void MyWin::MyGetFileSystemName(const wchar_t* rootName, MyStringW* ret) {
    wchar_t buf[MAX_PATH+1];
    BOOL suc = GetVolumeInformation(
        rootName,
        NULL, 0,
        NULL,
        NULL,
        NULL,
        buf, MAX_PATH
    );
    if (suc) {
        ret->Set(buf);
    }
}
void MyWin::MyGetFileSystemFlags(const wchar_t* rootName, DWORD* ret) {
     GetVolumeInformation(
        rootName,
        NULL, 0,
        NULL,
        NULL,
        ret,
        NULL, 0
    );
}


int MyWin::MyGetAppDataFolder(const wchar_t* appName, MyStringW* ret) {
    wchar_t path[MAX_PATH];
    HRESULT result = SHGetFolderPath(NULL, CSIDL_COMMON_APPDATA, NULL, 0, path);
    if (SUCCEEDED(result)) {
        ret->Set(path);
        if (appName != NULL && wcslen(appName) > 0) {
            MyStringW newPath;
            MyStringW::JoinPath(ret->Deref(), ret->Length(), appName, wcslen(appName), &newPath);
            ret->Set(newPath.Deref(), newPath.Length());
        }
        return 0;
    }
    return result;
}
int MyWin::GetAppDataSubFolder(MyStringW* subFolder, MyStringW* subSub, bool createIfNotExist, MyStringW* ret, MyStringA* errMsg) {
    int err = 0;
    MyStringW tmp;

    MyGetAppDataFolder(subFolder->Deref(), ret);
    if (createIfNotExist) {
        if (err = EnsureFolderExist(ret, errMsg)) {
            return err;
        }
    }
    if (subSub == NULL || subSub->Length() == 0) {
        return err;
    }
    tmp.Set(ret->Deref(), ret->Length());
    MyStringW::JoinPath(tmp.Deref(), tmp.Length(), subSub->Deref(), subSub->Length(), ret);
    if (createIfNotExist) {
        if (err = EnsureFolderExist(ret, errMsg)) {
            return err;
        }
    }
    return 0;
}
int MyWin::EnsureFolderExist(MyStringW* path, MyStringA* errMsg) {
    int err = 0;
    if (!MyFile::DirectoryExist(path->Deref())) {
        MyFile f;
        if (err = f.CreateDirectory(path->Deref())) {
            if (errMsg) {
                errMsg->Set(f.LastErrorMessage());
            }
            return err;
        }
    }
    return 0;
}

bool MyWin::SetAutoStartAtStartup(const wchar_t* name, const wchar_t* binaryPath, bool enable) {
    if (enable) {
        MyWinReg reg;
        int err = reg.Open(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Run", KEY_ALL_ACCESS);
        if (err) return false;
        err = reg.AddKeyValueString(name, binaryPath, wcslen(binaryPath));
        reg.Close();
        return err == 0;
    } else {
        MyStringW key;
        key.Set(L"Software\\Microsoft\\Windows\\CurrentVersion\\Run\\");
        key.Append(name);

        if (!MyWinReg::KeyExists(HKEY_CURRENT_USER, key.Deref())) {
            return TRUE; // doesn't exist, consider it as delete successfully
        }

        return MyWinReg::DeleteValue(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Run", name) != 0;
    }
}

MyWinReg::MyWinReg() {
    INIT_LAST_ERROR;
    m_HKey = NULL;
}
MyWinReg::~MyWinReg() {
    Close();
}

int MyWinReg::Open(HKEY root, LPCWSTR subKey, REGSAM samDesired) {
    HKEY hKey;
    HRESULT result = RegCreateKeyEx(
        root,
        subKey,
        0,
        NULL,
        0,
        samDesired,
        NULL, 
        &hKey,
        NULL
    );

    if (result != ERROR_SUCCESS) return HandleSystemError(result);

    m_HKey = hKey;
    return 0;
}
int MyWinReg::OpenExist(HKEY root, LPCWSTR subKey, REGSAM samDesired) {
    HKEY hKey;
    HRESULT result = RegOpenKeyEx(
        root,
        subKey,
        0,
        samDesired,
        &hKey
    );

    if (result != ERROR_SUCCESS) return HandleSystemError(result);

    m_HKey = hKey;
    return 0;
}
int MyWinReg::Close() {
    if (m_HKey != NULL) {
        RegCloseKey(m_HKey);
        m_HKey = NULL;
    }
    return 0;
}

int MyWinReg::AddKeyValueDWORD(LPCWSTR key, DWORD value) {
    HRESULT result = RegSetValueEx(
        m_HKey,
        key,
        0,
        REG_DWORD,
        (const BYTE*)&value,
        sizeof(value)
    );

    if (result != ERROR_SUCCESS) return HandleSystemError(result);
    return 0;
}

int MyWinReg::AddKeyValueExpandString(LPCWSTR key, LPCWSTR value, int valueLen) {
    HRESULT result = RegSetValueEx(
        m_HKey,
        key,
        0,
        REG_EXPAND_SZ,
        (const BYTE*)value,
        sizeof(wchar_t) * (valueLen + 1)
    );

    if (result != ERROR_SUCCESS) return HandleSystemError(result);
    return 0;
}

int MyWinReg::AddKeyValueString(LPCWSTR key, LPCWSTR value, int valueLen) {
    HRESULT result = RegSetValueEx(
        m_HKey,
        key,
        0,
        REG_SZ,
        (const BYTE*)value,
        sizeof(wchar_t) * (valueLen + 1)
    );

    if (result != ERROR_SUCCESS) return HandleSystemError(result);
    return 0;
}

int MyWinReg::GetValue(LPCWSTR key, MyWinRegValue* value) {

    DWORD type = 0, sizeInBytes = 0;
    HRESULT result = RegQueryValueEx(
        m_HKey,
        key,
        0,
        &type,
        NULL,
        &sizeInBytes
    );
    if (result != ERROR_SUCCESS) return HandleSystemError(result);

    switch (type) {

    case REG_DWORD: {
        result = RegQueryValueEx(
            m_HKey,
            key,
            0,
            &type,
            (LPBYTE)(&value->dwordValue),
            &sizeInBytes
        );

        if (result != ERROR_SUCCESS) return HandleSystemError(result);
        value->valueLen = sizeInBytes;
        break;
    }

    case REG_EXPAND_SZ:
    case REG_SZ: {
        result = RegQueryValueEx(
            m_HKey,
            key,
            0,
            &type,
            (LPBYTE)(value->strValue),
            &sizeInBytes
        );
        if (result != ERROR_SUCCESS) return HandleSystemError(result);

        value->valueLen = sizeInBytes / sizeof(WCHAR);
        // From the doc:
        // If the data has the REG_SZ, REG_MULTI_SZ or REG_EXPAND_SZ type, the string may not have been stored with
        // the proper terminating null characters. Therefore, even if the function returns ERROR_SUCCESS,
        // the application should ensure that the string is properly terminated before using it; otherwise, it may overwrite a buffer.
        if (value->strValue[value->valueLen - 1] == 0) {
            value->valueLen--;
        }
        break;
    }

    case REG_BINARY: {
        result = RegQueryValueEx(
            m_HKey,
            key,
            0,
            &type,
            (LPBYTE)value->binaryValue,
            &sizeInBytes
        );
        if (result != ERROR_SUCCESS) return HandleSystemError(result);
        value->valueLen = sizeInBytes;
        break;
    }

    default:
        break;
    }

    return 0;
}


int MyWinReg::HandleSystemError(HRESULT result) {
    m_LastErrorCode = result;
    MyWin::GetSysLastErrorMessage(&m_LastErrorMessage, m_LastErrorCode);
    return m_LastErrorCode;
}


BOOL MyWinReg::KeyExists(HKEY rootKey, LPCWSTR subKey) {
  HKEY hKey;
  LONG result;

  result = RegOpenKeyEx(rootKey, subKey, 0, KEY_READ | KEY_WOW64_64KEY, &hKey);
  RegCloseKey(hKey);
  
  return result != ERROR_FILE_NOT_FOUND;
}

HRESULT MyWinReg::SetKeyValue(HKEY rootKey, LPCWSTR subKey, LPCWSTR valueName, DWORD type, void* value) {

  // KEY_WOW64_64KEY is necessary because CLSIDs are "Redirected and reflected only for CLSIDs that do not specify InprocServer32 or InprocHandler32."
  // https://msdn.microsoft.com/en-us/library/windows/desktop/aa384253%28v=vs.85%29.aspx#redirected__shared__and_reflected_keys_under_wow64
  // This shouldn't be an issue in our case since we use shell32.dll as InprocServer32, so we could write those registry keys for both 32 and 64bit.
  // FIXME: Not doing so at the moment means that explorer will show the cloud provider, but 32bit processes' open dialogs (like the ownCloud client itself) won't show it.

  HRESULT result;
  HKEY hKey;

  result = RegCreateKeyEx(rootKey, subKey, 0, NULL, 0, KEY_WRITE | KEY_WOW64_64KEY, NULL, &hKey, NULL);
  if (result != ERROR_SUCCESS) {
    return result;
  }

  result = E_FAIL;
  switch (type) {

  case REG_DWORD: {
    result = RegSetValueEx(hKey, valueName, 0, type, (const BYTE*)&value, sizeof(DWORD));
    break;
  }

  case REG_EXPAND_SZ:
  case REG_SZ: {
    result = RegSetValueEx(hKey, valueName, 0, type, (const BYTE*)value, (wcslen((WCHAR*)value) + 1) * sizeof(wchar_t));
    break;
  }

  default: // shouldn't reach here
    break;  
  }

  RegCloseKey(hKey);

  return result;

}

BOOL MyWinReg::DeleteKeyTree(HKEY rootKey, LPCWSTR subKey) {
  HRESULT result = 0;
  HKEY hKey;
  REGSAM samDesired = DELETE | KEY_ENUMERATE_SUB_KEYS | KEY_QUERY_VALUE | KEY_SET_VALUE | KEY_WOW64_64KEY;

  result = RegOpenKeyEx(rootKey, subKey, 0, samDesired, &hKey);
  if (result != ERROR_SUCCESS) {
    return FALSE;
  }

  result = RegDeleteTree(hKey, NULL);
  RegCloseKey(hKey);

  result |= RegDeleteKeyEx(rootKey, subKey, samDesired, 0);

  return result == ERROR_SUCCESS;
}

BOOL MyWinReg::DeleteValue(HKEY rootKey, LPCWSTR subKey, LPCWSTR key) {
    HRESULT result = 0;
    HKEY hKey;
    REGSAM samDesired = DELETE | KEY_ENUMERATE_SUB_KEYS | KEY_QUERY_VALUE | KEY_SET_VALUE | KEY_WOW64_64KEY;

    result = RegOpenKeyEx(rootKey, subKey, 0, samDesired, &hKey);
    if (result != ERROR_SUCCESS) {
        return FALSE;
    }

    result = RegDeleteValue(hKey, key);

    RegCloseKey(hKey);

    return result == ERROR_SUCCESS;
}

HRESULT MyWinReg::WalkSubKeys(HKEY rootKey, LPCWSTR subKey, void* context, HRESULT (*callback)(HKEY, MyStringW&, void*)) {
  HRESULT result;
  HKEY    hKey;
  REGSAM  samDesired = KEY_READ | KEY_WOW64_64KEY;
  DWORD   maxSubKeyNameSize = 0;
  MyStringW subKeyName;
  DWORD retCode = ERROR_SUCCESS;
  DWORD subKeyNameSize = 0;

  result = RegOpenKeyEx(rootKey, subKey, 0, samDesired, &hKey);
  if (result != ERROR_SUCCESS) {
    return result;
  }

  result = RegQueryInfoKey(hKey, NULL, NULL, NULL, NULL, &maxSubKeyNameSize, NULL, NULL, NULL, NULL, NULL, NULL);
  if (result != ERROR_SUCCESS) {
    RegCloseKey(hKey);
    return result;
  }

  subKeyName.SetLength(maxSubKeyNameSize + 1);
  for (DWORD i = 0; retCode == ERROR_SUCCESS; ++i) {
    subKeyNameSize = maxSubKeyNameSize;
    retCode = RegEnumKeyEx(hKey, i, subKeyName.Deref(), &subKeyNameSize, NULL, NULL, NULL, NULL);
    if (retCode == ERROR_SUCCESS) {
      // subKeyNameSize excludes the trailing \0
      subKeyName.SetLength(subKeyNameSize);
      // Pass only the sub keyname, not the full path.
      result = callback(hKey, subKeyName, context);
      if (result != 0) { break; }
    }
  }

  RegCloseKey(hKey);

  return result;

}

HRESULT MyWinReg::GetKeyValue(HKEY rootKey, LPCWSTR subKey, LPCWSTR valueName, MyWinRegValue& value) {
  HRESULT result;
  HKEY    hKey;
  REGSAM samDesired = KEY_READ | KEY_WOW64_64KEY;
  DWORD type = 0, sizeInBytes = 0;

  result = RegOpenKeyEx(rootKey, subKey, 0, samDesired, &hKey);
  if (result != ERROR_SUCCESS) {
    return result;
  }
   
  result = RegQueryValueEx(hKey, valueName, 0, &type, nullptr, &sizeInBytes);

  if (result == ERROR_SUCCESS) {
    switch (type) {
    case REG_DWORD:
      result = RegQueryValueEx(hKey, valueName, 0, &type, (LPBYTE)(&value.dwordValue), &sizeInBytes);
      if (result == ERROR_SUCCESS) {
        value.valueLen = sizeInBytes;
      }
      break;
    case REG_EXPAND_SZ:
    case REG_SZ: {
      result = RegQueryValueEx(hKey, valueName, 0, &type, (LPBYTE)(value.strValue), &sizeInBytes);
      if (result == ERROR_SUCCESS) {
        int newCharSize = sizeInBytes / sizeof(WCHAR);
        // From the doc:
        // If the data has the REG_SZ, REG_MULTI_SZ or REG_EXPAND_SZ type, the string may not have been stored with
        // the proper terminating null characters. Therefore, even if the function returns ERROR_SUCCESS,
        // the application should ensure that the string is properly terminated before using it; otherwise, it may overwrite a buffer.
        if (value.strValue[newCharSize - 1] == 0) {
          newCharSize --;
        }
        value.valueLen = newCharSize;
      }
      break;
    }
    case REG_BINARY: {
      result = RegQueryValueEx(hKey, valueName, 0, &type, (LPBYTE)value.binaryValue, &sizeInBytes);
      if (result == ERROR_SUCCESS) {
          value.valueLen = sizeInBytes;
      }
      break;
    }
    default:
        break;
    }
    }

    RegCloseKey(hKey);
    return result;
}





#endif // _WIN32

#endif // _MY_WIN_CPP_
