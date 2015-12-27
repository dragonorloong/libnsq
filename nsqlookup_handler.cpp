#include "tcp_handler.h"
#include "thread.h"
#include "factory.h"
#include "net_thread.h"
#include "nsqlookup_handler.h"
#include "main_thread.h"
#include "nsqlookup_protocol.h"
#include <algorithm>

namespace NSQTOOL
{
        CNsqLookupHandler::CNsqLookupHandler(int iCmdType, int iCmdId,
                uint64_t iHandleId, CThread *pThread)
                : CTcpHandler(iCmdType, iCmdId, iHandleId, pThread)
        {
        }

        void CNsqLookupHandler::OnConnect()
        {
            string strUrl = "/lookup?topic=";
            strUrl += m_strTopic;

            char buff[10] = {0};
            snprintf(buff, sizeof(buff), "%u", m_iPort);

            string strAddr = m_strHost;
            strAddr += ":";
            strAddr += buff;

            CHttpRequest cHttpRequest;
            cHttpRequest.SetRequest(strUrl.c_str(), strAddr.c_str());
            SendData(cHttpRequest.Encode().c_str(), cHttpRequest.Encode().length());
            NsqLogPrintf(LOG_DEBUG, "NSQLOOKUP_HANDLER: Connect After SendData = %s\n", cHttpRequest.Encode().c_str());
        }

        int CNsqLookupHandler::ProcessRead()
        {
            CNsqLookupResponse *pProtocol = dynamic_cast<CNsqLookupResponse *>(m_pProtocol);

            if (find(pProtocol->GetChannels().begin(), pProtocol->GetChannels().end(), m_strChannel) == pProtocol->GetChannels().end() && !m_strChannel.empty())
            {
                //消费者没有对应的channel，直接返回    
                GetThread()->DestoryHandler(GetHandlerId());
                return -1;
            }


            CMainThread::LookupReadCallBack(m_strTopic, m_strChannel,
                                            pProtocol->GetChannels(), 
                                            pProtocol->GetProducers());

            GetThread()->DestoryHandler(GetHandlerId());
            //DestoryHandler以后，就不能再调用对象的任何信息了，已经析构了自己
            return -1; 
        }

        void CNsqLookupHandler::ProcessCmd(CCommand *pCmd)
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
