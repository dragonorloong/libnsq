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
    //持续发现
    void CMainThread::RealProcessCmd(CCommand &cCmd)
    {
       switch(cCmd.GetCmdType()) 
       {
           case LOOKUP_TIMER:
           {
                CGuard<CLock> cGuard(&m_cLock);
                map<int, CNsqLookupContext>::iterator iter = 
                    m_mapLookupContext.begin();

                for (; iter != m_mapLookupContext.end(); ++iter)
                {
                    CCommand cmd(NET_CONNECT_TYPE);
                    CNetThread::SNetContext *pNetContext = new CNetThread::SNetContext;
                    pNetContext->m_strHost = iter->second.m_strLookupHost;
                    pNetContext->m_iPort = iter->second.m_iLookupPort;
                    pNetContext->m_iProtocolType = NSQLOOKUP_TYPE;
                    pNetContext->m_iProtocolId = iter->first;
                    cmd.SetLData(pNetContext);
                    CCommand::CCmdAddr cCmdAddr;
                    cCmdAddr.m_iDstTid = -1;
                    cCmdAddr.m_iDstType = NET_THREAD_TYPE;
                    cmd.SetAddr(cCmdAddr);
                    CThreadMgrSingleton::GetInstance()->PostCmd(NET_THREAD_TYPE, cmd);
                }

                break;
           }
           default:
           {
                CThread::RealProcessCmd(cCmd);
                break;
           }
       }
    }

    map<string, vector<CHandlerContext> > CMainThread::m_mapTopic2Handler;
    map<int, string> CMainThread::m_mapHandler2Topic;
    map<int, CNsqLookupContext>  CMainThread::m_mapLookupContext;
    int CMainThread::m_iConnectNum = 2;
    int CMainThread::m_iProtocolId = 0;
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

        m_mapLookupContext[++m_iProtocolId] = cLookup;
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

        m_mapLookupContext[++m_iProtocolId] = cLookup;
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
        m_iProtocolId = 0;
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

        CTimerThread::STimerInfo *pTimerInfo = new CTimerThread::STimerInfo();

        struct timeval cTimeval = {15, 0};
        pTimerInfo->m_cTimeval = cTimeval;

        pTimerInfo->m_iDstTid = 0;
        pTimerInfo->m_iDstType = MAIN_THREAD_TYPE;
        pTimerInfo->m_iPersist = 0; 
        pTimerInfo->m_iCmdType = LOOKUP_TIMER;

        CCommand::CCmdAddr cCmdAddr;
        cCmdAddr.m_iDstTid = -1;
        cCmdAddr.m_iDstType = TIMER_THREAD_TYPE;

        CCommand cmd(TIMER_ADD_TYPE);
        cmd.SetAddr(cCmdAddr);
        cmd.SetLData(pTimerInfo);

        CThreadMgrSingleton::GetInstance()->PostCmd(TIMER_THREAD_TYPE, cmd);
    }

    CNsqLookupContext &CMainThread::LookupConnectCallBack(int iProtocolId)
    {
        CGuard<CLock> cGuard(&m_cLock);
        fprintf(stdout, "LookupConnectCallBack iProtocolId = %d\n", iProtocolId);
        return m_mapLookupContext[iProtocolId];
    }

    void CMainThread::NewNsqdConnect(const string &strTopic, const string &strChannel, 
            const string &strHost, uint16_t iPort)
    {
        CGuard<CLock> cGuard(&m_cLock);
        fprintf(stdout, "NewNsqdConnect:Topic = %s, Channel = %s\n", strTopic.c_str(), strChannel.c_str());
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
            CCommand cmd(NET_CONNECT_TYPE);
            CNetThread::SNetContext *pNetContext = new CNetThread::SNetContext;
            pNetContext->m_strHost = strHost;
            pNetContext->m_iPort = iPort;
            pNetContext->m_iProtocolType = NSQD_TYPE;
            pNetContext->m_iProtocolId = ++m_iProtocolId;
            cmd.SetLData(pNetContext);
            CCommand::CCmdAddr cCmdAddr;
            cCmdAddr.m_iDstTid = -1;
            cCmdAddr.m_iDstType = NET_THREAD_TYPE;
            cmd.SetAddr(cCmdAddr);
            CThreadMgrSingleton::GetInstance()->PostCmd(NET_THREAD_TYPE, cmd);           
            m_mapHandler2Topic[m_iProtocolId] = strTopic + "_" + strChannel;
           
            CHandlerContext cHandlerContext;
            cHandlerContext.m_iProtocolId = m_iProtocolId;
            cHandlerContext.m_strNsqdHost = strHost;
            cHandlerContext.m_iNsqdPort = iPort;
            cHandlerContext.m_pHandler = NULL;
            cHandlerContext.m_strTopic = strTopic;
            cHandlerContext.m_strChannel = strChannel;
            m_mapTopic2Handler[strTopic+"_"+strChannel].push_back(cHandlerContext);
            iConnectNum ++;
            fprintf(stdout, "NewNsqdConnect iPorotocolId = %d\n", m_iProtocolId);
        }

    }

    void CMainThread::LookupReadCallBack(int iProtocolId, const vector<string> &vecChannels, 
                        const vector<CNsqLookupResponse::SProducers> &vecProducers)
    {
        CGuard<CLock> cGuard(&m_cLock);
        fprintf(stdout, "LookupReadCallBack iPorotocolId = %d\n", iProtocolId);

        fprintf(stdout, "LookupReadCallBack topic = %s, channel = %s\n", 
                m_mapLookupContext[iProtocolId].m_strTopic.c_str(), 
                m_mapLookupContext[iProtocolId].m_strChannel.c_str());

        if (find(vecChannels.begin(), vecChannels.end(), 
             m_mapLookupContext[iProtocolId].m_strChannel) == vecChannels.end() &&
            !m_mapLookupContext[iProtocolId].m_strChannel.empty())
        {
            //消费者没有对应的channel，直接返回    
            return;
        }

        vector<CNsqLookupResponse::SProducers>::const_iterator iter = 
            vecProducers.begin();

        for (; iter != vecProducers.end(); ++ iter)
        {
            NewNsqdConnect(m_mapLookupContext[iProtocolId].m_strTopic,
                        m_mapLookupContext[iProtocolId].m_strChannel, 
                        iter->m_strBroadcastAddres, iter->m_iTcpPort);    
        }

    }

    CHandlerContext &CMainThread::NsqdConnectCallBack(int iProtocolId, CTcpHandler *pHandler)
    {
        CGuard<CLock> cGuard(&m_cLock);
        fprintf(stdout, "NsqdConnectCallBack iPorotocolId = %d\n", iProtocolId);
        string strKey = m_mapHandler2Topic[iProtocolId];
        fprintf(stdout, "NsqdConnectCallBack key = %s\n", strKey.c_str());

        vector<CHandlerContext> &vecHandlerContext = m_mapTopic2Handler[strKey];
        vector<CHandlerContext>::iterator iter = vecHandlerContext.begin();

        for (; iter != vecHandlerContext.end(); ++iter)
        {
            if (iter->m_iProtocolId == iProtocolId)
            {
                iter->m_pHandler = pHandler;
                fprintf(stdout, "NsqdConnectCallBack found\n");
                return *iter;
            }    
        }
    }

    void CMainThread::NsqdErrorCallBack(int iProtocolId)
    {
        CGuard<CLock> cGuard(&m_cLock);

        string strKey = m_mapHandler2Topic[iProtocolId];

        vector<CHandlerContext> &vecHandlerContext = m_mapTopic2Handler[strKey];
        vector<CHandlerContext>::iterator iter = vecHandlerContext.begin();

        for (; iter != vecHandlerContext.end(); ++iter)
        {
            if (iter->m_iProtocolId == iProtocolId)
            {
                vecHandlerContext.erase(iter); 
                break; 
            }    
        }
    }

    void CMainThread::NsqdReadCallBack(int iProtocolId, 
                                       const string &strMsgId, 
                                       const string &strMsgBody)
    {
        CGuard<CLock> cGuard(&m_cLock);
        map<int, CNsqLookupContext>::iterator iter = m_mapLookupContext.begin();

        for (; iter != m_mapLookupContext.end(); ++iter)
        {
            if ((iter->second.m_strTopic + "_" + iter->second.m_strChannel) == 
              m_mapHandler2Topic[iProtocolId])    
            {
               iter->second.m_funcCallBack(iProtocolId, strMsgId, strMsgBody); 
            }
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
        CTcpHandler *pHandler = m_mapTopic2Handler[strTopic + "_"][iIndex].m_pHandler;  
        
        //这里应该用异步消息的模式，后续改掉
        CNsqdRequest cNsqdRequest;
        cNsqdRequest.PubLish(strTopic, strMsg, strMsg.length());
        dynamic_cast<CNetThread *>(pHandler->GetThread())->SendData(pHandler->GetBufferevent(), &cNsqdRequest.Encode(), true);
        return 0;
    }
};
