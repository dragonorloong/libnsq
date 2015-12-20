#include "tcp_handler.h"
#include "listen_handler.h"

namespace NSQTOOL
{
    CListennHandler::CListenHandler(int iProtocolType, int iProtocolId, 
            uint64_t iHandlerId, CThread *pThread)
            :CHandler(iHandlerId, pThread)
            , m_iProtocolType(iProtocolType), m_iProtocolId(iProtocolId)

    {
         
    }

    void CListennHandler::OnError(CTcpHandler *pHandler = NULL, int iErrorNo = 0)
    {
        if (pHandler != NULL)
        {
            m_mapTcpHandlerMgr.erase(pHandler->GetHandlerId()); 
        }


    }

    void CListennHandler::OnAccept(CTcpHandler *pHandler)
    {
        m_mapTcpHandlerMgr[pHandler->GetHandlerId()] = pHandler;    
    }

    void CListennHandler::ProcessCmd(CCommand &cCmd)
    {
        
    }

};
