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
        CTimerThread(int32_t iThreadType, int32_t iThreadId);
        void TimerAdd(CCommand *pCmd);
 //       void TimerDel(CCommand *pCmd);
//        void KeepAlive(CCommand *pCmd);

        ~CTimerThread()
        {
            pthread_mutex_destroy(&m_mutex);
        }

		void RealProcessCmd(CCommand *pCmd);
        static void OnStaticTimeOut(int iHandle, short iEvent, void *pArg);
        void OnTimeOut(int iHandle, short iEvent, void *pArg);
    private: 
        std::map<std::string, CTimerAddCommand *> m_mapTimer;
        pthread_mutex_t m_mutex;
    };
}
#endif
