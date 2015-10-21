#include "net_thread.h"
name NSQTOOL
{
    CThread::CThread()
    {
        pthread_mutex_init(&m_mutex, NULL);
        pthread_mutex_init(&m_mutex2, NULL);
        pthread_cond_init(&m_cond, NULL);
        pthread_cond_init(&m_condWait, NULL);
        m_bStop = false;
    }
    
    CThread::~CThread()
    {
        pthread_mutex_destroy(&m_mutex);
        pthread_mutex_destroy(&m_mutex2);
        pthread_cond_destroy(&m_cond);
        pthread_cond_destroy(&m_condWait);
    }
    
    int32_t CThread::Init(int32_t iThreadType, int32_t iThreadId, void *pArg)
    {
        m_iThreadType = iThreadType;
        m_iThreadId = iThreadId;
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
    void CThread::SendCmd(const CCommand &cCmd)
    {
        pthread_mutex_lock(&m_mutex2);
        RealProcessCmd(&cCmd);
        pthread_mutex_unlock(&m_mutex2);
    }
    
    //异步消息
    void CThread::PostCmd(const CCommand &cCmd)
    {
        pthread_mutex_lock(&m_mutex);
        m_lstCmd.push_back(cCmd);
        
        if (m_lstCmd.size() == 1)
        {
            pthread_cond_signal(&m_cond);
        }

        pthread_mutex_unlock(&m_mutex);
    }
    
    void CThread::RealProcessCmd(const CCommand &cCmd)
    {
       // fprintf(stdout, "CThread::ProcessCmd(cmd)\n");
        if (cCmd.GetCmdType() == STOP_TYPE)
        {
            m_lstCmd.clear();	
            m_bStop = true;
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
                return iRet;
            }
            
            CCommand cCmd = *m_lstCmd.begin();
            m_lstCmd.erase(m_lstCmd.begin());
            pthread_mutex_unlock(&m_mutex);
            
            pthread_mutex_lock(&m_mutex2);
            RealProcessCmd(cCmd);
            pthread_mutex_unlock(&m_mutex2);
            
            iRet ++;
        }

        return iRet;
    }

    void CThread::RealRun()
    {
        while (!m_bStop)
        {
            uint32_t iRet = ProcessCmd();
            
            if (iRet ==0 && !m_bStop)
            {
                pthread_cond_wait(&m_cond, &m_mutex);
            }
        }	

        pthread_signal(&m_condWait);
    }

    void CThread::Run()
    {
        pthread_create(&m_tid, NULL, ThreadFunc, this);	
    }
    
    void CThread::Stop()
    {
        m_bStop = true;
        pthread_cond_wait(&m_condWait, &m_mutex);
    }

    void *CThread::ThreadFunc(void *pArgs)
    {
        CThread *pThread = (CThread *)pArgs;	
        pThread->RealRun();
    }

    ////////////////////////////////////////////////////////////////////////////////////
    //                          CThreadMgr
    ///////////////////////////////////////////////////////////////////////////////////
    
    void CThreadMgr::RegisterThreadPool(CThreadPoolInterface *pThreadPool)
    {
        fprintf(stdout, "threadtype = %d\n", pThreadPool->GetThreadType());
        m_mapThreadPool[pThreadPool->GetThreadType()] = pThreadPool;
    }

    void CThreadMgr::SendCmd(int32_t iThreadType, CCommand &cCmd, int32_t iThreadNum) 
    {
        fprintf(stdout, "SendCmd threadtype = %d\n", iThreadType);
        if (m_mapThreadPool[iThreadType] != NULL)
        {
            fprintf(stdout, "is not NULL\n");
            m_mapThreadPool[iThreadType]->SendCmd(cCmd, iThreadNum);
        }
    }

    void CThreadMgr::PostCmd(uint32_t iThreadType, CCommand &cCmd, uint32_t iThreadNum) 
    {
        if (m_mapThreadPool[iThreadType] != NULL)
        {
            m_mapThreadPool[iThreadType]->PostCmd(cCmd, iThreadNum);
        }
    }

    void CThreadMgr::Stop()
    {
        std::map<int32_t, CThreadPoolInterface*>::iterator iter = m_mapThreadPool.begin();

        for (; iter != m_mapThreadPool.end(); ++iter)
        {
            iter->second->Stop();
            delete iter->second;
            iter = m_mapThreadPool.begin();
        }
    } 
};
