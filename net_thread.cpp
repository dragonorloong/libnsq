#include "net_thread.h"
namespace NSQTOOL
{
    int32_t CNetThread::Init(int32_t iThreadType, int32_t iThreadId, void *pArg)
    {
        CThread::Init(iThreadType, iThreadId, pArg);
        m_pEventBase = event_base_new();			
    }

    void CNetThread::OnRead(struct bufferevent *pBufevt, void *arg)
    {
        CNetThread *pThis = (CNetThread*)arg;	
        SNetContext *pContext = pThis->m_mapNetContext[bufferevent_getfd(pBufevt)];
        //内存拷贝三次，太浪费，后续改进
        int iLength = evbuffer_get_length(bufferevent_get_input(pBufevt));
        char *pData = new char[iLength];
        bufferevent_read(pBufevt, pData, iLength);			

        if (pContext->m_pPkg->Need(pData, iLength) == 0)
        {
            pContext->m_pPkg->Process(pContext, this);	
        }

        delete pData;
    }

    void CNetThread::OnError(struct bufferevent *pBufevt, short iTemp, void *arg)
    {
        
    }

    int CNetThread::SendData(int iHandle, std::string *pString)
    {
        if (m_mapNetContext[iHandle] == NULL)
        {
            return -1;	
        }
        
        int iRet = bufferevent_write(m_mapNetContext[iHandle]->m_pBufevt, pString->c_str(), pString->size());	
        
        if (iRet != 0)
        {
            fprintf(stderr, "send data field, iHandle = %d", iHandle);
        }

        return iRet;
    }

