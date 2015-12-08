#ifndef _NET_THREAD_H_
#define _NET_THREAD_H_
#include "nsq_thread.h"
#include "event2/event.h"
#include "event2/bufferevent.h"
#include "event2/buffer.h"
#include <string>
#include <set>
#include <map>
#include <stdint.h>
#include <errno.h>
#include "listener.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

namespace NSQTOOL
{
    class CData;
	class CNetThread:public CThread
	{
	public:
		struct SNetContext
		{
			int32_t m_iHandle;
			bufferevent *m_pBufevt;
			int32_t m_iPkgType;
			CData *m_pPkg;
            int32_t m_iPkdgIndex; //相同类型的包根据索引来区分处理
            CCommand::CCmdAddr m_cAddr;
            std::string m_strHost;
            uint16_t m_iPort;
			void Init(int32_t iHandle, bufferevent *pBufevt);
		};

		CNetThread():m_pEventBase(NULL){pthread_mutex_init(&m_mutex, NULL);}
		~CNetThread() {pthread_mutex_destroy(&m_mutex);}
		int32_t Init(int32_t iThreadType, int32_t iThreadId, void *pArg);
		static void OnStaticRead(struct bufferevent *pBufevt, void *arg);
		static void OnStaticError(struct bufferevent *pBufevt, short iTemp, void *arg);
		void RealProcessCmd(CCommand &cCmd);
		void RealRun();
		int  SendData(int iHandle, const std::string *pString);
    private:
		void OnRead(struct bufferevent *pBufevt, void *arg);
		void OnError(struct bufferevent *pBufevt, short iTemp, void *arg);
	private:
		event_base *m_pEventBase;
		std::map<int, SNetContext *> m_mapNetContext;
        pthread_mutex_t m_mutex;
	};

	class CListenThread:public CThread
	{
	public:
		struct SListenInfo
		{
            std::string m_strHost;
            uint16_t m_iPort;
			int m_iPkgType;	
			evconnlistener *m_pListener;
            CCommand::CCmdAddr m_cAddr;
		};

		int32_t Init(int32_t iThreadType, int32_t iThreadId, void *pArg);
		void RealRun();
		static void OnStaticRead(struct evconnlistener *pListener, evutil_socket_t iAcceptHandle, 
                            struct sockaddr *pAddr, int socklen, void *pArg);		
		void RealProcessCmd(CCommand &cCmd);
    private:
		 void OnRead(struct evconnlistener *pListener, evutil_socket_t iAcceptHandle, 
                            struct sockaddr *pAddr, int socklen, void *pArg);		

	private:
		event_base *m_pEventBase;
		std::map<int, SListenInfo*> m_mapListen;
	};
};
#endif
