#include "net_thread.h"
#include <string.h>
#include "protocol.h"
#include "handler.h"
#include "tcp_handler.h"
#include "listen_handler.h"

namespace NSQTOOL
{
    CNetThread::CNetThread(int32_t iThreadType, int32_t iThreadId)
        :CEventThread(iThreadType, iThreadId)
    {

    }

    void CNetThread::OnStaticRead(struct bufferevent *pBufevt, void *arg)
    {
        CTcpHandler *pHandler =  (CTcpHandler*)arg;
        dynamic_cast<CNetThread *>(pHandler->GetThread())->OnRead(pBufevt, arg); 
    }

    void CNetThread::OnRead(struct bufferevent *pBufevt, void *arg)
    {
        pthread_mutex_lock(&m_mutex);
        CTcpHandler *pHandler =  (CTcpHandler*)arg;
        //内存copy三次，太浪费，后续改进
        int iLength = evbuffer_get_length(bufferevent_get_input(pBufevt));
        char *pData = new char[iLength + 1];
        bufferevent_read(pBufevt, pData, iLength);			
        pData[iLength] = '\0';

        //iNeedLength 证明包处理出错，已经析构掉了handler相关的一些
        int iNeedLength = pHandler->OnRead(pData, iLength); 

        if (iNeedLength > 0)
        {
            bufferevent_setwatermark(pBufevt, EV_READ, iNeedLength, 0);
        }

        delete pData;
        pthread_mutex_unlock(&m_mutex);
    }

    void CNetThread::OnStaticError(struct bufferevent *pBufevt, short iTemp, void *arg)
    {
        CTcpHandler *pHandler =  (CTcpHandler*)arg;
        dynamic_cast<CNetThread *>(pHandler->GetThread())->OnError(pBufevt, iTemp, arg);
    }

    void CNetThread::OnError(struct bufferevent *pBufevt, short iTemp, void *arg)
    {
        pthread_mutex_lock(&m_mutex);
        CTcpHandler *pHandler =  (CTcpHandler*)arg;

        if (iTemp & BEV_EVENT_CONNECTED)
        {
            pHandler->OnConnect();
        }
        else
        {
            pHandler->OnError(iTemp); 
        }

        pthread_mutex_unlock(&m_mutex);
    }

    void CNetThread::DestoryHandler(uint64_t iHandlerId)
    {
        pthread_mutex_lock(&m_mutex);
        NsqLogPrintf(LOG_DEBUG, "CNetThread::DestoryHandler HandlerId = %d\n", iHandlerId);
        if (m_mapHandler.find(iHandlerId) != m_mapHandler.end())
        {
            NsqLogPrintf(LOG_DEBUG, "CNetThread::DestoryHandler found\n");
            CTcpHandler *pHandler = dynamic_cast<CTcpHandler *>(m_mapHandler[iHandlerId]);
            close(bufferevent_getfd(pHandler->GetBufferevent()));
            bufferevent_free(pHandler->GetBufferevent()); 
            CThread::DestoryHandler(iHandlerId);
        }

        pthread_mutex_unlock(&m_mutex);
    }

    int CNetThread::SendData(struct bufferevent *pBufevt, 
                            const std::string *pString,
                            bool bIsCopy)
                            
    {
        pthread_mutex_lock(&m_mutex);

        int iRet = bufferevent_write(pBufevt, pString->c_str(), pString->size());	
        
        pthread_mutex_unlock(&m_mutex);

        if (!bIsCopy)
        {
            delete pString;
        }

        return iRet;
    }


    void CNetThread::RealProcessCmd(CCommand &cCmd)
    {

        pthread_mutex_lock(&m_mutex);

        switch(cCmd.GetCmdType())
        {
            case NET_CONNECT_TYPE:
            {
                //只做tcp的connect
                CNetThread::SNetContext *pNetContext = (CNetThread::SNetContext *)cCmd.GetLData();
            
                sockaddr_in sAddr;
                memset(&sAddr, 0, sizeof(sockaddr_in));
                sAddr.sin_addr.s_addr = inet_addr(pNetContext->m_strHost.c_str());
                sAddr.sin_port = htons(pNetContext->m_iPort);
                sAddr.sin_family = AF_INET;


                struct bufferevent * bufevt = bufferevent_socket_new(m_pEventBase, -1, 0);
                int32_t iRet = bufferevent_socket_connect(bufevt, (sockaddr*)&sAddr, sizeof(sockaddr_in));
                CTcpHandler *pHandler;
                pHandler = dynamic_cast<CTcpHandler *>(CSingletonNsqFactory::GetInstance()->GenHandler(
                            pNetContext->m_iProtocolType, pNetContext->m_iProtocolId, 
                            GetHandlerId(), this));
                
                if (iRet != 0)
                {
                    bufferevent_free(bufevt);
                    bufevt = NULL;
                    //不肯定bufferevent_free是否会释放socket，后续测试
                    pHandler->OnError(errno);
                    delete pNetContext;
                    delete pHandler;
                    break;
                }

                bufferevent_setcb(bufevt, OnStaticRead, NULL, OnStaticError, pHandler);
                bufferevent_enable(bufevt, EV_READ|EV_PERSIST);		

                //设置读入最低水位，防止无效回调
                bufferevent_setwatermark(bufevt, EV_READ, 
                                         pHandler->OnRead(NULL, 0), 0);

                pHandler->SetBufferevent(bufevt);
                pHandler->SetHost(pNetContext->m_strHost);
                pHandler->SetPort(pNetContext->m_iPort);
                m_mapHandler[pHandler->GetHandlerId()] = pHandler;
            }
            break;
            case NET_DEL_TYPE:
            {
                
            }
            case NET_ADD_TYPE:
            {
                CNetThread::SNetContext *pNetContext = (CNetThread::SNetContext *)cCmd.GetLData();
                CTcpHandler *pHandler;
                pHandler = dynamic_cast<CTcpHandler *>(CSingletonNsqFactory::GetInstance()->GenHandler(
                            pNetContext->m_iProtocolType, pNetContext->m_iProtocolId, 
                            GetHandlerId(), this));
                //listen上来的包，默认由pkg的process函数
                struct bufferevent * bufevt = bufferevent_socket_new(m_pEventBase, pNetContext->m_iFd, 0);
                bufferevent_setcb(bufevt, OnStaticRead, NULL, OnStaticError, pHandler);
                bufferevent_enable(bufevt, EV_READ|EV_WRITE|EV_PERSIST|EV_ET);		
                bufferevent_setwatermark(bufevt, EV_READ, 
                                          pHandler->OnRead(NULL, 0), 0);
                pHandler->SetListenHandler(pNetContext->m_pListenHandler);
                pHandler->SetHost(pNetContext->m_strHost);
                pHandler->SetPort(pNetContext->m_iPort);
                pHandler->SetBufferevent(bufevt);
                pHandler->OnConnect();
            }
            break;
            case NET_SEND_TYPE:
            {
            };
            break;
        }
        
        pthread_mutex_unlock(&m_mutex);
    }	

