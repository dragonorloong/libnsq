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

		CThread();
		~CThread();
		virtual int32_t Init(int32_t iThreadType, int32_t iThreadId, void *pArg = NULL);
		int32_t GetThreadId();
		int32_t GetThreadType();
		//同步消息
		void SendCmd(CCommand &cCmd);
		//异步消息
		void PostCmd(CCommand &cCmd);
		virtual void RealProcessCmd(CCommand &cCmd);
		int32_t ProcessCmd();
		virtual void RealRun();
		void Run();
		void Stop();
		static void *ThreadFunc(void *pArgs);
	protected:
		std::list<CCommand> m_lstCmd;				
		pthread_mutex_t m_mutex;	
		pthread_mutex_t m_mutexSync; //同步	
		pthread_cond_t m_cond;
        pthread_cond_t m_condWait; //退出等待
		bool m_bStop;
		int32_t m_iThreadType;
		int32_t m_iThreadId;
        pthread_t m_iTid; //系统线程id
	};

	class CThreadPoolInterface
	{
	public:
		virtual int32_t Init(int32_t iThreadType, int32_t iNum, void *pArg)=0;	
		virtual void Run()=0;
		virtual void SendCmd(CCommand &cCmd, int32_t iThreadNum = 0)=0;	
		virtual void PostCmd(CCommand &cCmd, int32_t iThreadNum = 0)=0;
		virtual void Stop()=0;
		virtual int32_t GetThreadType() = 0;
	};

	template<typename T>
	class CThreadPool:public CThreadPoolInterface
	{
	public:		
        CThreadPool()
        {
		    m_pThread = NULL;
		    m_iTotalThreadNum = 0;
		    m_iCurrentNum = 0;
		    m_iThreadType = 0;
            m_bIsAdd = false;
        }

        ~CThreadPool()
        {
            if (m_bIsAdd)
            {
                delete m_pThread;
            }
            else
            {
                delete [] m_pThread;    
            }
        }

		int32_t Init(int32_t iThreadType, int32_t iNum, void *pArg)	
		{
			m_iThreadType = iThreadType;
			m_iCurrentNum = 0;
			m_iTotalThreadNum = iNum;
			m_pThread = new T[iNum];				
			
			for (int i = 0; i < iNum; ++i)
			{
				m_pThread[i].Init(iThreadType, iNum, pArg);
			}
		}

        void Add(int32_t iThreadType, T *pThis)
        {
            m_pThread = pThis;
            m_iThreadType = iThreadType;
            m_iTotalThreadNum = 1;
            m_bIsAdd = true;
        }

		void Run()
		{
			for (int i = 0; i < m_iTotalThreadNum; ++i)
			{
                if (!m_bIsAdd)
                {
				    (&m_pThread[i])->Run();
                }
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

		int32_t GetThreadType()
		{
			return m_iThreadType;
		}

	private:
		T *m_pThread;
		int32_t m_iTotalThreadNum;
		uint64_t m_iCurrentNum;
		int32_t m_iThreadType;
        bool m_bIsAdd;
	};

	class CThreadMgr
	{
	friend CSingleton<CThreadMgr>;
	private:
		CThreadMgr()
		{

		}
	public:
		void RegisterThreadPool(CThreadPoolInterface *pThreadPool);
		void SendCmd(int32_t iThreadType, CCommand &cCmd, int32_t iThreadNum); 
		void PostCmd(int32_t iThreadType, CCommand &cCmd, int32_t iThreadNum); 
		void Stop();
        void Run();
	private:
		std::map<int32_t, CThreadPoolInterface*> m_mapThreadPool;
	};

	typedef CSingleton<CThreadMgr> CThreadMgrSingleton;
};

#endif
