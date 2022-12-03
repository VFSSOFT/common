#ifndef _MY_LOGGER_H_
#define _MY_LOGGER_H_

#include "MyCoreDef.h"

class MyILogger {
public:
  virtual void OutoutLog(const char* msg) = 0;
};

enum class MyLogLevel {
  None       = 0,
  Error      = 1,
  Info       = 2,
  Verbose    = 3,
  Debug      = 4,
  DebugEx    = 5
};

class MyLogger {
public:
  MyLogger(MyILogger* logger=NULL);
  virtual ~MyLogger();

  MyLogLevel LogLevel() { return m_LogLevel; }
  void SetLogLevel(MyLogLevel val) { m_LogLevel = val; }

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

  void LogError(const char* fmt, ...);
  void LogInfo(const char* fmt, ...);
  void LogVerbose(const char* fmt, ...);
  void LogDebug(const char* fmt, ...);
  void LogDebugEx(const char* fmt, ...);
  void LogHexDump(const char* data, int len);

  void CopyFrom(MyLogger* logger);

protected:
  virtual void Log(MyLogLevel level, const char* fmt, va_list args);

private:
  int MakePrefix(MyLogLevel logLevel);

private:
  MyILogger* m_ILogger;

  MyLogLevel m_LogLevel;

  char m_PrefixBuf[16];
  char m_SuffixBuf[16];
  char m_LogBuf[1024 + 256]; // 256 for log prefix
  char m_LogLevelStrs[6][8];
};

#endif // _MY_LOGGER_H_