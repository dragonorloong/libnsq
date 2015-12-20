#include "tcp_handler.h"
#include "thread.h"
#include "listen_handler.h"

namespace NSQTOOL
{
    CListenHandler::CListenHandler(int iProtocolType, int iProtocolId, 
            uint64_t iHandlerId, CThread *pThread)
            :CHandler(iHandlerId, pThread)
            , m_iProtocolType(iProtocolType), m_iProtocolId(iProtocolId)

    {
         
    }

    void CListenHandler::OnError(CTcpHandler *pHandler, int iErrorNo)
    {
        if (pHandler != NULL)
        {
            m_mapTcpHandlerMgr.erase(pHandler->GetHandlerId()); 
        }
    }

    void CListenHandler::OnAccept(CTcpHandler *pHandler)
    {
        m_mapTcpHandlerMgr[pHandler->GetHandlerId()] = pHandler;    
    }

    void CListenHandler::ProcessCmd(CCommand &cCmd)
    {
        
    }

};
