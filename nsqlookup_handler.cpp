#include "tcp_handler.h"
#include "thread.h"
#include "factory.h"
#include "net_thread.h"
#include "nsqlookup_handler.h"
#include "main_thread.h"
#include "nsqlookup_protocol.h"

namespace NSQTOOL
{
        CNsqLookupHandler::CNsqLookupHandler(int iProtocolType, int iProtocolId,
                uint64_t iHandleId, CThread *pThread)
                : CTcpHandler(iProtocolType, iProtocolId, iHandleId, pThread)
        {
        }

        void CNsqLookupHandler::OnConnect()
        {
            CNsqLookupContext cLookup = CMainThread::LookupConnectCallBack(m_iProtocolId);

            string strUrl = "/lookup?topic=";
            strUrl += cLookup.m_strTopic;

            char buff[10] = {0};
            snprintf(buff, sizeof(buff), "%u", cLookup.m_iLookupPort);

            string strAddr = cLookup.m_strLookupHost;
            strAddr += ":";
            strAddr += buff;

            CHttpRequest cHttpRequest;
            cHttpRequest.SetRequest(strUrl.c_str(), strAddr.c_str());
            dynamic_cast<CNetThread*>(GetThread())->SendData(GetBufferevent(), 
                    &cHttpRequest.Encode(), true);
            printf("NSQLOOKUP_HANDLER: Connect After SendData = %s\n", cHttpRequest.Encode().c_str());
        }

        int CNsqLookupHandler::ProcessRead()
        {
            CNsqLookupResponse *pProtocol = dynamic_cast<CNsqLookupResponse *>(m_pProtocol);

            CMainThread::LookupReadCallBack(m_iProtocolId, 
                                               pProtocol->GetChannels(), 
                                               pProtocol->GetProducers());

            GetThread()->DestoryHandler(GetHandlerId());
            //DestoryHandler以后，就不能再调用对象的任何信息了，已经析构了自己
            return -1; 
        }
};
