#include "net_thread.h"
#include <string.h>
#include "protocol.h"
#include "handler.h"
#include "tcp_handler.h"
#include "listen_handler.h"

namespace NSQTOOL
{
    CNetThread::CNetThread(int32_t iThreadType, int32_t iThreadId)
        :CEventThread(iThreadType, iThreadId)
    {

    }

    void CNetThread::OnStaticRead(struct bufferevent *pBufevt, void *arg)
    {
        CAddr *pAddr = (CAddr *)arg;
        CTcpReadCommand *pCmd = new CTcpReadCommand();
        CCmdAddr cCmdAddr;
        cCmdAddr.m_cDstAddr = *pAddr;
        pCmd->SetAddr(cCmdAddr);
        CThreadMgrSingleton::GetInstance()->PostCmd(pCmd);
	    NsqLogPrintf(LOG_DEBUG, "OnStaticRead ThreadType = %d, ThreadId = %d, HandlerId = %ld", 
		pAddr->m_iThreadType, pAddr->m_iThreadId, pAddr->m_iHandlerId);
    }

    void CNetThread::OnStaticError(struct bufferevent *pBufevt, short iTemp, void *arg)
    {
        CAddr *pAddr = (CAddr *)arg;
        CTcpDelCommand *pCmd = new CTcpDelCommand(iTemp);
        CCmdAddr cCmdAddr;
        cCmdAddr.m_cDstAddr = *pAddr;
        pCmd->SetAddr(cCmdAddr);
        CThreadMgrSingleton::GetInstance()->PostCmd(pCmd);
    }

    //////////////////////////////////////////////////////////
    //                CListenThread
    /////////////////////////////////////////////////////////
    CListenThread::CListenThread(int32_t iThreadType, int32_t iThreadId)
        :CEventThread(iThreadType, iThreadId)
    {

    }

    void CListenThread::OnStaticRead(struct evconnlistener *pListener, evutil_socket_t iAcceptHandle, 
                               struct sockaddr *pAddr, int socklen, void *pArg)		
    {
        CAddr *pDstAddr = (CAddr *)pArg;
        CTcpListenAcceptCommand *pCmd = new CTcpListenAcceptCommand(iAcceptHandle);
        CCmdAddr cCmdAddr;
        cCmdAddr.m_cDstAddr = *pDstAddr;
        pCmd->SetAddr(cCmdAddr);
        CThreadMgrSingleton::GetInstance()->PostCmd(pCmd);
    }
};

