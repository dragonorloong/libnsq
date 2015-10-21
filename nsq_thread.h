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
		//异步消息
		void SendCmd(const CCommand &cCmd);
		//同步消息
		void PostCmd(const CCommand &cCmd);
		virtual void RealProcessCmd(const CCommand &cCmd);
		int32_t ProcessCmd();
		virtual void RealRun();
		void Run();
		void Stop();
		static void *ThreadFunc(void *pArgs);
	protected:
		std::list<CCommand> m_lstCmd;				
		pthread_mutex_t m_mutex;	
		pthread_mutex_t m_mutex2;	
		pthread_cond_t m_cond;
        pthread_cond_t m_condWait;
		bool m_bStop;
		int32_t m_iThreadType;
		int32_t m_iThreadId;
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
        ~CThreadPool()
        {
            delete [] T;    
        }

		int32_t Init(uint32_t iThreadType, uint32_t iNum, void *pArg)	
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
		void RegisterThreadPool(CThreadPoolInterface *pThreadPool);
		void SendCmd(int32_t iThreadType, CCommand &cCmd, int32_t iThreadNum); 
		void PostCmd(uint32_t iThreadType, CCommand &cCmd, uint32_t iThreadNum); 
		void Stop();
	private:
		std::map<int32_t, CThreadPoolInterface*> m_mapThreadPool;
	};

	typedef CSingleton<CThreadMgr> CThreadMgrSingleton;
};

#endif
