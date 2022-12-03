#ifndef MY_DATE_H_
#define MY_DATE_H_

#include "MyCoreDef.h"
#include "MyStringA.h"

class MyDate {

public:
  MyDate();

  int Year() { return m_Year; }
  void SetYear(int val) { m_Year = val; }

  int Month() { return m_Month; }
  void SetMonth(int val) { m_Month = val; }

  int Day() { return m_Day; }
  void SetDay(int val) { m_Day = val; }

  int Hour() { return m_Hour; }
  void SetHour(int val) { m_Hour = val; }

  int Minute() { return m_Minute; }
  void SetMinute(int val) { m_Minute = val; }

  int Second() { return m_Second; }
  void SetSecond(int val) { m_Second = val; }

  int MS() { return m_MS; }
  void setMS(int val) { m_MS = val; }

  static UINT64 UtcNow();
  static UINT64 UtcToLocal(UINT64 milliSecondsFrom1970);
  static UINT64 LocalToUtc(UINT64 milliSecondsFrom1970);
  static void LocalNow(MyStringA& ret, const char* format=NULL);

  static void LocalNow(MyDate& ret);
  static void UtcNow(MyDate& ret);

  void Reset();

  UINT64 MillisecondsFrom1970();
  int FromMillisecondsFrom1970(UINT64 mills);

  int Format(const char* format, MyStringA& ret);
  int Parse(const char* format, const char* timestr);

private:
  static int FormatTimeToStr(MyStringA& ret, const char* format, int year, int month, int day, int hour, int minute, int second, int ms, int dayOfWeek, int offsetHour, int offsetMinutes);
  static int ParseTime(const char* timestr, const char* format, int* year, int* month, int *day, int* hour, int* minute, int* second, int* ms, int* dayOfWeek, int* offsetHour, int* offsetMinutes);
  static int ParseNextFormatToken(const char* str);
  static int ParseInt(const char*& str, int len);

  static int FromMillisecondsFrom1970(UINT64 millis, int* year, int* month, int* day, int* hour, int* minutes, int* seconds, int* ms, int* dayOfWeek);
  static int ToMillisecondsFrom1970(int year, int month, int day, int hour, int minutes, int seconds, int ms, UINT64* millis);

private:
  int m_Year;
  int m_Month;
  int m_Day;
  int m_Hour;
  int m_Minute;
  int m_Second;
  int m_MS;
  int m_DayOfWeek;

};

#endif // MY_DATE_H_