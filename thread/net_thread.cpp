#include "net_thread.h"
#include <string.h>
#include "protocol.h"

namespace NSQTOOL
{
    int32_t CNetThread::CNetThread(int32_t iThreadType, int32_t iThreadId)
        :CThread(iThreadType, iThreadId)
    {
        m_pEventBase = event_base_new();			
        pthread_mutex_init(&m_mutex, NULL);
    }

    void CNetThread::OnStaticRead(struct bufferevent *pBufevt, void *arg)
    {
        CNetThread *pThis =  (CNetThread*)arg;
        pThis->OnRead(pBufevt, arg); 
    }

    void CNetThread::OnRead(struct bufferevent *pBufevt, void *arg)
    {
        fprintf(stdout, "CNetThread::OnRead\n");
        pthread_mutex_lock(&m_mutex);
        CNetThread *pThis = (CNetThread*)arg;	
        SNetContext *pContext = pThis->m_mapNetContext[bufferevent_getfd(pBufevt)];
        //内存copy三次，太浪费，后续改进
        int iLength = evbuffer_get_length(bufferevent_get_input(pBufevt));
        char *pData = new char[iLength + 1];
        bufferevent_read(pBufevt, pData, iLength);			
        pData[iLength] = '\0';
        int iNeedLength = pContext->m_pHandler->OnRead(pData, iLength); 

        while (iNeedLength == 0)
        {
            iNeedLength = pContext->m_pHandler->OnRead(NULL, 0);
        }
                
        bufferevent_setwatermark(pBufevt, EV_READ, iNeedLength, 0);
        delete pData;
        pthread_mutex_unlock(&m_mutex);
    }

    void CNetThread::OnStaticError(struct bufferevent *pBufevt, short iTemp, void *arg)
    {
        CNetThread *pThis = (CNetThread*)arg;	
        pThis->OnError(pBufevt, iTemp, arg);
    }

    void CNetThread::OnError(struct bufferevent *pBufevt, short iTemp, void *arg)
    {
        if (iTemp & BEV_EVENT_CONNECTED)
        {
            pthread_mutex_lock(&m_mutex);
            fprintf(stdout, "the client has connected to server\n");
            int32_t iHandle = bufferevent_getfd(pBufevt);            
            CNetThread *pThis = (CNetThread*)arg;	
            SNetContext *pContext = pThis->m_mapNetContext[bufferevent_getfd(pBufevt)];
            pContext->m_pHandler->OnConnect();
            pthread_mutex_unlock(&m_mutex);
            return; 
        }
    }

    int CNetThread::SendData(int iFd, const std::string *pString)
    {
        fprintf(stdout, "test3:iHandle = %d\n", iHandle);

        if (m_mapNetContext[iFd] == NULL)
        {
            fprintf(stdout, "SendData is null\n");
            return -1;	
        }
        
        int iRet = bufferevent_write(m_mapNetContext[iFd]->m_pBufevt, pString->c_str(), pString->size());	
        
        if (iRet != 0)
        {
            fprintf(stderr, "send data field, iHandle = %d", iFd);
        }

        return iRet;
    }

