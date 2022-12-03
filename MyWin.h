#ifndef _MY_WIN_H_
#define _MY_WIN_H_

#include "MyStringA.h"
#include "MyStringW.h"

#ifdef _WIN32

#include <windows.h>

// Base class that wrap all windows utils functions
class MyWin {
public:
    static int GetSysLastErrorCode();
    static int GetSysLastErrorMessage(MyStringA* msg, int errCode=0);

    static void Milliseconds2LargeIntergerTime(INT64 ms, LARGE_INTEGER* largeInteger);

    static HRESULT RetrieveWindowsSid(MyStringW& sid);
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

    static BOOL KeyExists(HKEY rootKey, LPCWSTR subKey);
    static HRESULT SetKeyValue(HKEY rootKey, LPCWSTR subKey, LPCWSTR valueName, DWORD type, void* value);
    static BOOL DeleteKeyTree(HKEY rootKey, LPCWSTR subKey);
    static HRESULT WalkSubKeys(HKEY rootKey, LPCWSTR subKey, void* context, HRESULT (*callback)(HKEY, MyStringW&, void*));
    static HRESULT GetKeyValue(HKEY rootKey, LPCWSTR subKey, LPCWSTR valueName, MyWinRegValue& value);

private:
    int HandleSystemError(HRESULT result);

private:
    HKEY m_HKey;
    MY_LAST_ERROR_DECL;
};


#endif // _WIN32

#endif // _MY_WIN_H_