    void CNetThread::RealProcessCmd(const CCommand &cCmd)
    {
        fprintf(stdout, "NetThread::ProcessCmd(cmd)\n");
        switch(cCmd.GetCmdType())
        {
            case NET_CONNECT_TYPE:
            {
                //只做tcp的connect
                CNetThread::SNetContext *pNetContext = cCmd.GetLData();
            
                sockaddr_in sAddr;
                memset(&sAddr, 0, sizeof(sockaddr_in));
                sAddr.sin_addr.s_addr = inet_addr(pNetContext->m_strHost.c_str());
                sAddr.sin_port = htons(pNetContext->m_iPort);
                sAddr.sin_family = AF_INET;

                fprintf(stdout, "NET_CONNECT_TYPE:%s,%d\n", pNetContext->m_strHost.c_str(), pNetContext->m_iPort);		
                int iHandle = socket(AF_INET, SOCK_STREAM, 0);	
                
                if (iHandle == -1)
                {
                    fprintf(stderr, "socket return error, errno = %d\n", errno);		
                }

                struct bufferevent * bufevt = bufferevent_socket_new(m_pEventBase, iHandle, 0);
                int32_t iRet = bufferevent_socket_connect(bufevt, (sockaddr*)&sAddr, sizeof(sockaddr_in));
                
                if (iRet != 0)
                {
                    bufferevent_free(bufevt);
                    bufevt = NULL;
                    //不肯定bufferevent_free是否会释放socket，后续测试
                    fprintf(stderr, "socket connect return error: %s,%d\n", pNetContext->m_strHost.c_str(), pNetContext->m_iPort);
                    break;
                }

                bufferevent_setcb(bufevt, OnRead, NULL, OnError, this);
                bufferevent_enable(bufevt, EV_READ);		
                m_mapNetContext[iHandle] = pNetContext;

                //回一个包给源线程
                CCommand::CCmdAddr &cAddr = cCmd.GetAddr();	
                CCommand::CCmdAddr cSendAddr;
                cSendAddr.m_iSrcTid = GetThreadId();
                cSendAddr.m_iSrcThreadType = GetThreadType();
                cSendAddr.m_iDstTid = cAddr.m_iSrcTid;
                cSendAddr.m_iDstThreadType = cAddr.m_iSrcThreadType;
                CCommand cmd(NET_CONNECT_TYPE);
                cmd.SetAddr(cSendAddr);
                cmd.SetLData((int32_t*)iHandle);
                //cmd.SetCmdType(NET_CONNECT_TYPE);
                CThreadMgrSingleton::GetInstance()->SendCmd(cAddr.m_iSrcThreadType, cmd, cAddr.m_iSrcTid);
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
                CNetThread::SNetContext *pNetContext = cCmd.GetLData();
                //listen上来的包，默认由pkg的process函数
                struct bufferevent * bufevt = bufferevent_socket_new(m_pEventBase, pNetContext->m_iHandle, 0);
                bufferevent_setcb(bufevt, OnRead, NULL, OnError, this);
                bufferevent_enable(bufevt, EV_READ);		
                m_mapNetContext[iHandle] = pNetContext;
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
    }	

    void CNetThread::RealRun()
    {
        fprintf(stdout, "CNetThread RealRun\n");
        while (!m_bStop)	
        {
            CThread::ProcessCmd(this);

            if (m_bStop)
            {
               pthread_cond_signal(&m_condWait); 
               break;
            }

            struct timeval sTm; 
            sTm.tv_sec =1;
            sTm.tv_usec = 0;
            event_base_loopexit(m_pEventBase, &sTm);	
              event_base_dispatch(m_pEventBase);
        }
    }


    //////////////////////////////////////////////////////////
    //                CListenThread
    /////////////////////////////////////////////////////////
    int32_t CListenThread::Init(void *pArg)
    {
        CThread::Init(pArg);
        m_pEventBase = event_base_new();			
    }

    void CListenThread::RealRun()
    {
        fprintf(stdout, "CListenThread RealRun\n");
        while (!m_bStop)	
        {
            ProcessCmd(this);

            if (m_bStop)
            {
               pthread_cond_signal(&m_condWait); 
               break;
            }

            struct timeval sTm; 
            sTm.tv_sec =1;
            sTm.tv_usec = 0;
            event_base_loopexit(m_pEventBase, &sTm);	
            event_base_dispatch(m_pEventBase);
        }
    }

    void CListenThread::OnRead(struct evconnlistener *pListener, evutil_socket_t iAcceptHandle, 
                               struct sockaddr *pAddr, int socklen, void *pArg)		
    {
        CListenThread* pThis = (CListenThread*)pArg;		
        CCommand cCmd(NET_ADD_TYPE);
        SNetContext *pNetContext = new CNetThread::SNetContext;
        pNetContext->m_iHandle = iHandle;
        pNetContext->m_iPkgType = pThis->m_mapListen[evconnlistener_get_fd(pListener)].m_iPkgType; 
        pNetContext->m_strHost = inet_ntoa(((sockaddr_in*)pAddr)->sin_addr);
        pNetContext->m_iPort = ntohs(((sockaddr_in*)pAddr)->sin_port);
        cCmd.SetLData(pNetContext);
        CThreadMgrSingleton::GetInstance()->SendCmd(
                pThis->m_mapListen[evconnlistener_get_fd(pListener)].m_cAddr.m_iDstThreadType, 
                cCmd, pThis->m_mapListen[evconnlistener_get_fd(pListener)].m_cAddr.m_iDstThreadNum);
    }
    
    void CListenThread::RealProcessCmd(const CCommand &cCmd)
    {
         fprintf(stdout, "CListenThread::RealProcessCmd begin\n");

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

                 evconnlistener * pListener = evconnlistener_new_bind(m_pEventBase, OnRead, this, 
                        LEV_OPT_THREADSAFE|LEV_OPT_REUSEABLE|LEV_OPT_CLOSE_ON_FREE, 10,
                        (sockaddr*)&sAddr, sizeof(sockaddr_in));
                int iHandle = evconnlistener_get_fd(pListener);	
                m_mapListen[iHandle] = pListenInfo;
                delete pAddr;
            }
            break;
            case NET_DEL_TYPE:
            {
                std::string *pString = (std::string *)cCmd.GetLData();
                std::map<int, SListenInfo>::iterator iter = m_mapListen.begin();
                
                for (; iter != m_mapListen.end(); ++iter)
                {
                    if (iter->second->m_strAddr == *pString)
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
    }

};