    void CNetThread::RealProcessCmd(CCommand &cCmd)
    {
        fprintf(stdout, "NetThread::ProcessCmd(cmd)\n");

        pthread_mutex_lock(&m_mutex);

        switch(cCmd.GetCmdType())
        {
            case NET_CONNECT_TYPE:
            {
                //只做tcp的connect
                CNetThread::SNetContext *pNetContext = (CNetThread::SNetContext *)cCmd.GetLData();
            
                CSingletonNsqFactory::GetInstance()->GenTcpHandler(pNetContext->m_iProtocolType, pNetContext->m_iProtocolId, 
                GetHandleId(),
                pNetContext->m_strHost, pNetContext->m_iPort);
                sockaddr_in sAddr;
                memset(&sAddr, 0, sizeof(sockaddr_in));
                sAddr.sin_addr.s_addr = inet_addr(pNetContext->m_strHost.c_str());
                sAddr.sin_port = htons(pNetContext->m_iPort);
                sAddr.sin_family = AF_INET;

                fprintf(stdout, "NET_CONNECT_TYPE:%s,%d\n", pNetContext->m_strHost.c_str(), pNetContext->m_iPort);		

                struct bufferevent * bufevt = bufferevent_socket_new(m_pEventBase, -1, 0);
                int32_t iRet = bufferevent_socket_connect(bufevt, (sockaddr*)&sAddr, sizeof(sockaddr_in));
                
                if (iRet != 0)
                {
                    bufferevent_free(bufevt);
                    bufevt = NULL;
                    //不肯定bufferevent_free是否会释放socket，后续测试
                    fprintf(stderr, "socket connect return error: %s,%d\n", pNetContext->m_strHost.c_str(), pNetContext->m_iPort);
                    pNetContext->m_pHandler = CSingletonNsqFactory::GetInstance()->GenTcpHandler(this,GetHandleId(), pNetContext->m_iProtocolType, pNetContext->m_iProtocolId, -1, pNetContext->m_strHost, pNetContext->m_iPort);
                    pNetContext->m_pHandler->OnError(errno);
                    delete pNetContext->m_pHandler;
                    delete pNetContext;
                    break;
                }

                int iFd = bufferevent_getfd(bufevt);
                bufferevent_setcb(bufevt, OnStaticRead, NULL, OnStaticError, this);
                bufferevent_enable(bufevt, EV_READ|EV_PERSIST|EV_ET);		
                pNetContext->m_pHandler = CSingletonNsqFactory::GetInstance()->GenTcpHandler(this,GetHandleId(), pNetContext->m_iProtocolType, pNetContext->m_iProtocolId, iFd, pNetContext->m_strHost, pNetContext->m_iPort);
                pNetContext->m_pBufevt = bufevt;

                //设置读入最低水位，防止无效回调
                bufferevent_setwatermark(bufevt, EV_READ, 
                                          pNetContext->m_pHandler->Need(NULL, 0), 0);
                m_mapNetContext[iFd] = pNetContext;
                pNetContext->m_iFd = iFd;
                fprintf(stdout, "test2:iHandle = %d\n", iFd);
            }
            break;
            case NET_DEL_TYPE:
            {
                int iFd = (int64_t)cCmd.GetLData();
                DestoryFd(iFd);
            }
            case NET_ADD_TYPE:
            {
                CNetThread::SNetContext *pNetContext = (CNetThread::SNetContext *)cCmd.GetLData();
                //listen上来的包，默认由pkg的process函数
                struct bufferevent * bufevt = bufferevent_socket_new(m_pEventBase, pNetContext->m_iHandle, 0);
                bufferevent_setcb(bufevt, OnStaticRead, NULL, OnStaticError, this);
                bufferevent_enable(bufevt, EV_READ|EV_WRITE|EV_PERSIST|EV_ET);		
                bufferevent_setwatermark(bufevt, EV_READ, 
                                          pNetContext->m_pPkg->Need(NULL, 0), 0);
                m_mapNetContext[pNetContext->m_iFd] = pNetContext;
                pNetContext->m_pHandler = CSingletonNsqFactory::GetInstance()->GenTcpHandler(this,GetHandleId(), pNetContext->m_iProtocolType, pNetContext->m_iProtocolId, pNetContext->iFd, pNetContext->m_strHost, pNetContext->m_iPort);
                pNetContext->m_pHandler->OnConnect();
            }
            break;
            case NET_SEND_TYPE:
            {
                int iFd = (int64_t)cCmd.GetLData();
                std::string *pString = (std::string*)cCmd.GetRData();

                if (SendData(iFd, pString) != 0)
                {
                    //发送失败应该回包给源线程，暂时忽略
                }

                delete pString;
            };
            break;
        }
        
        pthread_mutex_unlock(&m_mutex);
    }	

    void CNetThread::RealRun()
    {
        fprintf(stdout, "CNetThread RealRun\n");
        while (!m_bStop)	
        {
            CThread::ProcessCmd();

            //1ms返回一次，所有有关net的thread通讯，时延在ms级别,建议通过send命令直接发送
            struct timeval sTm; 
            sTm.tv_sec =0;
            sTm.tv_usec = 1000;
            event_base_loopexit(m_pEventBase, &sTm);	
            event_base_dispatch(m_pEventBase);
        }
    }


    void CNetThread::DestoryFd(int iFd)
    {
        uint64_t iRealHandleId = 0;

        if (m_mapNetContext.find(iFd) != m_mapNetContext.end())
        {
            iRealHandleId = m_mapNetContext[iFd]->m_pHandler->GetHandlerId();
            bufferevent_free(m_mapNetContext[iFd]->m_pBufevt);
            m_mapNetContext.erase(iFd);
        }

        CThread::DestoryHandler(iRealHandleId);
    }

