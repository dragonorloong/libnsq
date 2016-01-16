/*************************************************************************
	> File Name: event_thread.cpp
	> Author: EastQ
	> Mail: log2013@163.com
	> Created Time: 2015年12月24日 星期四 20时17分40秒
	> Last Modifiede: 2015年12月24日 星期四 20时17分40秒
 ************************************************************************/

#include "event_thread.h"
#include "event2/thread.h"

namespace NSQTOOL
{
    static void OnStaticTimeOut(int iHandle, short iEvent, void *pArg)
    {

    }

    CEventThread::CEventThread(int32_t iThreadType, int32_t iThreadId)
        :CThread(iThreadType, iThreadId)
    {
        evthread_use_pthreads();
        m_pEventBase = event_base_new();			
        struct timeval sTm; 
        sTm.tv_sec =630720000;
        sTm.tv_usec = 0;
        event *pEvent = event_new(m_pEventBase, -1, EV_PERSIST, &OnStaticTimeOut, NULL);
        evtimer_add(pEvent, &sTm);
         
    }

    CEventThread::~CEventThread()
    {
        event_base_free(m_pEventBase);
    }

    void CEventThread::NotifyWait()
    {
        pthread_mutex_lock(&m_mutex);
        event_base_loopexit(m_pEventBase, NULL); 
        pthread_mutex_unlock(&m_mutex);
        CThread::NotifyWait();
    }

    void CEventThread::RealRun()
    {
        while (!m_bStop)	
        {
            CThread::ProcessCmd();

            struct timeval sTm; 
            sTm.tv_sec =1000;
            sTm.tv_usec = 1000;
            event_base_loop(m_pEventBase, EVLOOP_ONCE);	
           // pthread_mutex_lock(&m_mutex);
            //event_base_dispatch(m_pEventBase);
//            NsqLogPrintf(LOG_DEBUG, "dispatch return thread_type = %d, thread_id =%d\n", GetThreadType(), GetThreadId());
            //pthread_mutex_unlock(&m_mutex);
        }
    }
};

