#include "tcp_handler.h"
#include "net_thread.h"
#include "protocol.h"
#include <stdio.h>
#include <string.h>

namespace NSQTOOL
{
        CTcpHandler::CTcpHandler(int iCmdType, int iCmdId,
                uint64_t iHandlerId, CThread *pThread)
                :CHandler(iHandlerId, pThread)
                , m_iCmdType(iCmdType)
                , m_iCmdId(iCmdId)
        {
            m_pProtocol = CSingletonNsqFactory::GetInstance()->GenProtocol(
                    iCmdType, iCmdId); 
        }

        CTcpHandler::~CTcpHandler()
        {
            close(bufferevent_getfd(GetBufferevent()));
            bufferevent_free(GetBufferevent()); 
           // CThread::DestoryHandler(iHandlerId);
        }

        void CTcpHandler::OnConnect()
        {
            
        }

        void CTcpHandler::OnError(int iErrorNo)
        {
            GetThread()->DestoryHandler(GetHandlerId());          
        }

        int CTcpHandler::OnRead(const char *pData, int iLength)
        {
            int iNeed = m_pProtocol->Need(pData, iLength); 
		NsqLogPrintf(LOG_DEBUG, "OnRead iNeed = %d", iNeed);	
            if (iLength == 0)
            {
            }

            int i = 0;

            while (iNeed == 0)
            {
                m_pProtocol->Decode();
                if (ProcessRead() != 0)
                {
                    return -1;     
                }

                m_pProtocol->NextPkg();
                iNeed = m_pProtocol->Need(NULL, 0);

            }

            return iNeed;
        }

        int CTcpHandler::ProcessRead()
        {
             
        }

        void CTcpHandler::TcpRead(CCommand *pCmd)
        {
            //内存copy三次，太浪费，后续改进
            int iLength = evbuffer_get_length(bufferevent_get_input(m_pBufevt));
            char *pData = new char[iLength + 1];
            bufferevent_read(m_pBufevt, pData, iLength);			
            pData[iLength] = '\0';
		NsqLogPrintf(LOG_DEBUG, "TcpRead iLength = %d\n", iLength);

            //iNeedLength 证明包处理出错，已经析构掉了handler相关的一些
            int iNeedLength = OnRead(pData, iLength); 

            if (iNeedLength > 0)
            {
                bufferevent_setwatermark(m_pBufevt, EV_READ, iNeedLength, 0);
            }

            delete pData;
        }

        void CTcpHandler::TcpDelete(CCommand *pCmd)
        {
            CTcpDelCommand *pDelCmd = dynamic_cast<CTcpDelCommand *>(pCmd);

            if (pDelCmd->m_iErrorType & BEV_EVENT_CONNECTED)
            {
                OnConnect();
            }
            else
            {
                OnError(pDelCmd->m_iErrorType); 
            }
        }

        void CTcpHandler::SendData(const char *pData, int32_t iLength)
        {
            int iRet = bufferevent_write(m_pBufevt, pData, 
                    iLength);	
             
            if (iRet != 0)
            {
                 
            }
        }

        void CTcpHandler::TcpSend(CCommand *pCmd)
        {
            CTcpSendCommand *pSendCommand = dynamic_cast<CTcpSendCommand *>(pCmd);

            int iRet = bufferevent_write(m_pBufevt, pSendCommand->m_pSendData, 
                    pSendCommand->m_iLength);	

            if (iRet != 0)
            {
            
            }

            delete []  pSendCommand->m_pSendData;
        }

        void CTcpHandler::TcpConnect(CCommand *pCmd)
        {
		NsqLogPrintf(LOG_DEBUG, "TcpConnect\n");
            CTcpConnectCommand *pConnectCmd = dynamic_cast<CTcpConnectCommand *>(pCmd);
            m_strHost = pConnectCmd->m_strHost;
            m_iPort = pConnectCmd->m_iPort;
            sockaddr_in sAddr;
            memset(&sAddr, 0, sizeof(sockaddr_in));
            sAddr.sin_addr.s_addr = inet_addr(pConnectCmd->m_strHost.c_str());
            sAddr.sin_port = htons(pConnectCmd->m_iPort);
            sAddr.sin_family = AF_INET;

            CNetThread *pThread = dynamic_cast<CNetThread *>(GetThread()); 
            m_pBufevt = bufferevent_socket_new(pThread->GetEventBase(), -1, 0);
            int32_t iRet = bufferevent_socket_connect(m_pBufevt, (sockaddr*)&sAddr, sizeof(sockaddr_in));
             
            if (iRet != 0)
            {
                OnError(errno); 
                return ;
            }

            CAddr *pAddr = new CAddr();
            pAddr->m_iThreadType = GetThread()->GetThreadType();
            pAddr->m_iThreadId = GetThread()->GetThreadId();
            pAddr->m_iHandlerId = GetHandlerId();
		NsqLogPrintf(LOG_DEBUG, "OnConnect ThreadType = %d, ThreadId = %d, HandlerId = %ld\n",
				pAddr->m_iThreadType, pAddr->m_iThreadId, pAddr->m_iHandlerId);

            bufferevent_setcb(m_pBufevt, CNetThread::OnStaticRead, NULL, CNetThread::OnStaticError, pAddr);
            bufferevent_enable(m_pBufevt, EV_READ|EV_PERSIST);		

            //设置读入最低水位，防止无效回调
            bufferevent_setwatermark(m_pBufevt, EV_READ, 
                                     OnRead(NULL, 0), 0);
        }
        

        void CTcpHandler::TcpAdd(CCommand *pCmd)
        {
            CAddr *pAddr = new CAddr();
            pAddr->m_iThreadType = GetThread()->GetThreadType();
            pAddr->m_iThreadId = GetThread()->GetThreadId();
            pAddr->m_iHandlerId = GetHandlerId();
 
            CTcpAddCommand *pConnectCmd = dynamic_cast<CTcpAddCommand *>(pCmd);
            CEventThread *pThread = dynamic_cast<CEventThread *>(GetThread()); 
            m_pBufevt = bufferevent_socket_new(pThread->GetEventBase(), 
                    pConnectCmd->m_iFd, 0);
            bufferevent_setcb(m_pBufevt, CNetThread::OnStaticRead, NULL, CNetThread::OnStaticError, pAddr);
            bufferevent_enable(m_pBufevt, EV_READ|EV_WRITE|EV_PERSIST|EV_ET);		
            bufferevent_setwatermark(m_pBufevt, EV_READ, 
                                      OnRead(NULL, 0), 0);
            OnConnect();
            //告诉listen线程
        }

        void CTcpHandler::ProcessCmd(CCommand *pCmd)
        {
		NsqLogPrintf(LOG_DEBUG, "CTcpHandler::ProcessCmd ProcessCmd Type = %d", pCmd->GetCmdType());
            switch(pCmd->GetCmdType())
            {
                case TCP_CONNECT_TYPE:
                {
                    TcpConnect(pCmd);
                    break;
                }
                case TCP_ADD_TYPE:
                {
                    TcpAdd(pCmd);
                    break;
                }
                case TCP_SEND_TYPE:
                {
                    TcpSend(pCmd);
                    break;
                }
                case TCP_DEL_TYPE:
                {
                    TcpDelete(pCmd); 
                    break;
                };
                case TCP_READ_TYPE:
                {
                    TcpRead(pCmd);    
                    break;
                }
            }

            delete pCmd;
        }
};
