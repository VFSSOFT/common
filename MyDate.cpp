
#include "MyDate.h"


static char NUM_BUF[8] = { 0 };

const char* MONTH_ABBR[] = {
  "Jan",
  "Feb",
  "Mar",
  "Apr",
  "May",
  "Jun",
  "Jul",
  "Aug",
  "Sep",
  "Oct",
  "Nov",
  "Dec"
};

const char* WEEK_DAYS[] = {
  "Sun",
  "Mon",
  "Tue",
  "Wed",
  "Thu",
  "Fri",
  "Sat"
};

MyDate::MyDate() {
  this->Reset();
}

UINT64 MyDate::UtcNow() {

#ifndef UNIX
  FILETIME fm;
  SYSTEMTIME tm;
  ULARGE_INTEGER ui;
  GetSystemTime(&tm);
  SystemTimeToFileTime(&tm, &fm);
  ui.LowPart = fm.dwLowDateTime;
  ui.HighPart = fm.dwHighDateTime;
  return ui.QuadPart / 10000 - 11644473600000I64;

#else

  // This is not a real UTC time
  struct timeval tv;
  gettimeofday(&tv, NULL);
  UINT64 ret = tv.tv_usec;
  ret /= 1000; /* Convert from micro seconds (10^-6) to milliseconds (10^-3) */
  ret += (tv.tv_sec * 1000); /* Adds the seconds (10^0) after converting them to milliseconds (10^-3) */
  return ret;

#endif

}

UINT64 MyDate::UtcToLocal(UINT64 milliSecondsFrom1970) {

#ifdef UNIX

  struct tm  ts;
  __int64 seconds = milliSecondsFrom1970 / 1000;
  time_t utc = (time_t)seconds;
  ts = *localtime(&utc);
  seconds = (__int64)timegm(&ts);
  return seconds * 1000;//to milli seconds.

#else

  __int64 ll = milliSecondsFrom1970;
  ll = ll * 10000 + 116444736000000000;
  FILETIME ft, lft;
  ft.dwLowDateTime = (DWORD)ll;
  ft.dwHighDateTime = ll >> 32;
  FileTimeToLocalFileTime(&ft, &lft);
  ll = lft.dwHighDateTime;
  ll <<= 32;
  ll |= lft.dwLowDateTime;
  ll -= 116444736000000000;
  ll /= 10000;
  return ll;

#endif

}

UINT64 MyDate::LocalToUtc(UINT64 milliSecondsFrom1970) {

#ifdef UNIX

  struct tm  ts;
  __int64 seconds = milliSecondsFrom1970 / 1000;
  time_t loc = (time_t)seconds;
  ts = *gmtime(&loc);
  seconds = (int)mktime(&ts);
  return seconds * 1000;//to milli seconds.

#else

  __int64 ll = milliSecondsFrom1970;
  ll = ll * 10000 + 116444736000000000;
  FILETIME ft, lft;
  ft.dwLowDateTime = (DWORD)ll;
  ft.dwHighDateTime = ll >> 32;
  LocalFileTimeToFileTime(&ft, &lft);
  ll = lft.dwHighDateTime;
  ll <<= 32;
  ll |= lft.dwLowDateTime;
  ll -= 116444736000000000;
  ll /= 10000;
  return ll;

#endif

}

void MyDate::LocalNow(MyStringA& ret, const char* format) {
  MyDate now;
  MyDate::LocalNow(now);
}

