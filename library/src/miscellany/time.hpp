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
	static int64_t now() {//��λ��ms
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
    stTime.tm_year = wtm.wYear - 1900;/* ��ݣ���ֵ����ʵ����ݼ�ȥ1900 */
    stTime.tm_mon = wtm.wMonth - 1;/* �·ݣ���һ�¿�ʼ��0����һ�£� - ȡֵ����Ϊ[0,11] */
    stTime.tm_mday = wtm.wDay; /* һ�����е����� - ȡֵ����Ϊ[1,31] */
    stTime.tm_hour = wtm.wHour; /* ʱ - ȡֵ����Ϊ[0,23] */
    stTime.tm_min = wtm.wMinute; /* �� - ȡֵ����Ϊ[0,59] */
    stTime.tm_sec = wtm.wSecond; /* �� �C ȡֵ����Ϊ[0,59] */
    stTime.tm_isdst = -1;/* ����ʱ��ʶ����ʵ������ʱ��ʱ��tm_isdstΪ������ʵ������ʱ�Ľ���tm_isdstΪ0�����˽����ʱ��tm_isdst()Ϊ����*/
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