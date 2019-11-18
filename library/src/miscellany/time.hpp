#ifndef _TIME_HPP_
#define _TIME_HPP_

#ifdef __unix__
#include <sys/time.h>
#elif _WIN32
#include <time.h>
#endif

inline int gettimeofday(struct timeval *TimeNow_F, void* TimeZone_F);

class Time
{
public:
	static int64_t now() {//单位，ms
		return nowMicro() / 1000;
	}

	static int64_t nowMicro() {
		timeval stTv;
		gettimeofday(&stTv, NULL);
		return (int64_t(stTv.tv_sec) * 1000000 + stTv.tv_usec);
	}
};

int gettimeofday(struct timeval *TimeNow_F, void *TimeZone_F)
{
    time_t clock;
    struct tm stTime;
    SYSTEMTIME wtm;
    GetLocalTime(&wtm);
    stTime.tm_year = wtm.wYear - 1900;/* 年份，其值等于实际年份减去1900 */
    stTime.tm_mon = wtm.wMonth - 1;/* 月份（从一月开始，0代表一月） - 取值区间为[0,11] */
    stTime.tm_mday = wtm.wDay; /* 一个月中的日期 - 取值区间为[1,31] */
    stTime.tm_hour = wtm.wHour; /* 时 - 取值区间为[0,23] */
    stTime.tm_min = wtm.wMinute; /* 分 - 取值区间为[0,59] */
    stTime.tm_sec = wtm.wSecond; /* 秒 – 取值区间为[0,59] */
    stTime.tm_isdst = -1;/* 夏令时标识符，实行夏令时的时候，tm_isdst为正。不实行夏令时的进候，tm_isdst为0；不了解情况时，tm_isdst()为负。*/
    clock = mktime(&stTime);
    TimeNow_F->tv_sec = (long)clock;
    TimeNow_F->tv_usec = wtm.wMilliseconds * 1000;

    return 0;
}

inline void usleep(uint32_t us)
{
    Sleep(us / 1000);
}

#endif /* _TIME_HPP_ */