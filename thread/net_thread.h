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
	class CNetThread:public CThread
	{
	public:
		struct SNetContext
		{
            int m_iProtocolType;
            int m_iProtocolId;
            int m_iFd;
            string m_strHost;
            uint16_t m_iPort;
			bufferevent *m_pBufevt;
            CNetHandler *m_pHandler;
		};

		CNetThread(int32_t iThreadType, int32_t iThreadId);

        ~CNetThread() {pthread_mutex_destroy(&m_mutex);}

		static void OnStaticRead(struct bufferevent *pBufevt, void *arg);
		static void OnStaticError(struct bufferevent *pBufevt, 
                                short iTemp, void *arg);
		void RealRun();
        void DestoryFd(uint64_t iHandleId);
    protected:
		void RealProcessCmd(CCommand &cCmd);
		int  SendData(int iFd, const std::string *pString);
    private:
		void OnRead(struct bufferevent *pBufevt, void *arg);
		void OnError(struct bufferevent *pBufevt, short iTemp, void *arg);
	private:
		event_base *m_pEventBase;
		std::map<uint64_t, SNetContext *> m_mapNetContext;
        pthread_mutex_t m_mutex;
	};

	class CListenThread:public CThread
	{
	public:
		struct SListenInfo
		{
            std::string m_strHost;
            uint16_t m_iPort;
			int m_iProtocolType;	
            int m_iProtocolId
			evconnlistener *m_pListener;
            CHandler *m_pHandler;
		};


		int32_t Init(int32_t iThreadType, int32_t iThreadId, void *pArg);
		void RealRun();
		static void OnStaticRead(struct evconnlistener *pListener, 
                            evutil_socket_t iAcceptHandle, 
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
