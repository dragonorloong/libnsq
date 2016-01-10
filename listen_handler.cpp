#include "tcp_handler.h"
#include "net_thread.h"
#include "listen_handler.h"
#include <string.h>

namespace NSQTOOL
{
    CListenHandler::CListenHandler(int iCmdType, int iCmdId, 
            uint64_t iHandlerId, CThread *pThread)
            :CHandler(iHandlerId, pThread)
            , m_iCmdType(iCmdType), m_iCmdId(iCmdId)
    {
         
    }

    CListenHandler::~CListenHandler()
    {
       evconnlistener_free(m_pListener);
    }

    void CListenHandler::TcpListenAdd(CCommand *pCmd)
    {
        CTcpListenAddCommand *pListenAddCmd = dynamic_cast<CTcpListenAddCommand *>(pCmd); 
        CListenThread *pListenThread = dynamic_cast<CListenThread *>(GetThread());

        sockaddr_in sAddr;
        memset(&sAddr, 0, sizeof(sockaddr_in));
        sAddr.sin_addr.s_addr = inet_addr(pListenAddCmd->m_strHost.c_str());
        sAddr.sin_port = htons(pListenAddCmd->m_iPort);
        sAddr.sin_family = AF_INET;

        CAddr *pAddr = new CAddr();
        pAddr->m_iThreadType = GetThread()->GetThreadType();
        pAddr->m_iThreadId = GetThread()->GetThreadId();
        pAddr->m_iHandlerId = GetHandlerId();

        m_pListener = evconnlistener_new_bind(pListenThread->GetEventBase(), CListenThread::OnStaticRead, pAddr, 
                LEV_OPT_THREADSAFE|LEV_OPT_REUSEABLE|LEV_OPT_CLOSE_ON_FREE, 10,
                (sockaddr*)&sAddr, sizeof(sockaddr_in));


    }

    void CListenHandler::TcpListenAccept(CCommand *pCmd)
    {
        CTcpListenAcceptCommand *pListenAccecptCommand = dynamic_cast<CTcpListenAcceptCommand *>(pCmd); 
        CTcpAddCommand *pTcpAddCommand = new CTcpAddCommand(pListenAccecptCommand->m_iAcceptFd, m_iCmdId);
        GetThread()->PostRemoteCmd(pTcpAddCommand, NET_THREAD_TYPE, -1, -1, GetHandlerId());
    }

    void CListenHandler::ProcessCmd(CCommand *pCmd)
    {
       switch(pCmd->GetCmdType())     
       {
            case TCP_LISTEN_ADD_TYPE:
            {
                TcpListenAdd(pCmd);
                break;
            }
            case TCP_LISTEN_DEL_TYPE:
            {
                TcpListenDel(pCmd); 
                break;
            }
            case TCP_LISTEN_ACCEPT_TYPE:
            {
                TcpListenAccept(pCmd); 
                break;
            }
       }

       delete pCmd;
    }

};
