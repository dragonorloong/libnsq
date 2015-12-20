#include "tcp_handler.h"
#include "nsqlookup_handler.h"

namespace NSQTOOL
{
        CNsqLookupHandler::CNsqLookupHandler(int iProtocolType, int iProtocolId,
                uint64_t iHandleId, CThread *pThread)
                : CTcpHandler(iProtocolType, iProtocolId, iHandleId, pThread)
        {
            m_pProtocol = CSingletonNsqFactory::GetInstance()->GentProtocol(
                    iProtocolType); 
            m_pListenHandler = NULL;
        }

        void CNsqLookupHandler::OnConnect()
        {
                        
        }

        void CTcpHandler::ProcessRead()
        {
             
        }
};
