#include "tcp_handler.h"
#include "thread.h"
#include "nsqd_handler.h"
#include "main_thread.h"
#include "nsqd_protocol.h"
#include "net_thread.h"

namespace NSQTOOL
{
        CNsqdHandler::CNsqdHandler(int iProtocolType, int iProtocolId,
                uint64_t iHandlerId, CThread *pThread)
            :CTcpHandler(iProtocolType, iProtocolId, iHandlerId, pThread)
        {

        }

        void CNsqdHandler::OnConnect()
        {
            CHandlerContext &cHandlerContext = CMainThread::NsqdConnectCallBack(m_iProtocolId, this);
            cHandlerContext.m_pHandler = this;

            fprintf(stdout, "NSQD_HANDLER OnConnect topic = %s, channel = %s\n", 
                    cHandlerContext.m_strTopic.c_str(), 
                    cHandlerContext.m_strChannel.c_str());

            //消费者
            if (!cHandlerContext.m_strChannel.empty())
            {
	            CNsqdRequest cNsqdRequest;
            	cNsqdRequest.Megic();	
                cNsqdRequest.Subscribe(cHandlerContext.m_strTopic, cHandlerContext.m_strChannel);                
	            cNsqdRequest.Ready(100);
                dynamic_cast<CNetThread*>(GetThread())->SendData(GetBufferevent(), 
                    &cNsqdRequest.Encode(), true);

                fprintf(stdout, "NSQD_HANDLER OnConnect SendData = %s\n", cNsqdRequest.Encode().c_str());
            }
            
        }

        void CNsqdHandler::OnError(int iErrorNo)
        {
            CMainThread::NsqdErrorCallBack(m_iProtocolId);
            GetThread()->DestoryHandler(GetHandlerId());          

            if (m_pListenHandler != NULL)
            {
                 
            }
        }

        int CNsqdHandler::ProcessRead()
        {	
            CNsqdResponse *pProtocol = dynamic_cast<CNsqdResponse *>(m_pProtocol);

            if (pProtocol->GetFrameType() == CNsqdResponse::FrameTypeResponse)
            {
                if (pProtocol->GetResponce() ==  "_heartbeat_")
                {
                    fprintf(stdout, "_hearbeat_\n");
                    CNsqdRequest cNsqdRequest;
                    cNsqdRequest.Nop();
                    dynamic_cast<CNetThread*>(GetThread())->SendData(GetBufferevent(), 
                                                                     &cNsqdRequest.Encode(), true);
                }

                fprintf(stdout, "response = %s\n", pProtocol->GetResponce().c_str());
            }
            else if (pProtocol->GetFrameType() == CNsqdResponse::FrameTypeMessage)
            {
                std::string &strMsgId = pProtocol->GetMsgId();
                std::string &strBody = pProtocol->GetMsgBody();
                fprintf(stdout, "msg:%s\n", strBody.c_str());
                CNsqdRequest cNsqdRequest;
                cNsqdRequest.Finish(strMsgId);
                dynamic_cast<CNetThread*>(GetThread())->SendData(GetBufferevent(), 
                    &cNsqdRequest.Encode(), true);
                CMainThread::NsqdReadCallBack(m_iProtocolId, strMsgId, strBody);
            }

            return 0;
        }
};
