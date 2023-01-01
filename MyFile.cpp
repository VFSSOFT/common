#ifndef MY_FILE_CPP_
#define MY_FILE_CPP_

#include "MyFile.h"

#ifdef WIN32

#include "MyWin.h"
#include "MyStringUtf16.h"

#include <fileapi.h>

MyFile::MyFile(): m_LastErrorCode(0), m_Handle(NULL) {

}
MyFile::~MyFile() {
  this->Close();
}

int MyFile::Open(const wchar_t* path, int creationDisp, int desiredAccess, int shareMode) {
  HANDLE h = OpenFileHandle(path, creationDisp, desiredAccess, shareMode);
  if (h == NULL)
    return m_LastErrorCode;

  m_Handle = h;
  m_Path.Set((const char*)path, wcslen(path) * sizeof(wchar_t));
  return 0;
}
int MyFile::Close() {
  if (m_Handle != NULL) {
    ::CloseHandle(m_Handle);
  }
  m_Handle = NULL;
  m_Path.Reset();
  return 0;
}

int MyFile::Length(UINT64* len) {
  LARGE_INTEGER fileSize;
  fileSize.QuadPart = 0;
  
  BOOL success = GetFileSizeEx(m_Handle, &fileSize);
  if (!success) {
    m_LastErrorCode = MyWin::GetSysLastErrorCode();
    MyWin::GetSysLastErrorMessage(&m_LastErrorMessage, m_LastErrorCode);
    return m_LastErrorCode;
  }
  *len = (UINT64)fileSize.QuadPart;
  return 0;
}

int MyFile::SetLength(UINT64 len) {
  // Set length will overwrite the currentPosition

  int errCode = Seek(len, MY_FILE_SEEK_TYPE_BEGIN);
  if (errCode < 0)
    return m_LastErrorCode;
  
  BOOL success = SetEndOfFile(m_Handle);
  if (!success) {
    m_LastErrorCode = MyWin::GetSysLastErrorCode();
    MyWin::GetSysLastErrorMessage(&m_LastErrorMessage, m_LastErrorCode);
    return m_LastErrorCode;
  }
  return 0;
}

UINT64 MyFile::CurrentPosition() {
  LONG distanceToMoveHigh = 0;
  DWORD distanceToMove = 0;

  distanceToMove = SetFilePointer(m_Handle, 0, &distanceToMoveHigh, FILE_CURRENT);
  if (distanceToMove == INVALID_SET_FILE_POINTER) {
    m_LastErrorCode = MyWin::GetSysLastErrorCode();
    MyWin::GetSysLastErrorMessage(&m_LastErrorMessage, m_LastErrorCode);
    return -1;
  }
  return ((UINT64)distanceToMoveHigh << 32L) | distanceToMove;
}

int MyFile::Seek(UINT64 pos, int seekType) {
  LONG distanceToMoveHigh = (pos & 0xFFFFFFFF00000000L) > 32;
  DWORD distanceToMove = pos & 0xFFFFFFFF;
  DWORD dwMoveMethod;

  if (seekType == MY_FILE_SEEK_TYPE_BEGIN) {
    dwMoveMethod = FILE_BEGIN;
  } else if (seekType == MY_FILE_SEEK_TYPE_CURRENT) {
    dwMoveMethod = FILE_CURRENT;
  }

  distanceToMove = SetFilePointer(m_Handle, distanceToMove, &distanceToMoveHigh, dwMoveMethod);
  if (distanceToMove == INVALID_SET_FILE_POINTER) {
    m_LastErrorCode = MyWin::GetSysLastErrorCode();
    MyWin::GetSysLastErrorMessage(&m_LastErrorMessage, m_LastErrorCode);
    return m_LastErrorCode;
  }
  // returns the position after seeking
  return ((UINT64)distanceToMoveHigh << 32L) | distanceToMove;
}
int MyFile::Write(const char* data, int len) {
  DWORD bytesWrite = 0;

  BOOL success = WriteFile(
                  m_Handle,
                  (LPCVOID)data,
                  len,
                  &bytesWrite,
                  NULL
                );
  if (!success) {
    m_LastErrorCode = MyWin::GetSysLastErrorCode();
    MyWin::GetSysLastErrorMessage(&m_LastErrorMessage, m_LastErrorCode);
    return m_LastErrorCode;
  }
  return 0;
}

int MyFile::Write(UINT64 pos, const char* data, int len) {
  int errCode = Seek(pos, MY_FILE_SEEK_TYPE_BEGIN);
  if (errCode < 0) return errCode;
  return Write(data, len);
}

int MyFile::Read(const char* data, int* len) {
  DWORD bytesRead = 0;

  BOOL success = ReadFile(
                  m_Handle,
                  (LPVOID)data,
                  *len,
                  &bytesRead,
                  NULL
                );
  if (!success) {
    m_LastErrorCode = MyWin::GetSysLastErrorCode();
    MyWin::GetSysLastErrorMessage(&m_LastErrorMessage, m_LastErrorCode);
    return m_LastErrorCode;
  }
  *len = bytesRead;
  return 0;
}