void MyDate::LocalNow(MyDate& ret) {

#ifndef UNIX

  SYSTEMTIME tm;
  GetLocalTime(&tm);
  ret.m_Year   = tm.wYear;
  ret.m_Month  = tm.wMonth;
  ret.m_Day    = tm.wDay;
  ret.m_Hour   = tm.wHour;
  ret.m_Minute = tm.wMinute;
  ret.m_Second = tm.wSecond;
  ret.m_MS     = tm.wMilliseconds;

#else

  struct tm tm;
  time_t t;
  time(&t);

  localtime_r(&t, &tm);

  ret.m_Year   = tm.tm_year + 1900;
  ret.m_Month  = tm.tm_mon + 1;
  ret.m_Day    = tm.tm_mday;
  ret.m_Hour   = tm.tm_hour;
  ret.m_Minute = tm.tm_min;
  ret.m_Second = tm.tm_sec;
  ret.m_MS     = 0;

#endif

}
void MyDate::UtcNow(MyDate& ret) {

#ifndef UNIX

  SYSTEMTIME tm;
  GetSystemTime(&tm);
  ret.m_Year   = tm.wYear;
  ret.m_Month  = tm.wMonth;
  ret.m_Day    = tm.wDay;
  ret.m_Hour   = tm.wHour;
  ret.m_Minute = tm.wMinute;
  ret.m_Second = tm.wSecond;
  ret.m_MS     = tm.wMilliseconds;

#else

  time_t tt;
  time(&tt);
  struct tm *timeinfo = NULL;
  timeinfo = gmtime(&tt);

  ret.m_Year   = timeinfo->tm_year + 1900;
  ret.m_Month  = timeinfo.tm_mon + 1;
  ret.m_Day    = timeinfo.tm_mday;
  ret.m_Hour   = timeinfo.tm_hour;
  ret.m_Minute = timeinfo.tm_min;
  ret.m_Second = timeinfo.tm_sec;
  ret.m_MS     = 0;
#endif
}

UINT64 MyDate::MillisecondsFrom1970() {
  UINT64 millis = 0;
  ToMillisecondsFrom1970(
    m_Year,
    m_Month,
    m_Day,
    m_Hour,
    m_Minute,
    m_Second,
    m_MS,
    &millis
  );
  return millis;
}
int MyDate::FromMillisecondsFrom1970(UINT64 mills) {
  this->Reset();
  return FromMillisecondsFrom1970(
    mills,
    &m_Year,
    &m_Month,
    &m_Day,
    &m_Hour,
    &m_Minute,
    &m_Second,
    &m_MS,
    &m_DayOfWeek
  );
}