    //////////////////////////////////////////////////////////
    //                CListenThread
    /////////////////////////////////////////////////////////
    CListenThread::CListenThread(int32_t iThreadType, int32_t iThreadId)
        :CEventThread(iThreadType, iThreadId)
    {

    }

    void CListenThread::OnStaticRead(struct evconnlistener *pListener, evutil_socket_t iAcceptHandle, 
                               struct sockaddr *pAddr, int socklen, void *pArg)		
    {
        //有bug
        CListenHandler *pHandler = (CListenHandler *)pArg;
        dynamic_cast<CListenThread *>(pHandler->GetThread())->OnRead(pListener, iAcceptHandle, pAddr, socklen, pArg);
    }

    void CListenThread::OnRead(struct evconnlistener *pListener, evutil_socket_t iAcceptHandle, 
                               struct sockaddr *pAddr, int socklen, void *pArg)		
    {
        pthread_mutex_lock(&m_mutex); 

        CListenHandler *pHandler = (CListenHandler *)pArg;

        CCommand cCmd(NET_ADD_TYPE);

        CNetThread::SNetContext *pNetContext = new CNetThread::SNetContext;
        pNetContext->m_iProtocolType = pHandler->GetProtocolType();
        pNetContext->m_iProtocolId = pHandler->GetProtocolId();
        pNetContext->m_iFd = iAcceptHandle;
        pNetContext->m_strHost = pHandler->GetHost();
        pNetContext->m_iPort = pHandler->GetPort();
        pNetContext->m_pListenHandler = pHandler;

        cCmd.SetLData(pNetContext);

        CThreadMgrSingleton::GetInstance()->SendCmd( NET_THREAD_TYPE, cCmd);

        pthread_mutex_unlock(&m_mutex); 
    }
    
    void CListenThread::RealProcessCmd(CCommand &cCmd)
    {
        pthread_mutex_lock(&m_mutex); 

        switch(cCmd.GetCmdType())
        {
            case NET_LISTEN_TYPE:
            {
                SListenInfo *pListenInfo = (SListenInfo *)cCmd.GetLData();		

                sockaddr_in sAddr;
                memset(&sAddr, 0, sizeof(sockaddr_in));
                sAddr.sin_addr.s_addr = inet_addr(pListenInfo->m_strHost.c_str());
                sAddr.sin_port = htons(pListenInfo->m_iPort);
                sAddr.sin_family = AF_INET;

                CListenHandler * pListenHandler = dynamic_cast<CListenHandler *>(CSingletonNsqFactory::GetInstance()->GenHandler(
                        pListenInfo->m_iProtocolType, pListenInfo->m_iProtocolId, 
                        GetHandlerId(), this));

                evconnlistener * pListener = evconnlistener_new_bind(m_pEventBase, OnStaticRead, pListenHandler, 
                        LEV_OPT_THREADSAFE|LEV_OPT_REUSEABLE|LEV_OPT_CLOSE_ON_FREE, 10,
                        (sockaddr*)&sAddr, sizeof(sockaddr_in));

                pListenHandler->SetHost(pListenInfo->m_strHost);
                pListenHandler->SetPort(pListenInfo->m_iPort);
                pListenHandler->SetEvlisten(pListener);
                m_mapHandler[pListenHandler->GetHandlerId()] = pListenHandler;
            }
            break;
            case NET_DEL_TYPE:
            {
            }
            break;
        }

        pthread_mutex_unlock(&m_mutex); 
    }

    void CListenThread::DestoryHandler(uint64_t iHandlerId)
    {
        pthread_mutex_lock(&m_mutex); 

        if (m_mapHandler.find(iHandlerId) != m_mapHandler.end())
        {
            CListenHandler * pListenHandler = dynamic_cast<CListenHandler *>(m_mapHandler[iHandlerId]);
            evconnlistener_free(pListenHandler->GetEvlisten());
            CThread::DestoryHandler(iHandlerId);
        }

        pthread_mutex_unlock(&m_mutex); 
    }
};

