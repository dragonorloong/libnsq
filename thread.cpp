#include "net_thread.h"
#include "factory.h"

namespace NSQTOOL
{
    CThread::CThread(int32_t iThreadType, int32_t iThreadId)
    {
        pthread_mutexattr_t attr;
        pthread_mutexattr_init(&attr);
        pthread_mutexattr_settype(&attr,PTHREAD_MUTEX_RECURSIVE);
        pthread_mutex_init(&m_mutex, &attr);
        pthread_mutex_init(&m_mutexSync, &attr);
        pthread_mutexattr_destroy(&attr);
        pthread_cond_init(&m_cond, NULL);
        m_bStop = false;

        m_iThreadType = iThreadType;
        m_iThreadId = iThreadId;
        NsqLogPrintf(LOG_DEBUG, "Init ThreadType = %d, iThreadId = %d\n", iThreadType, iThreadId);
        m_iHandleIdInc = 0;
    }
    
    CThread::~CThread()
    {
        pthread_mutex_lock(&m_mutex);
        map<uint64_t, CHandler *>::iterator iter = m_mapHandler.begin();

        for (; iter != m_mapHandler.end();)
        {
            NsqLogPrintf(LOG_DEBUG, "~Thread Type = %d", m_iThreadType);
            DestoryHandler(iter->second->GetHandlerId()); 
            iter = m_mapHandler.begin();
        }

        NsqLogPrintf(LOG_DEBUG, "~Thread end Type = %d", m_iThreadType);

        pthread_mutex_unlock(&m_mutex);
        pthread_mutex_destroy(&m_mutex);
        pthread_mutex_destroy(&m_mutexSync);
        pthread_cond_destroy(&m_cond);
    }
    
    int32_t CThread::GetThreadId()
    {
        return m_iThreadId;
    }
    
    int32_t  CThread::GetThreadType()
    {
        return m_iThreadType;
    }

    //同步消息
    void CThread::SendCmd(CCommand *pCmd)
    {
        pthread_mutex_lock(&m_mutexSync);
        RealProcessCmd(pCmd);
        pthread_mutex_unlock(&m_mutexSync);
    }
    
    //异步消息
    void CThread::PostCmd(CCommand *pCmd)
    {
        pthread_mutex_lock(&m_mutex);
        m_lstCmd.push_back(pCmd);
        
        if (m_lstCmd.size() == 1)
        {
            //pthread_cond_signal(&m_cond);
            NotifyWait();
        }

        pthread_mutex_unlock(&m_mutex);
    }

    void CThread::NotifyWait()
    {
        pthread_cond_signal(&m_cond);
    }
    
    void CThread::RealProcessCmd(CCommand *pCmd)
    {
        if (pCmd->GetCmdType() == STOP_TYPE)
        {
            m_lstCmd.clear();	
            m_bStop = true;
        }
        else
        {
           if (m_mapHandler.find(pCmd->GetAddr().m_cDstAddr.m_iHandlerId) != m_mapHandler.end())
           {
                m_mapHandler[pCmd->GetAddr().m_cDstAddr.m_iHandlerId]->ProcessCmd(pCmd);         
           }
           else
           {
                CHandler *pHandler = CSingletonNsqFactory::GetInstance()->GenHandler(pCmd->GetCmdType(),
                                    pCmd->GetCmdId(), 
                                    GetHandlerId(), this);
                
                if (pHandler == NULL)
                {
                    NsqLogPrintf(LOG_DEBUG, "no found this handler handler id = ", pCmd->GetAddr().m_cDstAddr.m_iHandlerId);
                    delete pCmd; 
                }

                m_mapHandler[pHandler->GetHandlerId()] = pHandler;
                pHandler->ProcessCmd(pCmd);
           }
        }
    }
    
    int32_t CThread::ProcessCmd()
    {
        int32_t iRet = 0;
        
        while (m_lstCmd.size() != 0)
        {
            pthread_mutex_lock(&m_mutex);
            
            if (m_lstCmd.size() == 0)
            {
                pthread_mutex_unlock(&m_mutex);
                return iRet;
            }
            
            CCommand *pCmd = *m_lstCmd.begin();
            m_lstCmd.erase(m_lstCmd.begin());
            pthread_mutex_unlock(&m_mutex);
            
            pthread_mutex_lock(&m_mutexSync);
            RealProcessCmd(pCmd);
            pthread_mutex_unlock(&m_mutexSync);
            
            iRet ++;
        }

        return iRet;
    }

    void CThread::RealRun()
    {
        while (!m_bStop)
        {
            int32_t iRet = ProcessCmd();

            if (iRet ==0 && !m_bStop)
            {
                pthread_mutex_lock(&m_mutex);
                pthread_cond_wait(&m_cond, &m_mutex);
                pthread_mutex_unlock(&m_mutex);
            }
        }	
    }

    void CThread::Run()
    {
        pthread_create(&m_iTid, NULL, ThreadFunc, this);	
    }
    
    void CThread::Stop()
    {
        m_bStop = true;
        NotifyWait();
        pthread_join(m_iTid, NULL);
    }

