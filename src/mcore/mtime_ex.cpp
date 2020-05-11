
#if defined(_MSC_VER)
#include <windows.h>
#else
#include <sys/time.h>
#endif
#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include "mtime_ex.h"

#if defined(_MSC_VER)
char *mtime2s(char *buf)
{
    static char buf1[128];

    char buf2[64];
    char *tbuf = buf ? buf : buf1;

    // Note: some broken versions only have 8 trailing zero's, the correct epoch has 9 trailing zero's
    // This magic number is the number of 100 nanosecond intervals since January 1, 1601 (UTC)
    // until 00:00:00 January 1, 1970 
    static const uint64_t EPOCH = ((uint64_t)116444736000000000ULL);

    SYSTEMTIME  system_time;
    FILETIME    file_time;
    uint64_t    time;
    time_t    sec;

    GetSystemTime(&system_time);
    SystemTimeToFileTime(&system_time, &file_time);
    time = ((uint64_t)file_time.dwLowDateTime);
    time += ((uint64_t)file_time.dwHighDateTime) << 32;

    sec = (uint32_t)((time - EPOCH) / 10000000L);

    strftime(buf2, 20, "%Y-%m-%d %H:%M:%S", localtime(&sec));

    sprintf(buf1, "%s.%03d", buf2, system_time.wMilliseconds);

    return buf1;
}

wchar_t *mtime2sW(wchar_t *buf)
{
    static wchar_t buf1[128];

    wchar_t buf2[64];
    wchar_t *tbuf = buf ? buf : buf1;

    // Note: some broken versions only have 8 trailing zero's, the correct epoch has 9 trailing zero's
    // This magic number is the number of 100 nanosecond intervals since January 1, 1601 (UTC)
    // until 00:00:00 January 1, 1970 
    static const uint64_t EPOCH = ((uint64_t)116444736000000000ULL);

    SYSTEMTIME  system_time;
    FILETIME    file_time;
    uint64_t    time;
    time_t    sec;

    GetSystemTime(&system_time);
    SystemTimeToFileTime(&system_time, &file_time);
    time = ((uint64_t)file_time.dwLowDateTime);
    time += ((uint64_t)file_time.dwHighDateTime) << 32;

    sec = (uint32_t)((time - EPOCH) / 10000000L);

    wcsftime(buf2, 20, L"%Y-%m-%d %H:%M:%S", localtime(&sec));

    swprintf_s(buf1, L"%s.%03d", buf2, system_time.wMilliseconds);

    return buf1;
}

unsigned int mtime_tick()
{
    return GetTickCount();
}

#else
char *mtime2s(char *buf)
{
    static char buf1[128];

    char buf2[64];
    struct timeval tv;
    time_t sec;

    gettimeofday(&tv, NULL);
    sec = tv.tv_sec;

    strftime(buf2, 20, "%Y-%m-%d %H:%M:%S", localtime(&sec));

    sprintf(buf1, "%s.%03d", buf2, tv.tv_usec / 1000);

    return buf1;
}

wchar_t *mtime2sW(wchar_t *buf)
{
    return NULL;
}

unsigned int mtime_tick()
{
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return (unsigned int)(t.tv_nsec / 1000000);
}
#endif
