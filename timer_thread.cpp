/*************************************************************************
	> File Name: timer_thread.cpp
	> Author: 
	> Mail: 
	> Created Time: 2015年10月25日 星期日 21时05分32秒
 ************************************************************************/

#include "timer_thread.h"

namespace NSQTOOL
{

CTimerThread::CTimerThread(int32_t iThreadType, int32_t iThreadId)
    : CEventThread(iThreadType, iThreadId)
{
    pthread_mutex_init(&m_mutex, NULL);
}

void CTimerThread::OnStaticTimeOut(int iHandle, short iEvent, void *pArg)
{
    CTimerThread *pThis = dynamic_cast<CTimerThread *>(((CTimerAddCommand *)pArg)->m_pThread);
    pThis->OnTimeOut(iHandle, iEvent, pArg);
}

void CTimerThread::OnTimeOut(int iHandle, short iEvent, void *pArg)
{
    pthread_mutex_lock(&m_mutex);
    CTimerAddCommand  *pTimerAddCommand = (CTimerAddCommand *)pArg;
    char buff[64] = {0};
    snprintf(buff, sizeof(buff), "%d_%d_%d", 
            pTimerAddCommand->GetAddr().m_cSrcAddr.m_iThreadType, pTimerAddCommand->GetAddr().m_cSrcAddr.m_iThreadId, pTimerAddCommand->m_iTimerType);
	NsqLogPrintf(LOG_DEBUG, "OnTimerOut:%s\n", buff);
    
    CCmdAddr cAddr;
    cAddr.m_cDstAddr.m_iThreadType = pTimerAddCommand->GetAddr().m_cSrcAddr.m_iThreadType;
    cAddr.m_cDstAddr.m_iThreadId = pTimerAddCommand->GetAddr().m_cSrcAddr.m_iThreadId;
    cAddr.m_cDstAddr.m_iHandlerId = pTimerAddCommand->GetAddr().m_cSrcAddr.m_iHandlerId;

    CCommand *pCmd = new CCommand(pTimerAddCommand->m_iTimerType, -1);
    pCmd->SetAddr(cAddr);
    CThreadMgrSingleton::GetInstance()->PostCmd(pCmd);

    if ((pTimerAddCommand->m_iPersist == 0) && (m_mapTimer.find(buff) != m_mapTimer.end()))
    {
        evtimer_del(m_mapTimer[buff]->m_pEvent);
        event_free(m_mapTimer[buff]->m_pEvent);
        delete m_mapTimer[buff];
        m_mapTimer.erase(buff);
    }

    pthread_mutex_unlock(&m_mutex);
}

void CTimerThread::TimerAdd(CCommand *pCmd)
{
    CTimerAddCommand *pTimerAddCommand = dynamic_cast<CTimerAddCommand *>(pCmd);
    event *pEvent = event_new(m_pEventBase, -1, EV_PERSIST, OnStaticTimeOut,pTimerAddCommand);
    evtimer_add(pEvent, &(pTimerAddCommand->m_cTimeval));

    char buff[64] = {0};
    snprintf(buff, sizeof(buff), "%d_%d_%d", 
            pTimerAddCommand->GetAddr().m_cSrcAddr.m_iThreadType, pTimerAddCommand->GetAddr().m_cSrcAddr.m_iThreadId, pTimerAddCommand->m_iTimerType);
	NsqLogPrintf(LOG_DEBUG, "TimerAdd:%s\n", buff);
    pTimerAddCommand->m_pEvent = pEvent;
    pTimerAddCommand->m_pThread = this;
    m_mapTimer[buff] = pTimerAddCommand;
  
}

void CTimerThread::RealProcessCmd(CCommand *pCmd)
{
    pthread_mutex_lock(&m_mutex);

    switch(pCmd->GetCmdType())
    {
        case TIMER_ADD_TYPE:
        {
            TimerAdd(pCmd);
    		pthread_mutex_unlock(&m_mutex);
            return ;
        }
        case TIMER_DEL_TYPE:
        {
        }
    }

    pthread_mutex_unlock(&m_mutex);
}

};