    void CThread::DestoryHandler(uint64_t iHandlerId)
    {
        pthread_mutex_lock(&m_mutex);
        NsqLogPrintf(LOG_DEBUG, "CThread::DestoryHandler iHandlerId = %d\n", iHandlerId);

        if (m_mapHandler.find(iHandlerId) != m_mapHandler.end())
        {
            NsqLogPrintf(LOG_DEBUG, "CThread::DestoryHandler Handler found \n");
            delete m_mapHandler[iHandlerId];
            m_mapHandler.erase(iHandlerId);
        }

        NsqLogPrintf(LOG_DEBUG, "CThread::DestoryHandler iHandlerId = %d end\n", iHandlerId);
        pthread_mutex_unlock(&m_mutex);
    }

    uint64_t CThread::GetHandlerId()
    {
        pthread_mutex_lock(&m_mutex);
        ++m_iHandleIdInc; 
        pthread_mutex_unlock(&m_mutex);
        return m_iHandleIdInc; 
    }

    void *CThread::ThreadFunc(void *pArgs)
    {
        CThread *pThread = (CThread *)pArgs;	
        pThread->RealRun();
    }

    CThreadPool::CThreadPool(int iThreadType, int iThreadNum)
    {
        m_iCurrentNum = 0;
        m_iTotalThreadNum = iThreadNum;
        m_iThreadType = iThreadType;
        
        for (int i = 0; i < iThreadNum; ++i)
        {
            NsqLogPrintf(LOG_DEBUG, "ThreadPool ThreadType = %d, iThreadId = %d\n", iThreadType, i);
            CThread *pThread = CSingletonNsqFactory::GetInstance()->GenThread(iThreadType, i); 
            m_vecThread.push_back(pThread);  
        }
    }

    CThreadPool::~CThreadPool()
    {
        for (int i = 0; i< m_vecThread.size(); ++i)
        {
            delete m_vecThread[i]; 
        }

        m_vecThread.clear();
    }

    void CThreadPool::Run()
    {
        for (int i = 0; i < m_iTotalThreadNum; ++i)
        {
            m_vecThread[i]->Run();
        }
    }

    void CThreadPool::SendCmd(CCommand *pCmd)
    {
        int iThreadNum = pCmd->GetAddr().m_cDstAddr.m_iThreadId;

        if (pCmd->GetAddr().m_cDstAddr.m_iThreadId == -1)
        {
            iThreadNum = (m_iCurrentNum++) % m_iTotalThreadNum;	
        }

        m_vecThread[iThreadNum]->SendCmd(pCmd);
    }

    void CThreadPool::PostCmd(CCommand *pCmd)
    {
        int iThreadNum = pCmd->GetAddr().m_cDstAddr.m_iThreadId;

        if (iThreadNum == -1)
        {
            iThreadNum = (m_iCurrentNum++) % m_iTotalThreadNum;	
        }

        m_vecThread[iThreadNum]->PostCmd(pCmd);
    }

    void CThreadPool::Stop()
    {
        for (int i = 0; i < m_iTotalThreadNum; ++i)
        {
            NsqLogPrintf(LOG_DEBUG, "threadtype = %d, total = %d, i = %d", m_iThreadType, m_iTotalThreadNum, i);
            m_vecThread[i]->Stop();
        }
    }

    int32_t CThreadPool::GetThreadType()
    {
        return m_iThreadType;
    }

    CThreadMgr::~CThreadMgr()
    {
        map<int32_t, CThreadPool *>::iterator iter = m_mapThreadPool.begin();

        for (; iter != m_mapThreadPool.end(); ++iter)
        {
            delete iter->second; 
        }

        m_mapThreadPool.clear();
    }
    void CThreadMgr::RegisterThreadPool(CThreadPool *pThreadPool)
    {
        NsqLogPrintf(LOG_DEBUG, "registerthreadpool threadtype = %d", pThreadPool->GetThreadType());
        m_mapThreadPool[pThreadPool->GetThreadType()] = pThreadPool;
    }

    void CThreadMgr::SendCmd(CCommand *pCmd) 
    {
        if (m_mapThreadPool.find(pCmd->GetAddr().m_cDstAddr.m_iThreadType) != m_mapThreadPool.end())
        {
            m_mapThreadPool[pCmd->GetAddr().m_cDstAddr.m_iThreadType]->SendCmd(pCmd);
        }
    }

    void CThreadMgr::PostCmd(CCommand *pCmd) 
    {
        if (m_mapThreadPool.find(pCmd->GetAddr().m_cDstAddr.m_iThreadType) != m_mapThreadPool.end())
        {
            m_mapThreadPool[pCmd->GetAddr().m_cDstAddr.m_iThreadType]->PostCmd(pCmd);
        }
    }

    void CThreadMgr::Stop()
    {

        std::map<int32_t, CThreadPool*>::iterator iter = m_mapThreadPool.begin();

        for (; iter != m_mapThreadPool.end(); iter++)
        {
            iter->second->Stop();
        }
    } 

    void CThreadMgr::Run()
    {
        std::map<int32_t, CThreadPool*>::iterator iter = m_mapThreadPool.begin();

        for (; iter != m_mapThreadPool.end(); ++iter)
        {
            iter->second->Run();
        }
    }
};
