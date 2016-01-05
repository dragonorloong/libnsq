/*************************************************************************
	> File Name: event_thread.cpp
	> Author: EastQ
	> Mail: log2013@163.com
	> Created Time: 2015年12月24日 星期四 20时17分40秒
	> Last Modifiede: 2015年12月24日 星期四 20时17分40秒
 ************************************************************************/

#include "event_thread.h"

namespace NSQTOOL
{
    CEventThread::CEventThread(int32_t iThreadType, int32_t iThreadId)
        :CThread(iThreadType, iThreadId)
    {
        m_pEventBase = event_base_new();			
    }

    CEventThread::~CEventThread()
    {
        event_base_free(m_pEventBase);
    }

    void CEventThread::NotifyWait()
    {
        pthread_mutex_lock(&m_mutex);
        event_base_loopbreak(m_pEventBase); 
        pthread_mutex_unlock(&m_mutex);
        CThread::NotifyWait();
    }

    void CEventThread::RealRun()
    {
        while (!m_bStop)	
        {
            CThread::ProcessCmd();

            struct timeval sTm; 
            sTm.tv_sec =0;
            sTm.tv_usec = 1000;
            event_base_loopexit(m_pEventBase, &sTm);	
           // pthread_mutex_lock(&m_mutex);
            event_base_dispatch(m_pEventBase);
            //pthread_mutex_unlock(&m_mutex);
        }
    }
};

