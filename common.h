#ifndef __COMMON_H_
#define __COMMON_H_
#include <stdint.h>
#include <sys/time.h>

namespace NSQTOOL
{
    enum EConnectKeepType
    {
        ESHORTCONNECT = 0,
        ELONGCONNECT = 1,
    };

    extern int g_iShortConnectTimeout ;   //300ms
    extern int g_iLongConnectTimeout ; //30s
    extern int g_iCmdQueueLength;

    using namespace std;
    void NsqLogPrintf(int iLogLevel, const char *pFormat, ...);

    //命令处理时间
    extern int g_iCmdProcessTime;

    //线程死锁时间
    extern int g_iThreadDeadLockTime;

    int64_t GetTvToMsc(const struct timeval *tv);

    //获取当前时间与参数的差值，单位ms
    int64_t GetIntervalNow(const struct timeval *pTimeval);

	enum EInternalCmdType
	{
		STOP_TYPE = -1,
		TCP_CONNECT_TYPE = -2,
		TCP_LISTEN_ADD_TYPE = -3,
        TCP_LISTEN_DEL_TYPE = -4,
        TCP_LISTEN_ACCEPT_TYPE = -11,
		TCP_DEL_TYPE = -5,
		TCP_ADD_TYPE = -6,
		TCP_SEND_TYPE = -7,
        TCP_READ_TYPE = -8, 
        TIMER_ADD_TYPE = -9,
        TIMER_DEL_TYPE = -10
	};

    enum EInternalThreadType
    {
        NET_THREAD_TYPE = -1,
        LISTEN_THREAD_TYPE = -2,
        TIMER_THREAD_TYPE = -3,
        NSQ_THREAD_TYPE = -4,
    };

    enum ENsqProtocolType
    {
        NSQLOOKUP_TYPE = -1,
        NSQD_TYPE = -2,
    };

    enum LOG_LEVEL
    {
        LOG_DEBUG = -3,
        LOG_ERROR = -2
    };

};
#endif
