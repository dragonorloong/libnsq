/*************************************************************************
	> File Name: timer_thread.h
	> Author: 
	> Mail: 
	> Created Time: 2015年10月25日 星期日 21时05分04秒
 ************************************************************************/

#ifndef _TIMER_THREAD_H
#define _TIMER_THREAD_H
#include "nsq_thread.h"
#include "event2/event.h"
#include <string>

namespace NSQTOOL
{
    class CTimerThread:public CThread
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

        CTimerThread()
        {
            pthread_mutex_init(&m_mutex, NULL);
        }

        ~CTimerThread()
        {
            pthread_mutex_destroy(&m_mutex);
        }

		int32_t Init(int32_t iThreadType, int32_t iThreadId, void *pArg);
		void RealRun();
		void RealProcessCmd(CCommand &cCmd);
        static void OnStaticTimeOut(int iHandle, short iEvent, void *pArg);
        void OnTimeOut(int iHandle, short iEvent, void *pArg);

    private: 
        event_base *m_pEventBase;
        std::map<std::string, STimerInfo *> m_mapTimer;
        pthread_mutex_t m_mutex;
    };
}
#endif
