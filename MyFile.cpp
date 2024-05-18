#ifndef MY_FILE_CPP_
#define MY_FILE_CPP_

#include "MyFile.h"

#ifdef WIN32

#include "MyWin.h"
#include "MyStringUtf16.h"

#include <fileapi.h>
#include <Accctrl.h>
#include <aclapi.h>

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
  m_Path.Set(path, wcslen(path));
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
      return HandleFSError(m_Path.Deref());
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
      return HandleFSError(m_Path.Deref());
  }
  return 0;
}

UINT64 MyFile::CurrentPosition() {
  LONG distanceToMoveHigh = 0;
  DWORD distanceToMove = 0;

  distanceToMove = SetFilePointer(m_Handle, 0, &distanceToMoveHigh, FILE_CURRENT);
  if (distanceToMove == INVALID_SET_FILE_POINTER) {
      HandleFSError(m_Path.Deref());
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
  } else if (seekType == MY_FILE_SEEK_TYPE_END) {
      dwMoveMethod = FILE_END;
  } else {
      assert(0);
  }

  distanceToMove = SetFilePointer(m_Handle, distanceToMove, &distanceToMoveHigh, dwMoveMethod);
  if (distanceToMove == INVALID_SET_FILE_POINTER) {
        return HandleFSError(m_Path.Deref());
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
      return HandleFSError(m_Path.Deref());
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
    return HandleFSError(m_Path.Deref());
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
  DWORD dwAttr = GetFileAttributesW(path);
  if (dwAttr == INVALID_FILE_ATTRIBUTES) {
      return HandleFSError(path);
  }
  return dwAttr;
}

int MyFile::SetFileAttributes(const wchar_t* path, UINT64 attr) {
  BOOL sucess = ::SetFileAttributesW(path, (DWORD)attr);
  if (!sucess) {
    return HandleFSError(path);
  }
  return 0;
}

BOOL MyFile::FileExists(const wchar_t* path) {
  DWORD dwAttr = GetFileAttributesW(path);

  return (dwAttr != INVALID_FILE_ATTRIBUTES && !(dwAttr & FILE_ATTRIBUTE_DIRECTORY));
}
bool MyFile::DirectoryExist(const wchar_t* path) {
    DWORD dwAttr = GetFileAttributesW(path);
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
        return HandleFSError(path);
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
            return HandleFSError(path);
        }
    }
    return 0;
}

