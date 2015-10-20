#ifndef _NSQ_THREAD_H_
#define _NSQ_THREAD_H_
#include <pthread.h>
#include "command.h"
#include "tool.h"
#include <stdint.h>
#include <map>
#include <list>
#include <unistd.h>

namespace NSQTOOL
{
	class CThread
	{
	public:

		CThread()
		{
			pthread_mutex_init(&m_mutex, NULL);
			pthread_mutex_init(&m_mutex2, NULL);
			pthread_cond_init(&m_cond, NULL);
			m_bStop = false;
		}
		
		~CThread()
		{
			pthread_mutex_destroy(&m_mutex);
			pthread_mutex_destroy(&m_mutex2);
			pthread_cond_destroy(&m_cond);
		}
		
		virtual int32_t Init(void *pArg)
		{
			m_iThreadType = (int64_t)pArg;
		}

		pthread_t GetThreadId()
		{
			return m_tid;
		}
		
		int32_t GetThreadType()
		{
			return m_iThreadType;
		}

		//异步消息
		void SendCmd(CCommand &cCmd)
		{
			pthread_mutex_lock(&m_mutex);
			m_lstCmd.push_back(cCmd);
			
			if (m_lstCmd.size() == 1)
			{
				pthread_cond_signal(&m_cond);
			}

			pthread_mutex_unlock(&m_mutex);
		}
		
		//同步消息
		void PostCmd(CCommand &cCmd)
		{
			pthread_mutex_lock(&m_mutex2);
			RealProcessCmd(&cCmd);
			pthread_mutex_unlock(&m_mutex2);
		}
		
		virtual void RealProcessCmd(CCommand *cCmd)
		{
           // fprintf(stdout, "CThread::ProcessCmd(cmd)\n");
			if (cCmd->GetCmdType() == STOP_TYPE)
			{
				m_lstCmd.clear();	
			}
		}
		
		uint32_t ProcessCmd(CThread *pThis)
		{
            //fprintf(stdout, "CThread::ProcessCmd()\n");
			uint32_t iRet = 0;
			
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
				pThis->RealProcessCmd(&cCmd);
				pthread_mutex_unlock(&m_mutex2);
				
				iRet ++;
			}

            //fprintf(stdout, "CThread::ProcessCmd() end\n");

			return iRet;
		}

		virtual void RealRun()
		{
            fprintf(stdout, "CThread RealRun\n");
            //sleep(10);
			while (!m_bStop)
			{
				uint32_t iRet = ProcessCmd(this);
				
				if (iRet ==0 )
				{
					pthread_cond_wait(&m_cond, &m_mutex);
				}
			}	
		}

		void Run()
		{
			pthread_create(&m_tid, NULL, ThreadFunc, this);	
		}
		
		void Stop()
		{
			m_bStop = true;
		}

		static void *ThreadFunc(void *pArgs)
		{
			CThread *pThread = (CThread *)pArgs;	
			pThread->RealRun();
		}
		
	protected:
		std::list<CCommand> m_lstCmd;				
		pthread_mutex_t m_mutex;	
		pthread_mutex_t m_mutex2;	
		pthread_cond_t m_cond;
		pthread_t m_tid;
		bool m_bStop;
		int32_t m_iThreadType;
	};

	class CThreadPoolInterface
	{
	public:
		virtual int32_t Init(uint32_t iThreadType, uint32_t iNum, void *pArg)=0;	
		virtual void Run()=0;
		virtual void SendCmd(CCommand &cCmd, int32_t iThreadNum = 0)=0;	
		virtual void PostCmd(CCommand &cCmd, int32_t iThreadNum = 0)=0;
		virtual void Stop()=0;
		virtual uint32_t GetThreadType() = 0;
	};

	template<typename T>
	class CThreadPool:public CThreadPoolInterface
	{
	public:		
		int32_t Init(uint32_t iThreadType, uint32_t iNum, void *pArg)	
		{
			m_iThreadType = iThreadType;
			m_iCurrentNum = 0;
			m_iTotalThreadNum = iNum;
			m_pThread = new T[iNum];				
			
			for (int i = 0; i < iNum; ++i)
			{
				m_pThread[i].Init(pArg);
			}
		}

		void Run()
		{
			for (int i = 0; i < m_iTotalThreadNum; ++i)
			{
				(&m_pThread[i])->Run();
			}
		}

		void SendCmd(CCommand &cCmd, int32_t iThreadNum = 0)
		{
			if (iThreadNum == 0)
			{
				iThreadNum = (m_iCurrentNum++) % m_iTotalThreadNum;	
			}

			m_pThread[iThreadNum].SendCmd(cCmd);
		}

		void PostCmd(CCommand &cCmd, int32_t iThreadNum = 0)
		{
			if (iThreadNum == 0)
			{
				iThreadNum = (m_iCurrentNum++) % m_iTotalThreadNum;	
			}

			m_pThread[iThreadNum].PostCmd(cCmd);
		}

		void Stop()
		{
			for (int i = 0; i < m_iTotalThreadNum; ++i)
			{
				m_pThread[i].Stop();
			}
		}

		uint32_t GetThreadType()
		{
			return m_iThreadType;
		}

	private:
		T *m_pThread;
		uint32_t m_iTotalThreadNum;
		uint64_t m_iCurrentNum;
		uint32_t m_iThreadType;
	};

	class CThreadMgr
	{
	friend CSingleton<CThreadMgr>;
	private:
		CThreadMgr()
		{

		}
	public:
		void RegisterThreadPool(CThreadPoolInterface *pThreadPool)
		{
            fprintf(stdout, "threadtype = %d\n", pThreadPool->GetThreadType());
			m_mapThreadPool[pThreadPool->GetThreadType()] = pThreadPool;
		}

		void SendCmd(int32_t iThreadType, CCommand &cCmd, int32_t iThreadNum) 
		{
            fprintf(stdout, "SendCmd threadtype = %d\n", iThreadType);
			if (m_mapThreadPool[iThreadType] != NULL)
			{
                fprintf(stdout, "is not NULL\n");
				m_mapThreadPool[iThreadType]->SendCmd(cCmd, iThreadNum);
			}
		}

		void PostCmd(uint32_t iThreadType, CCommand &cCmd, uint32_t iThreadNum) 
		{
			if (m_mapThreadPool[iThreadType] != NULL)
			{
				m_mapThreadPool[iThreadType]->PostCmd(cCmd, iThreadNum);
			}
		}

		void Stop()
		{
			std::map<int32_t, CThreadPoolInterface*>::iterator iter = m_mapThreadPool.begin();

			for (; iter != m_mapThreadPool.end(); ++iter)
			{
				iter->second->Stop();
				delete iter->second;
				iter = m_mapThreadPool.begin();
			}
		}

	private:
		std::map<int32_t, CThreadPoolInterface*> m_mapThreadPool;
	};

	typedef CSingleton<CThreadMgr> CThreadMgrSingleton;
};

#endif