int MyDate::Format(const char* format, MyStringA& ret) {
  return MyDate::FormatTimeToStr(ret, format, m_Year, m_Month, m_Day, m_Hour, m_Minute, m_Second, m_MS, m_DayOfWeek, -1, -1);
}
int MyDate::FormatTimeToStr(MyStringA& ret, const char* format, int year, int month, int day, int hour, int minute, int second, int ms, int dayOfWeek, int offsetHour, int offsetMinutes) {
  int retCode = 0;
  const char* p = format;
  int curTokenLen = 0;

  while (*p != 0) {

    const char* pStart = p;
    const char* pEnd = pStart;
    char c = *pEnd++;
    while (*pEnd == c) pEnd++; // all the `format` string will end with a '\0', so no need to check the end of the string here.
    curTokenLen = (int)((UINT64)pEnd - (UINT64)pStart);

    if (strncmp(p, "yy", curTokenLen) == 0) {
      if (retCode = ret.AppendInt(year % 100, 2)) return retCode;
    } else if (strncmp(p, "yyyy", curTokenLen) == 0) {
      if (retCode = ret.AppendInt(year, 4)) return retCode;
    } else if (strncmp(p, "M", curTokenLen) == 0) {
      if (retCode = ret.AppendInt(month, 0)) return retCode;
    } else if (strncmp(p, "MM", curTokenLen) == 0) {
      if (retCode = ret.AppendInt(month, 2)) return retCode;
    } else if (strncmp(p, "MMM", curTokenLen) == 0) {
      if (retCode = ret.Append(MONTH_ABBR[month-1])) return retCode;
    } else if (strncmp(p, "d", curTokenLen) == 0) {
      if (retCode = ret.AppendInt(day, 0)) return retCode;
    } else if (strncmp(p, "dd", curTokenLen) == 0) {
      if (retCode = ret.AppendInt(day, 2)) return retCode;
    } else if (dayOfWeek >= 0 && dayOfWeek <= 6 && curTokenLen == 3 && strncmp(p, "ddd", curTokenLen) == 0) {
      if (retCode = ret.Append(WEEK_DAYS[dayOfWeek])) return retCode;
    } else if (strncmp(p, "h", curTokenLen) == 0) {
      if (retCode = ret.AppendInt(hour > 12 ? hour - 12 : hour, 0)) return retCode;
    } else if (strncmp(p, "hh", curTokenLen) == 0) {
      if (retCode = ret.AppendInt(hour > 12 ? hour - 12 : hour, 2)) return retCode;
    } else if (strncmp(p, "H", curTokenLen) == 0) {
      if (retCode = ret.AppendInt(hour, 0)) return retCode;
    } else if (strncmp(p, "HH", curTokenLen) == 0) {
      if (retCode = ret.AppendInt(hour, 2)) return retCode;
    } else if (strncmp(p, "m", curTokenLen) == 0) {
      if (retCode = ret.AppendInt(minute, 0)) return retCode;
    } else if (strncmp(p, "mm", curTokenLen) == 0) {
      if (retCode = ret.AppendInt(minute, 2)) return retCode;
    } else if (strncmp(p, "s", curTokenLen) == 0) {
      if (retCode = ret.AppendInt(second, 0)) return retCode;
    } else if (strncmp(p, "ss", curTokenLen) == 0) {
      if (retCode = ret.AppendInt(second, 2)) return retCode;
    } else if (strncmp(p, "tt", curTokenLen) == 0) {
      if (retCode = ret.Append(hour < 12 ? "AM" : "PM")) return retCode;
    } else if (curTokenLen == 3 && strncmp(p, "zzz", curTokenLen) == 0) {
      if (offsetHour >= 0) {
        if (retCode = ret.Append("+")) return retCode;
      } else {
        if (retCode = ret.Append("-")) return retCode;
        offsetHour *= -1;
      }
      if (retCode = ret.AppendInt(offsetHour, 2)) return retCode;
      if (retCode = ret.Append(":")) return retCode;
      if (offsetMinutes < 0) offsetMinutes *= -1;
      if (retCode = ret.AppendInt(offsetMinutes, 2)) return retCode;
    } else if (curTokenLen == 6 && strncmp(p, "ffffff", curTokenLen) == 0) {
      //https://docs.microsoft.com/en-us/dotnet/standard/base-types/custom-date-and-time-format-strings
      if (retCode = ret.AppendInt(ms, 3)) return retCode;
      if (retCode = ret.Append("000")) return retCode;
    } else if (curTokenLen == 4 && strncmp(p, "ffff", curTokenLen) == 0) {
      if (retCode = ret.AppendInt(ms, 3)) return retCode;
      if (retCode = ret.Append("0")) return retCode;
    } else if (curTokenLen == 3 && strncmp(p, "fff", curTokenLen) == 0) {
      if (retCode = ret.AppendInt(ms, 3)) return retCode;
    } else {
      if (retCode = ret.Append(p, curTokenLen)) return retCode;
    }

    p += curTokenLen;
  }
  return retCode;
}

