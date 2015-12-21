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
            CLookupContext cLookup = CMainThread::GetLookupContext(m_iProtocolId);
            CHttpRequest cHttpRequest;
            string strUrl = "/lookup?topic=";
            strUrl += cLookup.m_strTopic;
            string strAddr = cLookup.m_strHost;
            strAddr += ":";
            strAddr += itoa(cLookup.m_iPort);
            cHttpRequest.SetRequest(strUrl, strAddr);
            dynamic_cast<CNetThread*>(GetThread())->SendData(GetBufferevent(), 
                    &cHttpRequest.Encode());
        }

        int CNsqLookupHandler::ProcessRead()
        {
            
        }

};
