/*************************************************************************
	> File Name: main_thread.cpp
	> Author: 
	> Mail: 
	> Created Time: 2015年10月25日 星期日 22时41分23秒
 ************************************************************************/

#include "main_thread.h"
#include "timer_thread.h"
#include "net_thread.h"
#include "stdlib.h"
#include "nsqd_protocol.h"
#include <algorithm>
#include <iostream>
#include <vector>
#include <stdarg.h>

namespace NSQTOOL
{
    void CMainThread::OnTimeOut()
    {
        CGuard<CLock> cGuard(&m_cLock);
        map<string, CNsqLookupContext>::iterator iter = 
            m_mapLookupContext.begin();

        for (; iter != m_mapLookupContext.end(); ++iter)
        {
            CNsqLookupCommand *pCmd = new CNsqLookupCommand(
                    iter->second.m_strTopic,
                    iter->second.m_strChannel,
                    iter->second.m_strLookupHost, 
                    iter->second.m_iLookupPort);

            CCmdAddr cCmdAddr;
            cCmdAddr.m_cDstAddr.m_iThreadType = NET_THREAD_TYPE;
            pCmd->SetAddr(cCmdAddr);
            CThreadMgrSingleton::GetInstance()->PostCmd(pCmd);
        }
    }

    //持续发现
    void CMainThread::RealProcessCmd(CCommand *pCmd)
    {
       switch(pCmd->GetCmdType()) 
       {
           case LOOKUP_TIMER:
           {
                OnTimeOut(); 
                break;
           }
           default:
           {
                CThread::RealProcessCmd(pCmd);
                break;
           }
       }
    }

    map<string, vector<CHandlerContext> > CMainThread::m_mapTopic2Handler;
    map<string, CNsqLookupContext>  CMainThread::m_mapLookupContext;
    int CMainThread::m_iConnectNum = 2;
    CLock CMainThread::m_cLock;
    LOGCALLBACK CMainThread::m_pLogFunc = NULL;
    int CMainThread::m_iLogLevel = LOG_DEBUG;
    int CMainThread::m_iThreadNum = 1;

    void NsqLogPrintf(int iLogLevel, const char *pFormat, ...)
    {
        if (iLogLevel < CMainThread::m_iLogLevel || CMainThread::m_pLogFunc == NULL)
        {
            return ;
        }

        va_list va;
        va_start(va, pFormat);
        char buff[256] = {0};
        vsnprintf(buff, sizeof(buff), pFormat, va);
        va_end(va);
        CMainThread::m_pLogFunc(iLogLevel, buff);
    }

    void CMainThread::SetProducer(const string &strLookupHost,
                                uint16_t iLookupPort, 
                                const string &strTopic,
                                BIZCALLBACK pFunc
                                )
    {
        CGuard<CLock> cGuard(&m_cLock);
        CNsqLookupContext cLookup;
        cLookup.m_strLookupHost = strLookupHost;
        cLookup.m_iLookupPort = iLookupPort;
        cLookup.m_strTopic = strTopic;
        cLookup.m_funcCallBack = pFunc;

        m_mapLookupContext[strTopic] = cLookup;
    }

    void CMainThread::SetConsumer(const string &strLookupHost, 
                                uint16_t iLookupPort,
                                const string &strTopic, 
                                const string &strChannel,
                                BIZCALLBACK pFunc 
                                )
    {
        CGuard<CLock> cGuard(&m_cLock);
        CNsqLookupContext cLookup;
        cLookup.m_strLookupHost = strLookupHost;
        cLookup.m_iLookupPort = iLookupPort;
        cLookup.m_strTopic = strTopic;
        cLookup.m_strChannel = strChannel;
        cLookup.m_funcCallBack = pFunc;

        m_mapLookupContext[strTopic + "_" + strChannel] = cLookup;
    }

    void CMainThread::InitSuperServer(int iThreadNum, 
                                    int iConnectNum, 
                                    int iLogLevel, 
                                    LOGCALLBACK pLogFunc
                                    )
    {
        m_iThreadNum = iThreadNum;
        m_iConnectNum = iConnectNum;
        m_iLogLevel = iLogLevel;
        m_pLogFunc = pLogFunc;
        srand(::time(NULL));
    }

    void CMainThread::StartSuperServer()
    {
        CThreadMgrSingleton::GetInstance()->RegisterThreadPool(
                new CThreadPool(NET_THREAD_TYPE, m_iThreadNum));

        CThreadMgrSingleton::GetInstance()->RegisterThreadPool(
                new CThreadPool(TIMER_THREAD_TYPE, 1));

        CThreadMgrSingleton::GetInstance()->RegisterThreadPool(
                new CThreadPool(MAIN_THREAD_TYPE, 1));

        CThreadMgrSingleton::GetInstance()->Run();

        struct timeval cTimeval = {15, 0};

        CTimerAddCommand *pCmd = new CTimerAddCommand(cTimeval, 1, LOOKUP_TIMER);
        CCmdAddr cCmdAddr;
        cCmdAddr.m_cSrcAddr.m_iThreadType = MAIN_THREAD_TYPE;
        cCmdAddr.m_cSrcAddr.m_iThreadId = 0;
        cCmdAddr.m_cDstAddr.m_iThreadType = TIMER_THREAD_TYPE;
        pCmd->SetAddr(cCmdAddr);

        CThreadMgrSingleton::GetInstance()->PostCmd(pCmd);
        OnTimeOut();
    }

