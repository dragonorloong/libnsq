/*************************************************************************
	> File Name: main_thread.cpp
	> Author: 
	> Mail: 
	> Created Time: 2015年10月25日 星期日 22时41分23秒
 ************************************************************************/

#include "http_server.h"
#include "timer_thread.h"
#include "net_thread.h"
#include "stdlib.h"
#include <algorithm>
#include <iostream>
#include <vector>
#include <stdarg.h>
#include "http_factory.h"

namespace NSQTOOL
{
    //启动函数，假如需要使用网络，定时器等基础代码，可以到这里修改代码
    void CHttpServer::StartSuperServer()
    {
        CThreadMgrSingleton::GetInstance()->RegisterThreadPool(
                new CThreadPool(TIMER_THREAD_TYPE, 1));

        CThreadMgrSingleton::GetInstance()->RegisterThreadPool(
                new CThreadPool(NET_THREAD_TYPE, 10));

        CThreadMgrSingleton::GetInstance()->RegisterThreadPool(
                new CThreadPool(LISTEN_THREAD_TYPE, 1));

        CThreadMgrSingleton::GetInstance()->Run();

        CTcpListenAddCommand *pCmd = new CTcpListenAddCommand("0.0.0.0", 12198, HTTP_TEST);
        CCmdAddr cCmdAddr;
        cCmdAddr.m_cSrcAddr.m_iThreadType = NSQ_THREAD_TYPE;
        cCmdAddr.m_cSrcAddr.m_iThreadId = 0;
        cCmdAddr.m_cDstAddr.m_iThreadType = LISTEN_THREAD_TYPE;
        pCmd->SetAddr(cCmdAddr);
        CThreadMgrSingleton::GetInstance()->PostCmd(pCmd);
    }

    void CHttpServer::StopSuperServer()
    {
        CThreadMgrSingleton::GetInstance()->Stop(); 
        CThreadMgrSingleton::ReleaseInstance();
    }
    
    void CHttpServer::InitSuperServer(int iThreadNum, 
                                    int iLogLevel, 
                                    LOGCALLBACK pLogFunc
                                    )
    {
        CHttpFactory *pFactory = new CHttpFactory();
        CFactory::SetFactory(pFactory);
        g_iLogLevel = iLogLevel;
        g_pLogFunc = pLogFunc;
        srand(::time(NULL));
    }
};