int MyFile::Read(UINT64 pos, const char* data, int* len) {
  int errCode = Seek(pos, MY_FILE_SEEK_TYPE_BEGIN);
  if (errCode < 0) return errCode;
  return Read(data, len);
}

UINT64 MyFile::FileAttributes(const wchar_t* path) {
  MyStringUtf16 wpath;
  wpath.Append((UCHAR*)L"\\\\?\\");
  wpath.Append((UCHAR*)path);

  DWORD dwAttr = GetFileAttributesW((LPCWSTR)wpath.Deref());
  if (dwAttr == INVALID_FILE_ATTRIBUTES) {
    return MyWin::GetSysLastErrorCode();
  }
  return dwAttr;
}

int MyFile::SetFileAttributes(const wchar_t* path, UINT64 attr) {
  MyStringUtf16 wpath;
  wpath.Append((UCHAR*)L"\\\\?\\");
  wpath.Append((UCHAR*)path);

  BOOL sucess = ::SetFileAttributesW((LPCWSTR)wpath.Deref(), (DWORD)attr);
  if (!sucess) {
    return MyWin::GetSysLastErrorCode();
  }
  return 0;
}

BOOL MyFile::FileExists(const wchar_t* path) {
  MyStringUtf16 wpath;
  wpath.Append((UCHAR*)L"\\\\?\\");
  wpath.Append((UCHAR*)path);

  DWORD dwAttr = GetFileAttributesW((LPCWSTR)wpath.Deref());

  return (dwAttr != INVALID_FILE_ATTRIBUTES && !(dwAttr & FILE_ATTRIBUTE_DIRECTORY));
}
bool MyFile::DirectoryExist(const wchar_t* path) {
    MyStringUtf16 wpath;
    wpath.Append((UCHAR*)L"\\\\?\\");
    wpath.Append((UCHAR*)path);

    DWORD dwAttr = GetFileAttributesW((LPCWSTR)wpath.Deref());
    return (dwAttr != INVALID_FILE_ATTRIBUTES && (dwAttr & FILE_ATTRIBUTE_DIRECTORY));
}
int MyFile::ListDirectory(const wchar_t* path, MyArray<MyStringW>* filenames, MyValArray<UINT64>* attrs) {
    int err = 0;
    HANDLE hFind;
    WIN32_FIND_DATA findFileData;
    MyStringW wpath;
    wpath.Set(path);
    wpath.Append(L"/*");

    filenames->Reset();
    attrs->Reset();

    hFind = FindFirstFile(
        wpath.Deref(),
        &findFileData
    );
    if (hFind == INVALID_HANDLE_VALUE) {
        m_LastErrorCode = MyWin::GetSysLastErrorCode();
        MyWin::GetSysLastErrorMessage(&m_LastErrorMessage, m_LastErrorCode);
        return m_LastErrorCode;
    }

    do {
        if (wcscmp(findFileData.cFileName, L".") == 0 || wcscmp(findFileData.cFileName, L"..") == 0) continue;
        MyStringW* name = filenames->AddNew();
        assert(name);
        if (err = name->Set(findFileData.cFileName, wcslen(findFileData.cFileName))) goto done;
        if (err = attrs->Add(findFileData.dwFileAttributes)) goto done;
    } while (FindNextFile(hFind, &findFileData) != 0);

done:
    FindClose(hFind);
    return err;
}
int MyFile::CreateDirectory(const wchar_t* path) {

    BOOL suc = ::CreateDirectoryW(path, NULL);
    if (!suc) {
        DWORD sysLastErr = GetLastError();
        if (sysLastErr != ERROR_ALREADY_EXISTS) {
            m_LastErrorCode = sysLastErr;
            MyWin::GetSysLastErrorMessage(&m_LastErrorMessage, sysLastErr);
            return m_LastErrorCode;
        }
    }
    return 0;
}

int MyFile::DeleteDirectory(const wchar_t* path) {
    BOOL suc = ::RemoveDirectoryW(path);
    if (!suc) {
        m_LastErrorCode = MyWin::GetSysLastErrorCode();
        MyWin::GetSysLastErrorMessage(&m_LastErrorMessage, m_LastErrorCode);
        return m_LastErrorCode;
    }
    return 0;
}
int MyFile::DeleteDirectoryRecursively(const wchar_t* path) {
    int err = 0;
    MyArray<MyStringW> filenames;
    MyValArray<UINT64> attrs;
    MyStringW curPath;

    if (err = ListDirectory(path, &filenames, &attrs)) return err;

    for (int i = 0; i < filenames.Size(); i++) {
        MyStringW* curFile = filenames.Get(i);

        curPath.Set(path);
        curPath.AppendChar(L'/');
        curPath.Append(curFile->Deref(), curFile->Length());

        if ((attrs.Get(i) & FILE_ATTRIBUTE_DIRECTORY) == 0) {
            if (err = this->DeleteFile(curPath.Deref())) return err;
        } else {
            if (err = DeleteDirectoryRecursively(curPath.Deref())) return err;
        }
    }

    if (err = DeleteDirectory(path)) return err;
    return 0;
}