int MyFile::DeleteDirectory(const wchar_t* path) {
    BOOL suc = ::RemoveDirectoryW(path);
    if (!suc) {
        return HandleFSError(path);
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
    MyStringW path;
    path.Set(m_Path.Deref());

    if (Opened()) {
        if (err = Close()) return err;
    }
    if (err = this->DeleteFile((const wchar_t*)path.Deref())) return err;
    return 0;
}
int MyFile::DeleteFile(const wchar_t* path) {
    BOOL suc = ::DeleteFileW(path);
    if (!suc) {
        return HandleFSError(path);
    }
    return 0;
}

int MyFile::Rename(const wchar_t* oldPath, const wchar_t* newPath) {
    BOOL suc = ::MoveFileExW(oldPath, newPath, MOVEFILE_COPY_ALLOWED);
    if (!suc) {
        return HandleFSError(oldPath);
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
        return HandleFSError(path);
    }

    fullPath->Set(pathBuf, usedLen);
    return 0;
}

int MyFile::GetTemporaryPath(MyStringW* path) {
    wchar_t temp[MAX_PATH];
    DWORD len = GetTempPathW(MAX_PATH, temp);
    if (len == 0) {
        return HandleFSError(L"TemporaryPath");
    } else {
        path->Set(temp, len);
    }
    return 0;
}
int MyFile::GetTemporaryFile(const wchar_t* path, MyStringW* filepath) {
    wchar_t temp[MAX_PATH];
    DWORD ret = GetTempFileNameW(path, L"TMP", MAX_PATH, temp);
    if (ret == 0) {
        return HandleFSError(L"TemporaryFile");
    } else {
        filepath->Set(temp);
    }
    return 0;
}
int MyFile::OpenTempFile() {
    int err = 0;
    MyStringW tmpPath, tmpFile;
    
    if (err = GetTemporaryPath(&tmpPath)) return err;
    if (err = GetTemporaryFile(tmpPath.Deref(), &tmpFile)) return err;

    return Open(tmpFile.Deref(), MY_FILE_CREATION_DISP_CREATE_ALWAYS);
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
    dwShareMode |= FILE_SHARE_READ;
  if ((shareMode & MY_FILE_SHARE_MODE_SHARE_WRITE) != 0)
    dwShareMode |= FILE_SHARE_WRITE;
  if ((shareMode & MY_FILE_SHARE_MODE_SHARE_DELETE) != 0)
    dwShareMode |= FILE_SHARE_DELETE;

  if ((creationDisp & MY_FILE_CREATION_DISP_CREATE_ALWAYS) != 0)
    dwCreationDisposition |= CREATE_ALWAYS;
  if ((creationDisp & MY_FILE_CREATION_DISP_CREATE_NEW) != 0)
    dwCreationDisposition |= CREATE_NEW;
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
    HandleFSError(path);
  }
  return h;
}
int MyFile::HandleFSError(const wchar_t* path) {
    m_LastErrorCode = GetLastError();
    MyWin::GetSysLastErrorMessage(&m_LastErrorMessage, m_LastErrorCode);

    MyStringA utf8Path;
    utf8Path.SetUnicode(path, wcslen(path));

    m_LastErrorMessage.Append("(\"", 2);
    m_LastErrorMessage.Append(utf8Path.Deref());
    m_LastErrorMessage.Append("\")", 2);

    return m_LastErrorCode;
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

int MyFile::MyGetDiskFreeSpace(const wchar_t* path, UINT64* freeBytesAvailableToCaller, UINT64* totalNumberOfBytes, UINT64* totalNumberOfFreeBytes) {
    BOOL result;
    ULARGE_INTEGER freeBytesToCaller;
    ULARGE_INTEGER totalBytes;
    ULARGE_INTEGER totalFreeBytes;

    result = GetDiskFreeSpaceEx(path, &freeBytesToCaller, &totalBytes, &totalFreeBytes);
    if (!result) {
        return HandleFSError(path);
    }

    if (freeBytesAvailableToCaller) *freeBytesAvailableToCaller = freeBytesToCaller.QuadPart;
    if (totalNumberOfBytes) *totalNumberOfBytes = totalBytes.QuadPart;
    if (totalNumberOfFreeBytes) *totalNumberOfFreeBytes = totalFreeBytes.QuadPart;
    return 0;
}

int MyFile::GrantEveryoneRWPermission(const wchar_t* path) {
    int err = 0;
    PSID everyoneSID = NULL;
    PACL acl = NULL;
    EXPLICIT_ACCESS ea[1];
    SID_IDENTIFIER_AUTHORITY sidAuthWorld = SECURITY_WORLD_SID_AUTHORITY;
    PSECURITY_DESCRIPTOR sd = NULL;

    // Create a well-known sid for the everyone group
    BOOL ret = AllocateAndInitializeSid(
        &sidAuthWorld,
        1, 
        SECURITY_WORLD_RID,
        0, 0, 0, 0, 0, 0, 0,
        &everyoneSID
    );
    if (!ret) {
        err = HandleFSError(path);
        goto done;
    }

    // Initialize an EXPLICIT_ACCESS structure for an ACE.
    ZeroMemory(&ea, 1 * sizeof(EXPLICIT_ACCESS));
    ea[0].grfAccessPermissions = SPECIFIC_RIGHTS_ALL | STANDARD_RIGHTS_ALL;
    ea[0].grfAccessMode = GRANT_ACCESS;
    ea[0].grfInheritance = NO_INHERITANCE;
    ea[0].Trustee.TrusteeForm = TRUSTEE_IS_SID;
    ea[0].Trustee.TrusteeType = TRUSTEE_IS_WELL_KNOWN_GROUP;
    ea[0].Trustee.ptstrName = (LPTSTR) everyoneSID;

    // Create the new ACL that contains the new ACEs
    SetEntriesInAcl(1, ea, NULL, &acl);

    // Initialize a security descriptor.  
    sd = (PSECURITY_DESCRIPTOR) LocalAlloc(LPTR, SECURITY_DESCRIPTOR_MIN_LENGTH);
    if (sd == NULL) {
        err = MY_ERR_OUT_OF_MEMORY;
        goto done;
    }
    if (!InitializeSecurityDescriptor(sd, SECURITY_DESCRIPTOR_REVISION)) {
        err = HandleFSError(path);
        goto done;
    }

    // Add the ACL to the security descriptor. 
    ret = SetSecurityDescriptorDacl(
        sd,
        TRUE, // bDaclPresent flag
        acl,
        FALSE
    );
    if (!ret) {
        err = HandleFSError(path);
        goto done;
    }

    // Change the security attributes
    if (!SetFileSecurity(path, DACL_SECURITY_INFORMATION, sd)) {
        err = HandleFSError(path);
        goto done;
    }

done:
    if (everyoneSID) {
        FreeSid(everyoneSID);
    }
    if (acl) {
        LocalFree(acl);
    }
    if (sd) {
        LocalFree(sd);
    }

    return err;
}

int MyFile::GrantUserFullControlACL(const wchar_t* path, bool isDir) {
    int err = 0;

    ACL* oldDacl = NULL;
    ACL* newDacl = NULL;
    SECURITY_DESCRIPTOR* sd = NULL;
    PSID sid = NULL;
    SID_IDENTIFIER_AUTHORITY authNt = SECURITY_NT_AUTHORITY;
    EXPLICIT_ACCESS ea = { 0 };

    HANDLE handle = CreateFile(
        path, 
        READ_CONTROL | WRITE_DAC,
        0,
        NULL,
        OPEN_EXISTING,
        (isDir ? FILE_FLAG_BACKUP_SEMANTICS : 0),
        NULL
    );
    if (handle == INVALID_HANDLE_VALUE) {
        err = HandleFSError(path);
        goto done;
    }

    err = GetSecurityInfo(handle, SE_FILE_OBJECT, DACL_SECURITY_INFORMATION, NULL, NULL, &oldDacl, NULL, (void**)&sd);
    if (err) {
        HandleFSError(path);
        goto done;
    }

    if (!AllocateAndInitializeSid(&authNt, 2, SECURITY_BUILTIN_DOMAIN_RID, DOMAIN_ALIAS_RID_USERS, 0, 0, 0, 0, 0, 0, &sid)) {
        err = HandleFSError(path);
        goto done;
    }

    ea.grfAccessMode = GRANT_ACCESS;
    ea.grfAccessPermissions = GENERIC_ALL;
    ea.grfInheritance = CONTAINER_INHERIT_ACE | OBJECT_INHERIT_ACE;
    ea.Trustee.TrusteeType = TRUSTEE_IS_GROUP;
    ea.Trustee.TrusteeForm = TRUSTEE_IS_SID;
    ea.Trustee.ptstrName   = (LPTSTR)sid;

    err = SetEntriesInAcl(1, &ea, oldDacl, &newDacl);
    if (err) {
        HandleFSError(path);
        goto done;
    }

    err = SetSecurityInfo(handle, SE_FILE_OBJECT, DACL_SECURITY_INFORMATION, NULL, NULL, newDacl, NULL);
    if (err) {
        HandleFSError(path);
        goto done;
    }

done:
    
    if (sid) {
        FreeSid(sid);
    }
    if (newDacl) {
        LocalFree(newDacl);
    }
    if (sd) {
        LocalFree(sd);
    }
    //if (oldDacl) {
    //    LocalFree(oldDacl);
    //}
    if (handle != INVALID_HANDLE_VALUE) {
        CloseHandle(handle);
    }

    return err;
}

#endif


#endif // MY_FILE_CPP_
