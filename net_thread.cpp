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
        CTcpHandler *pHandler = (CTcpHandler *)(arg);
        pHandler->TcpRead();
    }

    void CNetThread::OnStaticWrite(struct bufferevent *pBufevt, void *arg)
    {
        CTcpHandler *pHandler = (CTcpHandler *)(arg);
        pHandler->TcpWrite();
    }

    void CNetThread::OnStaticError(struct bufferevent *pBufevt, short iTemp, void *arg)
    {
        CTcpHandler *pHandler = (CTcpHandler *)(arg);
        pHandler->TcpDelete(iTemp); 
    }

    //////////////////////////////////////////////////////////
    //                CListenThread
    /////////////////////////////////////////////////////////
    CListenThread::CListenThread(int32_t iThreadType, int32_t iThreadId)
        :CEventThread(iThreadType, iThreadId)
    {

    }

    void CListenThread::OnStaticRead(struct evconnlistener *pListener, evutil_socket_t iAcceptHandler, 
                               struct sockaddr *pAddr, int socklen, void *pArg)		
    {
       CListenHandler *pHandler = (CListenHandler *) (pArg); 
       pHandler->TcpListenAccept(iAcceptHandler, pAddr, socklen);
    }
};

