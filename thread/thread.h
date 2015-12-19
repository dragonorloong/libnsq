#ifndef _THREAD_H_
#define _THREAD_H_
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
		CThread(int32_t iThreadType, int32_t iThreadId, void *pArg = NULL);
		~CThread();

		int32_t GetThreadId();
		int32_t GetThreadType();
		//同步消息
		void SendCmd(CCommand &cCmd);
		//异步消息
		void PostCmd(CCommand &cCmd);
		void Run();
		void Stop();
		virtual void RealRun();

    protected:
		virtual void RealProcessCmd(CCommand &cCmd);
		int32_t ProcessCmd();
        virtual DestoryHandler(uint64_t iHandleId);
        uint64_t GetHandleId();

    public:
		static void *ThreadFunc(void *pArgs);

	protected:
		list<CCommand> m_lstCmd;				
        map<uint64_t, CHandler *> m_mapHandler;
		pthread_mutex_t m_mutex;	
		pthread_mutex_t m_mutexSync; //同步	
		pthread_cond_t m_cond;

		bool m_bStop;
		int32_t m_iThreadType;
		int32_t m_iThreadId;
        pthread_t m_iTid; //系统线程id

        uint64_t m_iHandleIdInc; 
	};

	class CThreadPool
	{
	public:		
        CThreadPool(int iThreadType, int iThreadNum)
        ~CThreadPool()
		void Run()
		void SendCmd(CCommand &cCmd, int32_t iThreadNum = 0)
		void PostCmd(CCommand &cCmd, int32_t iThreadNum = 0)
		void Stop()
		int32_t GetThreadType()

	private:
		vector<CThread *> m_vecThread;
		int32_t m_iTotalThreadNum;
		uint64_t m_iCurrentNum;
		int32_t m_iThreadType;
	};

	class CThreadMgr
	{
	friend CSingleton<CThreadMgr>;
	private:
		CThreadMgr() { }
	public:
		void RegisterThreadPool(CThreadPool *pThreadPool);
		void SendCmd(int32_t iThreadType, int32_t iThreadNum, CCommand &cCmd); 
		void PostCmd(int32_t iThreadType, int32_t iThreadNum, CCommand &cCmd); 
		void Stop();
        void Run();
	private:
		std::map<int32_t, CThreadPool*> m_mapThreadPool;
	};

	typedef CSingleton<CThreadMgr> CThreadMgrSingleton;
};

#endif
