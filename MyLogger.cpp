#ifndef _MY_LOGGER_CPP_
#define _MY_LOGGER_CPP_

#include "MyLogger.h"

#include "MyDate.h"
#include "MyUtils.h"

const char DECIMAL_TO_HEX_MAP[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f' };
const char ASCII_VISIABLE_CHAR_MAP[] = {
  '.', '.', '.', '.', '.', '.', '.', '.',   /* 0-7    :non visible */
  '.', '.', '.', '.', '.', '.', '.', '.',   /* 7-15   :non visible */
  '.', '.', '.', '.', '.', '.', '.', '.',   /* 16-23  :non visible */
  '.', '.', '.', '.', '.', '.', '.', '.',   /* 24-31  :non visible */
  '.', '!', '\"', '#', '$', '%', '&', '\'', /* 32-39  :            */ /* map whitespace as . */
  '(', ')', '*', '+', ',', '-', '.', '/',   /* 40-47  :            */
  '0', '1', '2', '3', '4', '5', '6', '7',   /* 48-55  :            */
  '8', '9', ':', ';', '<', '=', '>', '?',   /* 56-63  :            */
  '@', 'A', 'B', 'C', 'D', 'E', 'F', 'G',   /* 64-71  :            */
  'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O',   /* 72-79  :            */
  'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W',   /* 80-87  :            */
  'X', 'Y', 'Z', '[', '\\', ']', '^', '_',  /* 88-95  :            */
  '`', 'a', 'b', 'c', 'd', 'e', 'f', 'g',   /* 96-103 :            */
  'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o',   /* 104-111:            */
  'p', 'q', 'r', 's', 't', 'u', 'v', 'w',   /* 112-119:            */
  'x', 'y', 'z', '{', '|', '}', '~', '.',   /* 120-127:            */
};

MyLogger::MyLogger(MyILogger* logger) {
  m_ILogger = logger;
  m_LogLevel = MyLogLevel::None;

#if _DEBUG
  m_LogLevel = MyLogLevel::DebugEx;
#endif

  strcpy(m_LogLevelStrs[0], "NONE   ");
  strcpy(m_LogLevelStrs[1], "ERROR  ");
  strcpy(m_LogLevelStrs[2], "INFO   ");
  strcpy(m_LogLevelStrs[3], "VERBOSE");
  strcpy(m_LogLevelStrs[4], "DEBUG  ");
  strcpy(m_LogLevelStrs[5], "DEBUGEX");

  m_PrefixBuf[0] = 0;
  m_SuffixBuf[0] = 0;
  m_LogBuf[0] = 0;
}
MyLogger::~MyLogger() { }

void MyLogger::LogError(const char* fmt, ...) {
  if (m_LogLevel < MyLogLevel::Error) return;

  va_list args;
  va_start(args, fmt);
  Log(MyLogLevel::Error, fmt, args);
  va_end(args);
}
void MyLogger::LogInfo(const char* fmt, ...) {
  if (m_LogLevel < MyLogLevel::Info) return;

  va_list args;
  va_start(args, fmt);
  Log(MyLogLevel::Info, fmt, args);
  va_end(args);
}
void MyLogger::LogVerbose(const char* fmt, ...) {
  if (m_LogLevel < MyLogLevel::Verbose) return;

  va_list args;
  va_start(args, fmt);
  Log(MyLogLevel::Verbose, fmt, args);
  va_end(args);
}
void MyLogger::LogDebug(const char* fmt, ...) {
  if (m_LogLevel < MyLogLevel::Debug) return;

  va_list args;
  va_start(args, fmt);
  Log(MyLogLevel::Debug, fmt, args);
  va_end(args);
}
void MyLogger::LogDebugEx(const char* fmt, ...) {
  if (m_LogLevel < MyLogLevel::DebugEx) return;

  va_list args;
  va_start(args, fmt);
  Log(MyLogLevel::DebugEx, fmt, args);
  va_end(args);
}
void MyLogger::LogHexDump(const char* data, int len) {
  if (m_LogLevel < MyLogLevel::Debug) return;

  // "xxxxxxxx xx xx xx xx xx xx xx xx  xx xx xx xx xx xx xx xx    xxxxxxxx xxxxxxxx"
  char line[96] = { 0 };
  for (int i = 0; i < len; i += 16) {
    sprintf(line, "%08x", i);

    memset(line + 8, ' ', 79);
    line[80] = 0;
    
    int leftBytes = i + 16 > len ? len - i : 16;
    for (int j = 0; j < leftBytes; j++) {
      int index = 9 + (j < 8 ? j * 3 : j * 3 + 1);
      char c = data[i + j] & 0xFF;

      line[index]   = DECIMAL_TO_HEX_MAP[c / 16];
      line[index+1] = DECIMAL_TO_HEX_MAP[c % 16];

      int index2 = 62 + (j < 8 ? j : j + 1);
      line[index2] = ASCII_VISIABLE_CHAR_MAP[c];
    }

    m_CriticalSection.Acquire();
    if (m_ILogger) {
        m_ILogger->OutoutLog(line);
    } else {
        printf("%s\n", line);
    }
    m_CriticalSection.Release();
  }
}

void MyLogger::Log(MyLogLevel level, const char* fmt, va_list args) {
  m_CriticalSection.Acquire();
  int curLen = MakePrefix(level);
  curLen += vsprintf(&m_LogBuf[curLen], fmt, args);
  if (m_ILogger) {
    m_ILogger->OutoutLog(m_LogBuf);
  } else {
    printf("%s\n", m_LogBuf);
  }
  m_CriticalSection.Release();
}

int MyLogger::MakePrefix(MyLogLevel logLevel) {
  // [Timestamp] [ThreadID] [LogLevel]
  long long timestamp = MyDate::UtcNow();
  UINT64 tid = MyUtils::CurrentThreadId();
  const char* levelStr = m_LogLevelStrs[(int)logLevel];

  if (m_PrefixBuf[0] != 0 && m_SuffixBuf[0] != 0) {
    return sprintf(m_LogBuf, "[%s][%llu][%llu][%s][%s] ", m_PrefixBuf, timestamp, tid, levelStr, m_SuffixBuf);
  } else if (m_PrefixBuf[0] != 0) {
    return sprintf(m_LogBuf, "[%s][%llu][%llu][%s] ", m_PrefixBuf, timestamp, tid, levelStr);
  } else if (m_SuffixBuf[0] != 0) {
    return sprintf(m_LogBuf, "[%llu][%llu][%s][%s] ", timestamp, tid, levelStr, m_SuffixBuf);
  } else {
    return sprintf(m_LogBuf, "[%llu][%llu][%s] ", timestamp, tid, levelStr);
  }
}

void MyLogger::CopyFrom(MyLogger* logger) {
  m_ILogger = logger->m_ILogger;
  m_LogLevel = logger->m_LogLevel;
  SetPrefix(logger->Prefix());
  SetSuffix(logger->Suffix());
}

#endif // _MY_LOGGER_CPP_