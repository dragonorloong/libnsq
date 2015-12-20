#include "net_thread.h"
namespace NSQTOOL
{
    CThread::CThread(int32_t iThreadType, int32_t iThreadId, void *pArg = NULL)
    {
        pthread_mutexattr_t attr;
        pthread_mutexattr_init(&attr);
        pthread_mutexattr_settype(&attr,PTHREAD_MUTEX_RECURSIVE);
        pthread_mutex_init(&m_mutex, &m_mutex);
        pthread_mutex_init(&m_mutexSync, &m_mutex);
        pthread_mutexattr_destory(&m_mutex);
        pthread_cond_init(&m_cond, NULL);
        m_bStop = false;

        fprintf(stdout, "Init ThreadType = %d, iThreadId = %d\n", iThreadType, iThreadId);

        m_iThreadType = iThreadType;
        m_iThreadId = iThreadId;
        m_iHandleIdInc = 0;
    }
    
    CThread::~CThread()
    {
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
    void CThread::SendCmd(CCommand &cCmd)
    {
        pthread_mutex_lock(&m_mutexSync);
        RealProcessCmd(cCmd);
        pthread_mutex_unlock(&m_mutexSync);
    }
    
    //异步消息
    void CThread::PostCmd(CCommand &cCmd)
    {
        pthread_mutex_lock(&m_mutex);
        m_lstCmd.push_back(cCmd);
        
        if (m_lstCmd.size() == 1)
        {
            fprintf(stdout, "post size = 1, type = %d, signal\n", m_iThreadType);
            pthread_cond_signal(&m_cond);
        }

        pthread_mutex_unlock(&m_mutex);
    }
    
    void CThread::RealProcessCmd(CCommand &cCmd)
    {
       // fprintf(stdout, "CThread::ProcessCmd(cmd)\n");
        if (cCmd.GetCmdType() == STOP_TYPE)
        {
            m_lstCmd.clear();	
            m_bStop = true;
        }
        else
        {
           if (m_mapHandler.find(cCmd.GetAddr().m_iDstHandleId) != m_mapHandler.end())
           {
                m_mapHandler[cCmd.GetAddr().m_iDstHandleId]->ProcessCmd(cCmd);         
           }
           else
           {
                fprintf(stdout, "error cmd\n"); 
           }
        }
    }
    
    int32_t CThread::ProcessCmd()
    {
        if (m_iThreadType == -4)
        {
            fprintf(stdout, "ProcessCmd size = %d, type = %d\n", m_lstCmd.size(), m_iThreadType);
        }

        int32_t iRet = 0;
        
        while (m_lstCmd.size() != 0)
        {
            pthread_mutex_lock(&m_mutex);
            
            if (m_lstCmd.size() == 0)
            {
                fprintf(stdout, "size() reset 0 type = %d\n", m_iThreadType);
                pthread_mutex_unlock(&m_mutex);
                return iRet;
            }
            
            CCommand cCmd = *m_lstCmd.begin();
            m_lstCmd.erase(m_lstCmd.begin());
            pthread_mutex_unlock(&m_mutex);
            
            pthread_mutex_lock(&m_mutexSync);
            fprintf(stdout, "realProcesCmd type = %d\n", m_iThreadType);
            RealProcessCmd(cCmd);
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
            fprintf(stdout, "iType = %d, iRet = %d\n", m_iThreadType, iRet);            

            if (iRet ==0 && !m_bStop)
            {
                fprintf(stdout, "size = 0, type = %d, wait\n", m_iThreadType);
                pthread_mutex_lock(&m_mutex);
                pthread_cond_wait(&m_cond, &m_mutex);
                pthread_mutex_unlock(&m_mutex);
                fprintf(stdout, "size = 1, alive, type = %d\n", m_iThreadType);
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
        pthread_join(m_iTid, NULL);
    }

    virtual CThread::DestoryHandler(uint64_t iHandleId)
    {
        pthread_mutex_lock(&m_mutex);

        if (m_mapHandler.find(iHandleId) != m_mapHandler.end())
        {
            delete m_mapHandler[iHandleId];
            m_mapHandler.erase(iHandleId);
        }

        pthread_mutex_unlock(&m_mutex);
    }

    uint64_t CThread::GetHandlerId()
    {
        return ++m_iHandleIdInc; 
    }

    void *CThread::ThreadFunc(void *pArgs)
    {
        CThread *pThread = (CThread *)pArgs;	
        pThread->RealRun();
    }

    void CThreadMgr::RegisterThreadPool(CThreadPoolInterface *pThreadPool)
    {
        fprintf(stdout, "threadtype = %d\n", pThreadPool->GetThreadType());
        m_mapThreadPool[pThreadPool->GetThreadType()] = pThreadPool;
    }

    void CThreadMgr::SendCmd(int32_t iThreadType, CCommand &cCmd, int32_t iThreadNum) 
    {
        fprintf(stdout, "SendCmd threadtype = %d\n", iThreadType);
        if (m_mapThreadPool.find(iThreadType) != m_mapThreadPool.end())
        {
            fprintf(stdout, "is not NULL\n");
            m_mapThreadPool[iThreadType]->SendCmd(cCmd, iThreadNum);
        }
    }

    void CThreadMgr::PostCmd(int32_t iThreadType, CCommand &cCmd, int32_t iThreadNum) 
    {
        fprintf(stdout, "CThreadMgr::PostCmd begin iThreadType = %d\n", iThreadType);
        if (m_mapThreadPool.find(iThreadType) != m_mapThreadPool.end())
        {
            fprintf(stdout, "CThreadMgr::PostCmd is not null\n");
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

    void CThreadMgr::Run()
    {
        std::map<int32_t, CThreadPoolInterface*>::iterator iter = m_mapThreadPool.begin();

        for (; iter != m_mapThreadPool.end(); ++iter)
        {
            iter->second->Run();
        }
    }

    CThreadPool::CThreadPool(int iThreadType, int iThreadNum)
    {
        m_pThread = NULL;
        m_iCurrentNum = 0;
        m_iTotalThreadNum = iThreadNum;
        m_iThreadType = iThreadType;
        
        for (int i = 0; i < iThreadNum; ++i)
        {
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

        m_vecThread.clean();
    }

    void CThreadPool::Run()
    {
        for (int i = 0; i < m_iTotalThreadNum; ++i)
        {
            m_vecThread[i]->Run();
        }
    }

    void CThreadPool::SendCmd(CCommand &cCmd, int32_t iThreadNum = 0)
    {
        if (iThreadNum == 0)
        {
            iThreadNum = (m_iCurrentNum++) % m_iTotalThreadNum;	
        }

        m_vecThread[iThreadNum]->SendCmd(cCmd);
    }

    void CThreadPool::PostCmd(CCommand &cCmd, int32_t iThreadNum = 0)
    {
        if (iThreadNum == 0)
        {
            iThreadNum = (m_iCurrentNum++) % m_iTotalThreadNum;	
        }

        m_vecThread[iThreadNum]->PostCmd(cCmd);
    }

    void CThreadPool::Stop()
    {
        for (int i = 0; i < m_iTotalThreadNum; ++i)
        {
            m_vecThread[i]->Stop();
        }
    }

    int32_t CThreadPool::GetThreadType()
    {
        return m_iThreadType;
    }
    
};
