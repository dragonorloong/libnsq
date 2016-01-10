#include "common.h"
#include "event2/event.h"
namespace NSQTOOL
{
    //短连接超时
    int g_iShortConnectTimeout = 300*1000;   //300ms

    //长连接超时
    int g_iLongConnectTimeout = 30*1000*1000; //30s

    //线程cmd队列长度
    int g_iCmdQueueLength = 10000;

    //命令处理时间
    int g_iCmdProcessTime = 30; //30ms

    //线程死锁时间
    int g_iThreadDeadLockTime = 10 * 1000; //10s

    int64_t GetTvToMsc(const struct timeval *tv)
    {
        return (tv->tv_sec * 1000) + ((tv->tv_usec + 999) / 1000);
    }

    //获取当前时间与参数的差值，单位ms
    int64_t GetIntervalNow(const struct timeval *pTimeval)
    {
        int64_t iInTimeMs = GetTvToMsc(pTimeval); 
        struct timeval cCurTime;
        gettimeofday(&cCurTime, NULL);  
        int64_t iCurTimeMs = GetTvToMsc(&cCurTime);  

        return iCurTimeMs - iInTimeMs;
    }
};
