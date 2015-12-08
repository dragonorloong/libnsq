#include "net_thread.h"
#include <string.h>
#include "protocol.h"

namespace NSQTOOL
{
    void CNetThread::SNetContext::Init(int32_t iHandle, bufferevent *pBufevt)
    {
        m_iHandle = iHandle;
        m_pBufevt = pBufevt;

        m_pPkg = CData::New(m_iPkgType);
    }

    int32_t CNetThread::Init(int32_t iThreadType, int32_t iThreadId, void *pArg)
    {
        CThread::Init(iThreadType, iThreadId, pArg);
        m_pEventBase = event_base_new();			
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
        int iNeedLength = pContext->m_pPkg->Need(pData, iLength);

        while (iNeedLength == 0)
        {
            pContext->m_pPkg->Process(pContext, pThis);	
            pContext->m_pPkg->NextPkg();
            iNeedLength = pContext->m_pPkg->Need(NULL, 0);
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
        pthread_mutex_lock(&m_mutex);

        if (iTemp & BEV_EVENT_CONNECTED)
        {
            fprintf(stdout, "the client has connected to server\n");
            int32_t iHandle = bufferevent_getfd(pBufevt);            
            CNetThread *pThis = (CNetThread*)arg;	
            SNetContext *pContext = pThis->m_mapNetContext[bufferevent_getfd(pBufevt)];
            pContext->m_pPkg->OnConnect(pContext, this);
            pthread_mutex_unlock(&m_mutex);
            return; 
        }

        int32_t iHandle = bufferevent_getfd(pBufevt);            
        CNetThread *pThis = (CNetThread*)arg;	
        SNetContext *pContext = pThis->m_mapNetContext[bufferevent_getfd(pBufevt)];
        pContext->m_pPkg->OnError(pContext, this, iTemp);


        if (pThis->m_mapNetContext[iHandle]->m_pBufevt != NULL)	
        {
            bufferevent_free(pThis->m_mapNetContext[iHandle]->m_pBufevt);
            pThis->m_mapNetContext[iHandle]->m_pBufevt = NULL;
        }
        
        delete pThis->m_mapNetContext[iHandle];
        pThis->m_mapNetContext.erase(iHandle);
        pthread_mutex_unlock(&m_mutex);
    }

    int CNetThread::SendData(int iHandle, const std::string *pString)
    {
        fprintf(stdout, "test3:iHandle = %d\n", iHandle);

        if (m_mapNetContext[iHandle] == NULL)
        {
            fprintf(stdout, "SendData is null\n");
            return -1;	
        }
        
        int iRet = bufferevent_write(m_mapNetContext[iHandle]->m_pBufevt, pString->c_str(), pString->size());	
        
        if (iRet != 0)
        {
            fprintf(stderr, "send data field, iHandle = %d", iHandle);
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
                    pNetContext->m_pPkg->OnError(pNetContext, this, errno);
                    break;
                }

                int iHandle = bufferevent_getfd(bufevt);
                bufferevent_setcb(bufevt, OnStaticRead, NULL, OnStaticError, this);
                bufferevent_enable(bufevt, EV_READ|EV_PERSIST);		
                pNetContext->Init(iHandle, bufevt);

                //设置读入最低水位，防止无效回调
                bufferevent_setwatermark(bufevt, EV_READ, 
                                          pNetContext->m_pPkg->Need(NULL, 0), 0);
                m_mapNetContext[iHandle] = pNetContext;
                fprintf(stdout, "test2:iHandle = %d\n", iHandle);
            }
            break;
            case NET_DEL_TYPE:
            {
                int iHandle = (int64_t)cCmd.GetLData();

                if (m_mapNetContext[iHandle]->m_pBufevt != NULL)	
                {
                    bufferevent_free(m_mapNetContext[iHandle]->m_pBufevt);
                    m_mapNetContext[iHandle]->m_pBufevt = NULL;
                }
                
                delete m_mapNetContext[iHandle];
                m_mapNetContext.erase(iHandle);
            }
            case NET_ADD_TYPE:
            {
                CNetThread::SNetContext *pNetContext = (CNetThread::SNetContext *)cCmd.GetLData();
                //listen上来的包，默认由pkg的process函数
                struct bufferevent * bufevt = bufferevent_socket_new(m_pEventBase, pNetContext->m_iHandle, 0);
                pNetContext->Init(pNetContext->m_iHandle, bufevt);
                bufferevent_setcb(bufevt, OnStaticRead, NULL, OnStaticError, this);
                bufferevent_enable(bufevt, EV_READ|EV_WRITE|EV_PERSIST);		
                bufferevent_setwatermark(bufevt, EV_READ, 
                                          pNetContext->m_pPkg->Need(NULL, 0), 0);
                m_mapNetContext[pNetContext->m_iHandle] = pNetContext;
                pNetContext->m_pPkg->OnAccept(pNetContext, this);
            }
            break;
            case NET_SEND_TYPE:
            {
                int iHandle = (int64_t)cCmd.GetLData();
                std::string *pString = (std::string*)cCmd.GetRData();

                if (SendData(iHandle, pString) != 0)
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

    //////////////////////////////////////////////////////////
    //                CListenThread
    /////////////////////////////////////////////////////////
    int32_t CListenThread::Init(int32_t iThreadType, int32_t iThreadId, void *pArg)
    {
        CThread::Init(iThreadType, iThreadId, pArg);
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
                int iHandle = evconnlistener_get_fd(pListener);	
                m_mapListen[iHandle] = pListenInfo;
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

