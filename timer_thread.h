/*************************************************************************
	> File Name: timer_thread.h
	> Author: 
	> Mail: 
	> Created Time: 2015年10月25日 星期日 21时05分04秒
 ************************************************************************/

#ifndef _TIMER_THREAD_H
#define _TIMER_THREAD_H
#include "thread.h"
#include "event2/event.h"
#include <string>
#include "event_thread.h"

namespace NSQTOOL
{
    class CTimerThread:public CEventThread
    {
    public:
        struct STimerInfo
        {
            event *m_pEvent;
            int32_t m_iDstType;
            int32_t m_iDstTid;
            int32_t m_iCmdType; //每一个线程都可以有多个定时器 
            int32_t m_iPersist; //1代表持续事件，0代表一次性定时器
            struct timeval m_cTimeval;
            CTimerThread *m_pThis;
        };

        CTimerThread(int32_t iThreadType, int32_t iThreadId);

        ~CTimerThread()
        {
            pthread_mutex_destroy(&m_mutex);
        }

		void RealProcessCmd(CCommand &cCmd);
        static void OnStaticTimeOut(int iHandle, short iEvent, void *pArg);
        void OnTimeOut(int iHandle, short iEvent, void *pArg);
    private: 
        std::map<std::string, STimerInfo *> m_mapTimer;
        pthread_mutex_t m_mutex;
    };
}
#endif
