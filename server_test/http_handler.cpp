#include "tcp_handler.h"
#include "http_handler.h"
#include "http_protocol.h"
#include <string.h>
#include "thread.h"

namespace NSQTOOL
{
        CHttpHandler::CHttpHandler(int iCmdType, int iCmdId,
                uint64_t iHandlerId, CThread *pThread, int iConnType)
            :CTcpHandler(iCmdType, iCmdId, iHandlerId, pThread, iConnType)
        {

        }

        void CHttpHandler::OnConnect()
        {
/*            CAddr cAddr;
            cAddr.m_iThreadType = GetThread()->GetThreadType();
            cAddr.m_iThreadId = GetThread()->GetThreadId();
            cAddr.m_iHandlerId = GetHandlerId();
            CNsqThread::NsqdConnectCallBack(cAddr, m_strHost, m_iPort, m_strTopic, m_strChannel);

	        CHttpRequest CHttpRequest;
            CHttpRequest.Megic();	

            //消费者
            if (!m_strChannel.empty())
            {
                CHttpRequest.Subscribe(m_strTopic, m_strChannel);                
	            CHttpRequest.Ready(1000);
            }

            SendData(CHttpRequest.Encode().c_str(), CHttpRequest.Encode().length());
            */
        }

        void CHttpHandler::OnError(int iErrorNo)
        {
            /*CAddr cAddr;
            cAddr.m_iThreadType = GetThread()->GetThreadType();
            cAddr.m_iThreadId = GetThread()->GetThreadId();
            cAddr.m_iHandlerId = GetHandlerId();
            CNsqThread::NsqdErrorCallBack(cAddr, m_strTopic, m_strChannel);
            */

            GetThread()->DestoryHandler(GetHandlerId());          
        }

        int CHttpHandler::ProcessRead()
        {	
            CHttpRequest *pProtocol = dynamic_cast<CHttpRequest *>(m_pProtocol);

            NsqLogPrintf(LOG_DEBUG, "body = %s", pProtocol->GetBody().c_str());

            CHttpResponse cHttpResponse;
            cHttpResponse.SetResponse("200 ok\r\nContent-Type: text/html");

            
            char *buff = new char[1024 * 10];
            char *body = new char[1024*1];
            memset(body, '1', 1*1024);
            body[1*1024-1] = '\0';
            //NsqLogPrintf(LOG_DEBUG, "Send Response body = %s", body);
            snprintf(buff, 10*1024, "HTTP/1.1 200 ok\r\nContent-Type: text/html\r\n"
                                "Connection: Keep-Alive\r\nContent-Length: %ld\r\n\r\n%s\r\n", 
                                strlen(body)+ 2, body);

//            NsqLogPrintf(LOG_DEBUG, "Send Response = %s", cHttpResponse.Encode().c_str());
            NsqLogPrintf(LOG_DEBUG, "Send Response = %s", buff);
            SendData(buff, strlen(buff));
            delete [] buff;
            delete [] body;
            return 0;
        }

        void CHttpHandler::ProcessCmd(CCommand *pCmd)
        {
            CTcpHandler::ProcessCmd(pCmd);
        }
};