int MyFile::DeleteFile() {
    int err = 0;
    if (Opened()) {
        if (err = Close()) return err;
    }
    if (err = this->DeleteFile((const wchar_t*)m_Path.Deref())) return err;
    return 0;
}
int MyFile::DeleteFile(const wchar_t* path) {
    BOOL suc = ::DeleteFileW(path);
    if (!suc) {
        m_LastErrorCode = MyWin::GetSysLastErrorCode();
        MyWin::GetSysLastErrorMessage(&m_LastErrorMessage, m_LastErrorCode);
        return m_LastErrorCode;
    }
    return 0;
}

int MyFile::Rename(const wchar_t* oldPath, const wchar_t* newPath, bool ignoreExistedError) {
    BOOL suc = ::MoveFileExW(oldPath, newPath, MOVEFILE_COPY_ALLOWED);
    if (!suc) {
        m_LastErrorCode = GetLastError();
        if (ignoreExistedError && m_LastErrorCode == ERROR_ALREADY_EXISTS) {
            m_LastErrorCode = 0;
            return 0;
        }
        MyWin::GetSysLastErrorMessage(&m_LastErrorMessage, m_LastErrorCode);
        return m_LastErrorCode;
    }
    return 0;
}

int MyFile::FullPath(const wchar_t* path, MyStringW* fullPath) {
    int err = 0;

    wchar_t pathBuf[32 * KB] = { 0 };

    DWORD usedLen = GetFullPathNameW(
        path,
        32 * KB,
        pathBuf,
        NULL
    );

    if (usedLen == 0) {
        m_LastErrorCode = GetLastError();
        MyWin::GetSysLastErrorMessage(&m_LastErrorMessage, m_LastErrorCode);
        return m_LastErrorCode;
    }

    fullPath->Set(pathBuf, usedLen);
    return 0;
}

void* MyFile::OpenFileHandle(const wchar_t* path, int creationDisp, int desiredAccess, int shareMode) {
  DWORD dwDesiredAccess = 0;
  DWORD dwShareMode = 0;
  DWORD dwCreationDisposition = 0;

  if ((desiredAccess & MY_FILE_DESIRED_ACCESS_READ) != 0) 
    dwDesiredAccess |= GENERIC_READ;
  if ((desiredAccess & MY_FILE_DESIRED_ACCESS_WRITE) != 0)
    dwDesiredAccess |= GENERIC_WRITE;

  if ((shareMode & MY_FILE_SHARE_MODE_SHARE_READ) != 0)
    dwShareMode = FILE_SHARE_READ;
  if ((shareMode & MY_FILE_SHARE_MODE_SHARE_WRITE) != 0)
    dwShareMode = FILE_SHARE_WRITE;
  if ((shareMode & MY_FILE_SHARE_MODE_SHARE_DELETE) != 0)
    dwShareMode = FILE_SHARE_DELETE;

  if ((creationDisp & MY_FILE_CREATION_DISP_CREATE_ALWAYS) != 0)
    dwCreationDisposition |= CREATE_ALWAYS;
  if ((creationDisp & MY_FILE_CREATION_DISP_CREATE_NEW) != 0)
    creationDisp |= CREATE_NEW;
  if ((creationDisp & MY_FILE_CREATION_DISP_OPEN_ALWAYS) != 0)
    dwCreationDisposition |= OPEN_ALWAYS;
  if ((creationDisp & MY_FILE_CREATION_DISP_OPEN_EXISTING) != 0)
    dwCreationDisposition |= OPEN_EXISTING;
  if ((creationDisp & MY_FILE_CREATION_DISP_CREATE_TRUNCATE_EXISTING) != 0)
    dwCreationDisposition |= TRUNCATE_EXISTING;

  HANDLE h = CreateFileW(
    path,
    dwDesiredAccess,
    shareMode,
    NULL,
    dwCreationDisposition,
    FILE_ATTRIBUTE_NORMAL,
    NULL
  );

  if (h == INVALID_HANDLE_VALUE) {
    h = NULL;
    m_LastErrorCode = MyWin::GetSysLastErrorCode();
    MyWin::GetSysLastErrorMessage(&m_LastErrorMessage, m_LastErrorCode);
  }
  return h;
}

int MyFile::ReadAllBytes(const wchar_t* path, MyBuffer* data) {
    int err = 0;
    UINT64 fileSize = 0;
    int bytesRead = 0;
    MyBuffer tmpBuf;
    data->Reset();

    err = this->Open(
        path,
        MY_FILE_CREATION_DISP_OPEN_EXISTING,
        MY_FILE_DESIRED_ACCESS_READ,
        MY_FILE_SHARE_MODE_SHARE_READ
    );
    if (err) return err;

    if (err = this->Length(&fileSize)) return err;
    tmpBuf.SetLength(1024 * 1024);
    while (data->Length() < fileSize) {
        bytesRead = tmpBuf.Length();
        if (err = this->Read(tmpBuf.Deref(), &bytesRead)) return err;
        if (bytesRead > 0) {
            data->Append(tmpBuf.Deref(), bytesRead);
        } else {
            err = bytesRead;
            break;
        }
    }

    this->Close();
    return err;
}

#endif


#endif // MY_FILE_CPP_