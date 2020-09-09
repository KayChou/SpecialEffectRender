#include "timstamp.h"

#include <stdio.h>
#include <time.h>

#ifndef __STDC_FORMAT_MACROS
#define __STDC_FORMAT_MACROS
#endif

#include <inttypes.h>

const string Timestamp::DEFUALT_STYLE  = "%4d%02d%02d %02d:%02d:%02d.%06d";
const string Timestamp::STRING_STYLE   = "%4d%02d%02d%02d%02d%02d%06d";
const string Timestamp::FILENAME_STYLE = "%4d%02d%02d%02d%02d%02d";

#ifdef WIN64
#include <windows.h>
#else
# include <sys/time.h>
# define _snprintf_s(a,b,c,...) snprintf(a,b,__VA_ARGS__)
#endif
#ifdef WIN64
int gettimeofday(struct timeval *tp, void *tzp)
{
    time_t clock;
    struct tm tm;
    SYSTEMTIME wtm;
    GetLocalTime(&wtm);
    tm.tm_year  = wtm.wYear - 1900;
    tm.tm_mon   = wtm.wMonth - 1;
    tm.tm_mday  = wtm.wDay;
    tm.tm_hour  = wtm.wHour;
    tm.tm_min   = wtm.wMinute;
    tm.tm_sec   = wtm.wSecond;
    tm.tm_isdst = -1;
    clock       = mktime(&tm);
    tp->tv_sec  = clock;
    tp->tv_usec = wtm.wMilliseconds * 1000;
    return (0);
}
#endif

string Timestamp::toString() const
{
  char buf[32] = {0};
  int64_t seconds = microSecondsSinceEpoch_ / kMicroSecondsPerSecond;
  int64_t microseconds = microSecondsSinceEpoch_ % kMicroSecondsPerSecond;
  _snprintf_s(buf, sizeof(buf)-1, sizeof(buf)-1, "%" PRId64 ".%06" PRId64 "", seconds, microseconds);
  return buf;
}

string Timestamp::toFormattedString(const string formatStyle, bool showMicroseconds) const
{
  char buf[32] = {0};
  time_t seconds = static_cast<time_t>(microSecondsSinceEpoch_ / kMicroSecondsPerSecond);
  struct tm tm_time;
  //_gmtime64_s(&tm_time, &seconds);  //msvc _gmtime64_s function is thread safe;linux always use gmtime_r that thread safe
  gmtime_r(&seconds, &tm_time);  //msvc _gmtime64_s function is thread safe;linux always use gmtime_r that thread safe

  if (showMicroseconds)
  {
    int microseconds = static_cast<int>(microSecondsSinceEpoch_ % kMicroSecondsPerSecond);
    _snprintf_s(buf, sizeof(buf), sizeof(buf), formatStyle.c_str(),
             tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday,
             tm_time.tm_hour + 8/* Beijing Time*/, tm_time.tm_min, tm_time.tm_sec,
             microseconds);
  }
  else
  {
    _snprintf_s(buf, sizeof(buf), sizeof(buf), formatStyle.c_str(),
             tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday,
             tm_time.tm_hour + 8/* Beijing Time*/, tm_time.tm_min, tm_time.tm_sec);
  }
  return buf;
}

Timestamp Timestamp::now()
{
  struct timeval tv;
  gettimeofday(&tv, NULL);
  int64_t seconds = tv.tv_sec;
  return Timestamp(seconds * kMicroSecondsPerSecond + tv.tv_usec);
}