    //////////////////////////////////////////////////////////
    //                CListenThread
    /////////////////////////////////////////////////////////
    int32_t CListenThread::CListenThread(int32_t iThreadType, int32_t iThreadId):
        CThread(iThreadType, iThreadId)
    {
        m_pEventBase = event_base_new();			
    }

    void CListenThread::RealRun()
    {
        fprintf(stdout, "CListenThread RealRun\n");
        while (!m_bStop)	
        {
            ProcessCmd();

            struct timeval sTm; 
            sTm.tv_sec =0;
            sTm.tv_usec = 1000;
            event_base_loopexit(m_pEventBase, &sTm);	
            event_base_dispatch(m_pEventBase);
        }
    }

    void CListenThread::OnStaticRead(struct evconnlistener *pListener, evutil_socket_t iAcceptHandle, 
                               struct sockaddr *pAddr, int socklen, void *pArg)		
    {
        CListenThread* pThis = (CListenThread*)pArg;		
        pThis->OnRead(pListener, iAcceptHandle, pAddr, socklen, pArg);
    }

    void CListenThread::OnRead(struct evconnlistener *pListener, evutil_socket_t iAcceptHandle, 
                               struct sockaddr *pAddr, int socklen, void *pArg)		
    {
        pthread_mutex_lock(&m_mutex);
        CListenThread* pThis = (CListenThread*)pArg;		
        CCommand cCmd(NET_ADD_TYPE);
        CNetThread::SNetContext *pNetContext = new CNetThread::SNetContext;
        pNetContext->m_iHandle = iAcceptHandle;
        pNetContext->m_iPkgType = pThis->m_mapListen[evconnlistener_get_fd(pListener)]->m_iPkgType; 
        pNetContext->m_strHost = inet_ntoa(((sockaddr_in*)pAddr)->sin_addr);
        pNetContext->m_iPort = ntohs(((sockaddr_in*)pAddr)->sin_port);
        cCmd.SetLData(pNetContext);
        CThreadMgrSingleton::GetInstance()->SendCmd(
                pThis->m_mapListen[evconnlistener_get_fd(pListener)]->m_cAddr.m_iDstType, 
                cCmd, pThis->m_mapListen[evconnlistener_get_fd(pListener)]->m_cAddr.m_iDstTid);
        pthread_mutex_unlock(&m_mutex);
    }
    
    void CListenThread::RealProcessCmd(CCommand &cCmd)
    {
        fprintf(stdout, "CListenThread::RealProcessCmd begin\n");
        pthread_mutex_lock(&m_mutex);

        switch(cCmd.GetCmdType())
        {
            case NET_LISTEN_TYPE:
            {
                SListenInfo *pListenInfo = (SListenInfo *)cCmd.GetLData();		
                fprintf(stdout, "NET_LISTEN_TYPE:%s_%d\n", pListenInfo->m_strHost.c_str(), pListenInfo->m_iPort);		

                sockaddr_in sAddr;
                memset(&sAddr, 0, sizeof(sockaddr_in));
                sAddr.sin_addr.s_addr = inet_addr(pListenInfo->m_strHost.c_str());
                sAddr.sin_port = htons(pListenInfo->m_iPort);
                sAddr.sin_family = AF_INET;

                 evconnlistener * pListener = evconnlistener_new_bind(m_pEventBase, OnStaticRead, this, 
                        LEV_OPT_THREADSAFE|LEV_OPT_REUSEABLE|LEV_OPT_CLOSE_ON_FREE, 10,
                        (sockaddr*)&sAddr, sizeof(sockaddr_in));
                int iFd = evconnlistener_get_fd(pListener);	
                m_mapListen[iFd] = pListenInfo;

            }
            break;
            case NET_DEL_TYPE:
            {
                std::string *pString = (std::string *)cCmd.GetLData();
                char chHost[64] = {0};
                uint16_t iPort = 0;
                sscanf(pString->c_str(), "%s_%d", chHost, &iPort);

                std::map<int, SListenInfo*>::iterator iter = m_mapListen.begin();
                
                for (; iter != m_mapListen.end(); ++iter)
                {
                    if (iter->second->m_strHost == chHost && iter->second->m_iPort == iPort)
                    {
                        evconnlistener_free(iter->second->m_pListener);
                        delete iter->second;
                        m_mapListen.erase(iter);
                        break;
                    }
                }

                delete pString;
            }
            break;
        }
            
        pthread_mutex_unlock(&m_mutex);
    }
};

