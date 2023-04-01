#ifndef MY_FILE_H_
#define MY_FILE_H_

#include "MyCoreDef.h"
#include "MyStringA.h"
#include "MyFileAttributes.h"
#include "MyArray.h"
#include "MyStringW.h"
#include "MyValArray.h"

#define MY_FILE_DESIRED_ACCESS_READ                     1
#define MY_FILE_DESIRED_ACCESS_WRITE                    2
#define MY_FILE_DESIRED_ACCESS_READ_WRITE               (MY_FILE_DESIRED_ACCESS_READ | MY_FILE_DESIRED_ACCESS_WRITE)
#define MY_FILE_DESIRED_ACCESS_ALL                      (MY_FILE_DESIRED_ACCESS_READ | MY_FILE_DESIRED_ACCESS_WRITE)

#define MY_FILE_SHARE_MODE_SHARE_READ                   1
#define MY_FILE_SHARE_MODE_SHARE_WRITE                  2
#define MY_FILE_SHARE_MODE_SHARE_DELETE                 4
#define MY_FILE_SHARE_MODE_SHARE_ALL                    (MY_FILE_SHARE_MODE_SHARE_READ | MY_FILE_SHARE_MODE_SHARE_WRITE | MY_FILE_SHARE_MODE_SHARE_DELETE)

#define MY_FILE_CREATION_DISP_CREATE_ALWAYS             1
#define MY_FILE_CREATION_DISP_CREATE_NEW                2
#define MY_FILE_CREATION_DISP_OPEN_ALWAYS               4
#define MY_FILE_CREATION_DISP_OPEN_EXISTING             8
#define MY_FILE_CREATION_DISP_CREATE_TRUNCATE_EXISTING  16

#define MY_FILE_SEEK_TYPE_BEGIN                         1
#define MY_FILE_SEEK_TYPE_CURRENT                       2
#define MY_FILE_SEEK_TYPE_END                           3

class MyFile {
public:
  MyFile();
  ~MyFile();

  int Open(const wchar_t* path, int creationDisp, int desiredAccess=MY_FILE_DESIRED_ACCESS_ALL, int shareMode=MY_FILE_SHARE_MODE_SHARE_ALL);
  
  static BOOL FileExists(const wchar_t* path);
  static bool DirectoryExist(const wchar_t* path);
  static UINT64 FileAttributes(const wchar_t* path);
  static int SetFileAttributes(const wchar_t* path, UINT64 attr);

    int CreateDirectory(const wchar_t* path);
    int DeleteDirectory(const wchar_t* path);
    int DeleteDirectoryRecursively(const wchar_t* path);
    int ListDirectory(const wchar_t* path, MyArray<MyStringW>* filenames, MyValArray<UINT64>* attrs);

    int DeleteFile();
    int DeleteFile(const wchar_t* path);

    int Rename(const wchar_t* oldPath, const wchar_t* newPath);

    int FullPath(const wchar_t* path, MyStringW* fullPath);

    HANDLE SysHandle() { return m_Handle; }

  inline bool Opened() { return m_Handle != NULL; }
  int Close();

  int Length(UINT64* len);
  int SetLength(UINT64 len);

  UINT64 CurrentPosition();
  int Seek(UINT64 pos, int seekType);
  int Write(const char* data, int len);
  int Write(UINT64 pos, const char* data, int len);
  int Read(const char* data, int* len);
  int Read(UINT64 pos, const char* data, int* len);

  int ReadAllBytes(const wchar_t* path, MyBuffer* data);

    int MyGetDiskFreeSpace(const wchar_t* path, UINT64* freeBytesAvailableToCaller, UINT64* totalNumberOfBytes, UINT64* totalNumberOfFreeBytes);

private:
  void* OpenFileHandle(const wchar_t* path, int openDisp, int desiredAccess, int shareMode);
  int  HandleFSError(const wchar_t* path);

private:
  MyStringW m_Path;
  void*     m_Handle;

  MY_LAST_ERROR_DECL;
};

#endif // MY_FILE_H_
