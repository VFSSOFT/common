#ifndef _MY_LOGGER_H_
#define _MY_LOGGER_H_

#include "MyCoreDef.h"

#include "MyCriticalSection.h"
#include "MyStringW.h"
#include "MyFile.h"

enum class MyLogLevel {
  None       = 0,
  Error      = 1,
  Info       = 2,
  Verbose    = 3,
  Debug      = 4,
  DebugEx    = 5
};

class MyILogger {
public:
    virtual void OutoutLog(const char* msg) = 0;
    virtual void OutputEventLog(MyLogLevel level, const char* msg) = 0;
};


class MyLogger {
public:
  MyLogger(MyILogger* logger=NULL);
  virtual ~MyLogger();

  MyLogLevel LogLevel() { return m_LogLevel; }
  void SetLogLevel(MyLogLevel val) { m_LogLevel = val; }

  UINT64 LogRotateInterval() { return m_LogRotateInterval; }
  void SetLogRotateInterval(UINT64 val) { m_LogRotateInterval = val; }
  UINT64 MaxLogFileSize() { return m_MaxLogFileSize; }
  void SetMaxLogFileSize(UINT64 val) { m_MaxLogFileSize = val; }

  MyStringW* LogDirectory() { return &m_LogDirectory; }
  int SetLogDirectory(const wchar_t* dir, int len);

  MyILogger* ILogger() { return m_ILogger; }
  void SetILogger(MyILogger* logger);

  inline void SetPrefix(const char* prefix) {
    memset(m_PrefixBuf, 0, 16);
    int l = 16 < strlen(prefix) ? 16 : strlen(prefix);
    strncpy(m_PrefixBuf, prefix, l);
  }
  inline const char* Prefix() { return m_PrefixBuf; }

  inline void SetSuffix(const char* suffix) {
    memset(m_SuffixBuf, 0, 16);
    int l = 16 < strlen(suffix) ? 16 : strlen(suffix);
    strncpy(m_SuffixBuf, suffix, l);
  }
  inline const char* Suffix() { return m_SuffixBuf; }

  bool LogDebugExEnabled() { return m_LogLevel >= MyLogLevel::DebugEx; }
  bool LogDebugEnabled() { return m_LogLevel >= MyLogLevel::Debug; }
  bool LogVerboseEnabled() { return m_LogLevel >= MyLogLevel::Verbose; }

  void LogError(const char* fmt, ...);
  void LogInfo(const char* fmt, ...);
  void LogVerbose(const char* fmt, ...);
  void LogDebug(const char* fmt, ...);
  void LogDebugEx(const char* fmt, ...);
  void LogHexDump(const char* data, int len);

  void LogInfoEvent(const char* fmt, ...);
  void LogErrorEvent(const char* fmt, ...);

  void CopyFrom(MyLogger* logger);

protected:
  virtual void Log(MyLogLevel level, const char* fmt, va_list args);
  virtual void LogEvent(MyLogLevel level, const char* fmt, va_list args);

private:
  int MakePrefix(MyLogLevel logLevel);

  int OpenLogFileHandle();
  int CloseLogFileHandle();
  int RotateLogFile(MyStringW* file);

private:
  MyILogger* m_ILogger;

  MyLogLevel m_LogLevel;

  UINT64     m_LogRotateInterval;
  UINT64     m_MaxLogFileSize;
  MyStringW  m_LogDirectory;
  MyFile     m_LogFile;

  char m_PrefixBuf[16];
  char m_SuffixBuf[16];
  char* m_LogBuf; // 256 for log prefix
  char m_LogLevelStrs[6][8];

  MyCriticalSection m_CriticalSection;

  MY_LAST_ERROR_DECL;
};

#endif // _MY_LOGGER_H_