    void CMainThread::NewNsqdConnect(const string &strTopic, const string &strChannel, 
            const string &strHost, uint16_t iPort)
    {
        CGuard<CLock> cGuard(&m_cLock);
        vector<CHandlerContext> &vecHandlerContext = m_mapTopic2Handler[strTopic + "_" + strChannel];
        vector<CHandlerContext>::iterator iter = vecHandlerContext.begin();
        int iConnectNum = 0;

        for (; iter != vecHandlerContext.end(); ++ iter)
        {
           if (iter->m_strNsqdHost == strHost && iter->m_iNsqdPort == iPort) 
           {
                iConnectNum ++;
           }
        }

        while ((m_iConnectNum - iConnectNum) > 0)
        {
            CNsqdCommand *pCmd = new CNsqdCommand(
                   strTopic, strChannel, strHost, iPort); 
            CCmdAddr cCmdAddr;
            cCmdAddr.m_cDstAddr.m_iThreadType = NET_THREAD_TYPE;
            pCmd->SetAddr(cCmdAddr);
            CThreadMgrSingleton::GetInstance()->PostCmd(pCmd);
        }
    }

    void CMainThread::LookupReadCallBack(const string &strTopic, const string &strChannel,
                        const vector<string> &vecChannels, const vector<CNsqLookupResponse::SProducers> &vecProducers)
    {
        CGuard<CLock> cGuard(&m_cLock);

        vector<CNsqLookupResponse::SProducers>::const_iterator iter = 
            vecProducers.begin();

        for (; iter != vecProducers.end(); ++ iter)
        {
            NewNsqdConnect(strTopic, strChannel, 
                        iter->m_strBroadcastAddres, iter->m_iTcpPort);    
        }

    }

    void CMainThread::NsqdConnectCallBack(
            CAddr &cAddr, const string &strNsqdHost
            , const uint16_t iPort, const string &strTopic
            , const string &strChannel)

    {
        CGuard<CLock> cGuard(&m_cLock);
        CHandlerContext cHandlerContext;
        cHandlerContext.m_cAddr = cAddr;
        cHandlerContext.m_strNsqdHost = strNsqdHost;
        cHandlerContext.m_iNsqdPort = iPort;
        cHandlerContext.m_strTopic = strTopic;
        cHandlerContext.m_strChannel = strChannel;
        m_mapTopic2Handler[strTopic + "_" + strChannel].push_back(cHandlerContext);
    }

    void CMainThread::NsqdErrorCallBack(CAddr &cAddr
            , const string &strTopic
            , const string &strChannel)
    {
        CGuard<CLock> cGuard(&m_cLock);

        string strKey = strTopic + "_" + strChannel;
        vector<CHandlerContext> &vecHandlerContext = m_mapTopic2Handler[strKey];
        vector<CHandlerContext>::iterator iter = vecHandlerContext.begin();

        for (; iter != vecHandlerContext.end(); ++iter)
        {
            if (iter->m_cAddr.m_iThreadType == cAddr.m_iThreadType && 
                iter->m_cAddr.m_iThreadId == cAddr.m_iThreadId &&
                iter->m_cAddr.m_iHandlerId == cAddr.m_iHandlerId )
            {
                vecHandlerContext.erase(iter); 
                break; 
            }    
        }
    }

    void CMainThread::NsqdReadCallBack(const string &strTopic, 
                                       const string &strChannel,
                                       const string &strMsgId, 
                                       const string &strMsgBody)
    {
        CGuard<CLock> cGuard(&m_cLock);
        map<string, CNsqLookupContext>::iterator iter = m_mapLookupContext.begin();

        if (m_mapLookupContext.find(strTopic + "_" + strChannel) != m_mapLookupContext.end())
        {
            m_mapLookupContext[strTopic + "_" + strChannel].m_funcCallBack(-1, strMsgId, strMsgBody);
        }
    }

    int CMainThread::ProducerMsg(const string &strTopic, const string &strMsg)
    {
        CGuard<CLock> cGuard(&m_cLock);

        if (m_mapTopic2Handler[strTopic + "_"].size() == 0)
        {
            return -1;
        }

        int iIndex = rand() % m_mapTopic2Handler[strTopic + "_"].size();
        CAddr cAddr = m_mapTopic2Handler[strTopic + "_"][iIndex].m_cAddr;  
        
        //这里应该用异步消息的模式，后续改掉
        CNsqdRequest cNsqdRequest;
        cNsqdRequest.PubLish(strTopic, strMsg, strMsg.length());
        char *pData = new char[cNsqdRequest.Encode().length()];
        memcpy(pData, cNsqdRequest.Encode().c_str(), cNsqdRequest.Encode().length());
        CTcpSendCommand *pCommand = new CTcpSendCommand(pData, cNsqdRequest.Encode().length());
        pCommand->GetAddr().m_cDstAddr = cAddr;
        CThreadMgrSingleton::GetInstance()->PostCmd(pCommand);
        return 0;
    }
};
