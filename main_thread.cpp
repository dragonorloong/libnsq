/*************************************************************************
	> File Name: main_thread.cpp
	> Author: 
	> Mail: 
	> Created Time: 2015年10月25日 星期日 22时41分23秒
 ************************************************************************/

#include "main_thread.h"
#include "timer_thread.h"
#include "net_thread.h"

namespace NSQTOOL
{
    void CMainThread::RealProcessCmd(CCommand &cCmd)
    {
       switch(cCmd.GetCmdType()) 
       {
           case LOOKUP_TIMER:
           {
                fprintf(stdout, "LOOKUP_TIMER\n");
                pthread_mutex_lock(&m_mutex);

                map<int, CNsqLookupContext>::iterator iter = 
                    m_mapLookupContext.begin();

                for (; iter != m_mapLookupContext.end(); ++iter)
                {
                    CCommand cmd(NET_CONNECT_TYPE);
                    CNetThread::SNetContext *pNetContext = new CNetThread::SNetContext;
                    pNetContext->m_strHost = iter->second.m_strHost;
                    pNetContext->m_iPort = iter->second.m_iPort;
                    pNetContext->m_iProtocolType = NSQLOOKUP_TYPE;
                    pNetContext->m_iProtocolId = iter->first;
                    cmd.SetLData(pNetContext);
                    CCommand::CCmdAddr cCmdAddr;
                    cCmdAddr.m_iDstTid = -1;
                    cCmdAddr.m_iDstType = NET_THREAD_TYPE;
                    cmd.SetAddr(cCmdAddr);
                    CThreadMgrSingleton::GetInstance()->PostCmd(NET_THREAD_TYPE, cmd);
                }

                pthread_mutex_unlock(&m_mutex);
                break;
           }
           default:
           {
                CThread::RealProcessCmd(cCmd);
                break;
           }
       }
    }

    map<string, list<CHandlerContext> > CMainThread::m_mapTopic2Handler;
    map<int, string> CMainThread::m_mapHandler2Topic;
    list<CNsqLookupContext> CMainThread::m_lstLookup;
    int CMainThread::m_iConnectNum = 5;
    int CMainThread::m_iProtocolId = 0;
    pthread_mutex_t CMainThread::m_mutex;

    void CMainThread::SetConnectNum(int iConnectNum)
    {
        m_iConnectNum = iConnectNum; 
    }

    void CMainThread::SetProducer(const string &strLookupHost,
                                uint16_t iLookupPort, 
                                const string &strTopic 
                                )
    {
        CNsqLookupContext cLookup;
        cLookup.m_strLookupHost = strLookupHost;
        cLookup.m_iLookupPort = iLookupPort;
        cLookup.m_strTopic = strTopic;

        m_mapLookupContext[++m_iProtocolId] = cLookup;
    }

    void CMainThread::SetConsumer(const string &strLookupHost, 
                                uint16_t iLookupPort,
                                const string &strTopic, 
                                const string &strChannel 
                                )
    {
        CNsqLookupContext cLookup;
        cLookup.m_strLookupHost = strLookupHost;
        cLookup.m_iLookupPort = iLookupPort;
        cLookup.m_strTopic = strTopic;
        cLookup.m_strChannel = strChannel;

        m_mapLookupContext[++m_iProtocolId] = cLookup;
    }


    void CMainThread::StartSuperServer()
    {
        pthread_mutex_init(&m_mutex, NULL); 
        m_iProtocolId = 0;

        CThreadMgrSingleton::GetInstance()->RegisterThreadPool(
                new CThreadPool(NET_THREAD_TYPE, 5));

        CThreadMgrSingleton::GetInstance()->RegisterThreadPool(
                new CThreadPool(TIMER_THREAD_TYPE, 1));

        CThreadMgrSingleton::GetInstance()->RegisterThreadPool(
                new CThreadPool(MAIN_THREAD_TYPE, 1));

        CThreadMgrSingleton::GetInstance()->Run();

        CTimerThread::STimerInfo *pTimerInfo = new CTimerThread::STimerInfo();

        struct timeval cTimeval = {15, 0};
        pTimerInfo->m_cTimeval = cTimeval;

        pTimerInfo->m_iDstTid = 0;
        pTimerInfo->m_iDstType = MAIN_THREAD_TYPE;
        pTimerInfo->m_iPersist = 1; 
        pTimerInfo->m_iCmdType = LOOKUP_TIMER;

        CCommand::CCmdAddr cCmdAddr;
        cCmdAddr.m_iDstTid = 0;
        cCmdAddr.m_iDstType = TIMER_THREAD_TYPE;

        CCommand cmd(TIMER_ADD_TYPE);
        cmd.SetAddr(cCmdAddr);
        cmd.SetLData(pTimerInfo);

        CThreadMgrSingleton::GetInstance()->PostCmd(TIMER_THREAD_TYPE, cmd);
    }

    CNsqLookupContext CMainThread::GetLookupContext(int iProtocolId)
    {
        return m_mapLookupContext[iProtocolId];
    }
};
