#include "tcp_handler.h"

namespace NSQTOOL
{
        CTcpHandler::CTcpHandler(int iProtocolType, int iProtocolId,
                uint64_t iHandlerId, CThread *pThread)
                :CHandler(iHandlerId, pThread)
                , m_iProtocolType(iProtocolType)
                , m_iProtocolId(iProtocolId)
        {
            m_pProtocol = CSingletonNsqFactory::GetInstance()->GentProtocol(
                    iProtocolType); 
            m_pListenHandler = NULL;
        }

        void CTcpHandler::OnConnect()
        {
            
        }

        void CTcpHandler::OnError(int iErrorNo)
        {
            GetThread()->DestoryHandler(GetHandleId());          

            if (m_pListenHandler != NULL)
            {
                 
            }
        }

        int CTcpHandler::OnRead(const char *pData, int iLength)
        {
            int iNeed = m_pProtocol->Need(pData, iLength); 

            while (iNeed == 0)
            {
                m_pProtocol->Decode();
                ProcessRead(); 
                m_pProtocol->NextPkg();
                iNeed = m_pProtocol->Need(NULL, 0);
            }

            return iNeed;
        }

        void CTcpHandler::ProcessRead()
        {
             
        }
};
