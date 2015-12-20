#include "tcp_handler.h"
#include "nsqd_handler.h"

namespace NSQTOOL
{
        CNsqdHandler::CNsqdHandler(int iProtocolType, int iProtocolId,
                uint64_t iHandlerId, CThread *pThread)
            :CTcpHandler(iProtocolType, iProtocolId, iHandlerId, pThread)
        {

        }

        void CNsqdHandler::OnConnect()
        {
            
        }

        void CNsqdHandler::OnError(int iErrorNo)
        {
            GetThread()->DestoryHandler(GetHandleId());          

            if (m_pListenHandler != NULL)
            {
                 
            }
        }

        void CNsqdHandler::ProcessRead()
        {
             
        }
};
