#include "tcp_handler.h"
#include "thread.h"
#include "factory.h"
#include "net_thread.h"
#include "nsqlookup_handler.h"

namespace NSQTOOL
{
        CNsqLookupHandler::CNsqLookupHandler(int iProtocolType, int iProtocolId,
                uint64_t iHandleId, CThread *pThread)
                : CTcpHandler(iProtocolType, iProtocolId, iHandleId, pThread)
        {
            m_pProtocol = CSingletonNsqFactory::GetInstance()->GenProtocol(
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
