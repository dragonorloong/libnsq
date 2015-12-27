/*************************************************************************
	> File Name: event_thread.h
	> Author: EastQ
	> Mail: log2013@163.com
	> Created Time: 2015年12月24日 星期四 20时12分54秒
	> Last Modifiede: 2015年12月24日 星期四 20时12分54秒
 ************************************************************************/

#ifndef _EVENT_THREAD_H
#define _EVENT_THREAD_H
#include "thread.h"
#include "event2/event.h"

namespace NSQTOOL
{
    struct SNetCallBackContext
    {
        int32_t m_iThreadType;
        int32_t m_iThreadId;
        uint64_t m_iHandlerId;
    };

    class CEventThread:public CThread
    {
    public:
        CEventThread(int32_t iThreadType, int32_t iThreadId);
        ~CEventThread();
        void NotifyWait();
        void RealRun();
        event_base *GetEventBase()
        {
            return m_pEventBase; 
        }

    protected:
        event_base *m_pEventBase;
    };
};
#endif
