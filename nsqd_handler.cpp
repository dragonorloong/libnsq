#include "tcp_handler.h"
#include "thread.h"
#include "nsqd_handler.h"
#include "main_thread.h"
#include "nsqd_protocol.h"
#include "net_thread.h"

namespace NSQTOOL
{
        CNsqdHandler::CNsqdHandler(int iCmdType, int iCmdId,
                uint64_t iHandlerId, CThread *pThread)
            :CTcpHandler(iCmdType, iCmdId, iHandlerId, pThread)
        {

        }

        void CNsqdHandler::OnConnect()
        {
            CAddr cAddr;
            cAddr.m_iThreadType = GetThread()->GetThreadType();
            cAddr.m_iThreadId = GetThread()->GetThreadId();
            cAddr.m_iHandlerId = GetHandlerId();
            CMainThread::NsqdConnectCallBack(cAddr, m_strHost, m_iPort, m_strTopic, m_strChannel);

	        CNsqdRequest cNsqdRequest;
            cNsqdRequest.Megic();	

            //消费者
            if (!m_strChannel.empty())
            {
                cNsqdRequest.Subscribe(m_strTopic, m_strChannel);                
	            cNsqdRequest.Ready(100);
            }

            SendData(cNsqdRequest.Encode().c_str(), cNsqdRequest.Encode().length());
        }

        void CNsqdHandler::OnError(int iErrorNo)
        {
            CAddr cAddr;
            cAddr.m_iThreadType = GetThread()->GetThreadType();
            cAddr.m_iThreadId = GetThread()->GetThreadId();
            cAddr.m_iHandlerId = GetHandlerId();
            CMainThread::NsqdErrorCallBack(cAddr, m_strTopic, m_strChannel);
            GetThread()->DestoryHandler(GetHandlerId());          
        }

        int CNsqdHandler::ProcessRead()
        {	
            CNsqdResponse *pProtocol = dynamic_cast<CNsqdResponse *>(m_pProtocol);

            if (pProtocol->GetFrameType() == CNsqdResponse::FrameTypeResponse)
            {
                if (pProtocol->GetResponce() ==  "_heartbeat_")
                {
                    CNsqdRequest cNsqdRequest;
                    cNsqdRequest.Nop();
                    SendData(cNsqdRequest.Encode().c_str(), cNsqdRequest.Encode().length());
                }

            }
            else if (pProtocol->GetFrameType() == CNsqdResponse::FrameTypeMessage)
            {
                std::string &strMsgId = pProtocol->GetMsgId();
                std::string &strBody = pProtocol->GetMsgBody();
                //直接返回FIN，可能会丢包
                CNsqdRequest cNsqdRequest;
                cNsqdRequest.Finish(strMsgId);
                SendData(cNsqdRequest.Encode().c_str(), cNsqdRequest.Encode().length());
                CMainThread::NsqdReadCallBack(m_strTopic, m_strChannel, strMsgId, strBody);
            }

            return 0;
        }

        void CNsqdHandler::ProcessCmd(CCommand *pCmd)
        {
            switch(pCmd->GetCmdType()) 
            {
                case TCP_CONNECT_TYPE:
                {
                    CNsqLookupCommand *pLookupCommand = dynamic_cast<CNsqLookupCommand *>(pCmd);  
                    m_strTopic = pLookupCommand->m_strTopic;
                    m_strChannel = pLookupCommand->m_strChannel;
                }
                break;
            }

            CTcpHandler::ProcessCmd(pCmd);
        }
};