int MyDate::Parse(const char* format, const char* timestr) {
  this->Reset();
  
  return ParseTime(
    timestr, 
    format, 
    &this->m_Year,
    &this->m_Month,
    &this->m_Day,
    &this->m_Hour,
    &this->m_Minute,
    &this->m_Second,
    &this->m_MS,
    &this->m_DayOfWeek, 
    NULL,
    NULL
  );
}
int MyDate::ParseTime(const char* timestr, const char* format, int* year, int* month, int *day, int* hour, int* minute, int* second, int* ms, int* dayOfWeek, int* offsetHour, int* offsetMinutes) {
  int tokenLen = 0;
  int num = 0;
  const char* t = timestr;
  const char* f = format;

  while (true) {
    tokenLen = ParseNextFormatToken(f);
    if (tokenLen == 0) break;

    if (tokenLen == 2 && strncmp(f, "yy", tokenLen) == 0) {
      num = ParseInt(t, 2);
      if (num < 0) return -1;
      if (year) {
        *year = num >= 50 ? 1900 + num : 2000 + num;
      }
    } else if (tokenLen == 4 && strncmp(f, "yyyy", tokenLen) == 0) {
      num = ParseInt(t, 4);
      if (num < 0) return -1;
      if (year) {
        *year = num;
      }
    } else if (tokenLen == 1 && strncmp(f, "M", tokenLen) == 0) {
      num = ParseInt(t, 0);
      if (num < 1 || num > 12) return -1;
      if (month) {
        *month = num;
      }
    } else if (tokenLen == 2 && strncmp(f, "MM", tokenLen) == 0) {
      num = ParseInt(t, 2);
      if (num < 1 || num > 12) return -1;
      if (month) {
        *month = num;
      }
    } else if (tokenLen == 3 && strncmp(f, "MMM", tokenLen) == 0) {
      num = -1;
      for (int i = 0; i < 12; i++) {
        if (strncmp(t, MONTH_ABBR[i], 3) == 0) {
          num = i + 1;
          break;
        }
      }
      if (num < 0) return -1;

      if (month) {
        *month = num;
      }
      t += 3;
    } else if (tokenLen == 1 && strncmp(f, "d", tokenLen) == 0) {
      num = ParseInt(t, 0);
      if (num < 1 || num > 31) return -1;
      if (day) {
        *day = num;
      }
    } else if (tokenLen == 2 && strncmp(f, "dd", tokenLen) == 0) {
      num = ParseInt(t, 2);
      if (num < 1 || num > 31) return -1;
      if (day) {
        *day = num;
      }
    } else if (tokenLen == 3 && strncmp(f, "ddd", tokenLen) == 0) {
      // 0 - 6 is valid value
      num = -1;
      for (int i = 0; i < 7; i++) {
        if (strncmp(t, WEEK_DAYS[i], 3) == 0) {
          num = i;
          break;
        }
      }
      if (num < 0 || num > 6) return -1;
      if (dayOfWeek) {
        *dayOfWeek = num;
      }
      t += 3;
    } else if (tokenLen == 1 && strncmp(f, "h", tokenLen) == 0) {
      num = ParseInt(t, 0);
      if (num < 0 || num >= 12) return -1;
      if (hour) {
        *hour = num;
      }
    } else if (tokenLen == 2 && strncmp(f, "hh", tokenLen) == 0) {
      num = ParseInt(t, 2);
      if (num < 0 || num >= 12) return false;
      if (hour) {
        *hour = num;
      }
    } else if (tokenLen == 1 && strncmp(f, "H", tokenLen) == 0) {
      num = ParseInt(t, 0);
      if (num < 0 || num >= 24) return -1;
      if (hour) {
        *hour = num;
      }
    } else if (tokenLen == 2 && strncmp(f, "HH", tokenLen) == 0) {
      num = ParseInt(t, 2);
      if (num < 0 || num >= 24) return -1;
      if (hour) {
        *hour = num;
      }
    } else if (tokenLen == 1 && strncmp(f, "m", tokenLen) == 0) {
      num = ParseInt(t, 0);
      if (num < 0 || num >= 60) return -1;
      if (minute) {
        *minute = num;
      }
    } else if (tokenLen == 2 && strncmp(f, "mm", tokenLen) == 0) {
      num = ParseInt(t, 2);
      if (num < 0 || num >= 60) return -1;
      if (minute) {
        *minute = num;
      }
    } else if (tokenLen == 1 && strncmp(f, "s", tokenLen) == 0) {
      num = ParseInt(t, 0);
      if (num < 0 || num >= 60) return -1;
      if (second) {
        *second = num;
      }
    } else if (tokenLen == 2 && strncmp(f, "ss", tokenLen) == 0) {
      num = ParseInt(t, 2);
      if (num < 0 || num >= 60) return -1;
      if (second) {
        *second = num;
      }
    } else if (tokenLen == 2 && strncmp(f, "tt", tokenLen) == 0) {
      if (*t == 'A' && *(t+1) == 'M') {
        t += 2;
      } else if (*t == 'P' && *(t+1) == 'M') {
        t += 2;
        if (hour) {
          *hour += 12;
        }
      } else {
        return false;
      }
    } else if (tokenLen == 3 && strncmp(f, "zzz", tokenLen) == 0) {
      return -1; // doesn't supported
    } else if (tokenLen == 2 && strncmp(f, "ff", tokenLen) == 0) {
      num = ParseInt(t, 2);
      if (num < 0) return -1;
      if (ms) {
        *ms = num;
      }
    } else if (tokenLen == 3 && strncmp(f, "fff", tokenLen) == 0) {
      num = ParseInt(t, 3);
      if (num < 0) return -1;
      if (ms) {
        *ms = num;
      }
    } else if (tokenLen == 4 && strncmp(f, "ffff", tokenLen) == 0) {
      num = ParseInt(t, 4);
      if (num < 0) return -1;
      if (ms) {
        *ms = num;
      }
    } else if (tokenLen == 6 && strncmp(f, "ffffff", tokenLen) == 0) {
      num = ParseInt(t, 6);
      if (num < 0) return -1;
      if (ms) {
        *ms = num;
      }
    } else {
      if (strncmp(t, f, tokenLen) != 0) return -1;
      t += tokenLen;
    }
    f += tokenLen;
  }
  return *t == 0 ? 0 : -1;
}
int MyDate::ParseNextFormatToken(const char* str) {
  const char* p = str;
  if (*p == 0) return 0;

  char c = *p;
  while (*p == c) p++;
  return (int)(p - str);
}
int MyDate::ParseInt(const char*& str, int len) {
  int ret = 0;
  if (len > 0) {
    if (strlen(str) < len) return -1;

    const char* p = str;
    while (*p >= '0' && *p <= '9' && len > 0) {
      ret = ret * 10 + (*p - '0');
      len--;
      p++;
    }
    if (len == 0) {
      str = p;
      return ret;
    } else {
      return -1;
    }
  } else {
    if (*str == '0') return -1;
    while (*str >= '0' && *str <= '9') {
      ret = ret * 10 + (*str - '0');
      str++;
    }
    return ret;
  }
}


