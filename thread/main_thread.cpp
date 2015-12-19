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
                    CCommand cmd(NET_CONNECT_TYPE);
                    CNetThread::SNetContext *pNetContext = new CNetThread::SNetContext;
                    pNetContext->m_strHost = "127.0.0.1";
                    pNetContext->m_iPort = 4161;
                    pNetContext->m_iPkgType = 3;
                    pNetContext->m_cAddr.m_iDstType = NET_THREAD_TYPE;
                    pNetContext->m_cAddr.m_iDstTid = 0;
                    cmd.SetLData(pNetContext);
                    CCommand::CCmdAddr cCmdAddr;
                    cCmdAddr.m_iDstTid = 0;
                    cCmdAddr.m_iDstType = NET_THREAD_TYPE;
                    cmd.SetAddr(cCmdAddr);
                    CThreadMgrSingleton::GetInstance()->PostCmd(NET_THREAD_TYPE, cmd, 0);
               }
               break;
            default:
               CThread::RealProcessCmd(cCmd);
               break;
       }
    }

    void CMainThread::RealRun()
    {
        CThreadPool<CNetThread> *pNetThreadPool = new CThreadPool<CNetThread>();
        pNetThreadPool->Init(NET_THREAD_TYPE, 1, NULL);
        CThreadMgrSingleton::GetInstance()->RegisterThreadPool(pNetThreadPool);

        CThreadPool<CTimerThread> *pTimerThreadPool = new CThreadPool<CTimerThread>();
        pTimerThreadPool->Init(TIMER_THREAD_TYPE, 1, NULL);
        CThreadMgrSingleton::GetInstance()->RegisterThreadPool(pTimerThreadPool);

        CThreadPool<CMainThread> *pMainThreadPool = new CThreadPool<CMainThread>();
        pMainThreadPool->Add(MAIN_THREAD_TYPE, this);
        CThreadMgrSingleton::GetInstance()->RegisterThreadPool(pMainThreadPool);
        
        CTimerThread::STimerInfo *pTimerInfo = new CTimerThread::STimerInfo();
        pTimerInfo->m_iDstTid = 0;
        pTimerInfo->m_iDstType = MAIN_THREAD_TYPE;
        pTimerInfo->m_iPersist = 1; 
        pTimerInfo->m_iCmdType = LOOKUP_TIMER;
        struct timeval cTimeval = {15, 0};
        pTimerInfo->m_cTimeval = cTimeval;
        CCommand cmd(TIMER_ADD_TYPE);
        CCommand::CCmdAddr cCmdAddr;
        cCmdAddr.m_iDstTid = 0;
        cCmdAddr.m_iDstType = TIMER_THREAD_TYPE;
        cmd.SetAddr(cCmdAddr);
        cmd.SetLData(pTimerInfo);
        CThreadMgrSingleton::GetInstance()->PostCmd(TIMER_THREAD_TYPE, cmd, 0);
        CThreadMgrSingleton::GetInstance()->Run();
        CThread::RealRun();
    }
};