void MyDate::Reset() {
  m_Year = 0;
  m_Month = 0;
  m_Day = 0;
  m_Hour = 0;
  m_Minute = 0;
  m_Second = 0;
  m_MS = 0;
  m_DayOfWeek = 0;
}

int MyDate::FromMillisecondsFrom1970(UINT64 millis, int* year, int* month, int* day, int* hour, int* minutes, int* seconds, int* ms, int* dayOfWeek) {
#ifndef UNIX
  ULARGE_INTEGER ft;
  SYSTEMTIME st;
  ft.QuadPart = 116444736000000000 + 10000 * millis;
  FileTimeToSystemTime((FILETIME*)&ft, &st);
  if (year) *year = st.wYear;
  if (month) *month = st.wMonth;
  if (day) *day = st.wDay;
  if (hour) *hour = st.wHour;
  if (minutes) *minutes = st.wMinute;
  if (seconds) *seconds = st.wSecond;
  if (ms) *ms = st.wMilliseconds;
  if (dayOfWeek) *dayOfWeek = st.wDayOfWeek;
#else
  time_t tt = (time_t)(millis/1000);
  struct tm *timeinfo = NULL;
  timeinfo = gmtime(&tt);
  if (year) *year = 1900+timeinfo->tm_year;
  if (month) *month = timeinfo->tm_mon+1;
  if (day) *day = timeinfo->tm_mday;
  if (hour) *hour = timeinfo->tm_hour;
  if (minutes) *minutes = timeinfo->tm_min;
  if (seconds) *seconds = timeinfo->tm_sec;
#endif // UNIX   
  return 0; 
}
int MyDate::ToMillisecondsFrom1970(int year, int month, int day, int hour, int minutes, int seconds, int ms, UINT64* millis) {
#ifndef UNIX
  ULARGE_INTEGER ft;
  SYSTEMTIME st;
  st.wYear = year;
  st.wMonth = month;
  st.wDay = day;
  st.wHour = hour;
  st.wMinute = minutes;
  st.wSecond = seconds;
  st.wMilliseconds = ms;

  SystemTimeToFileTime(&st, (FILETIME*)&ft);
  *millis = (ft.QuadPart - 116444736000000000I64) / 10000;
#else
  time_t tt;
  struct tm timeinfo;
  timeinfo.tm_year = year - 1900;
  timeinfo.tm_mon = month - 1;
  timeinfo.tm_mday = day;
  timeinfo.tm_hour = hour;
  timeinfo.tm_min = minutes;
  timeinfo.tm_sec = seconds;
  timeinfo.tm_isdst = -1;
  tt = mktime(&timeinfo);
  *millis = tt * 1000;
#endif // UNIX
  return 0; 
}



