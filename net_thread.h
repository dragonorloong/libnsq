#ifndef _NET_THREAD_H_
#define _NET_THREAD_H_
#include "nsq_thread.h"
#include "protocol.h"
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
			int32_t m_iHandle;
			bufferevent *m_pBufevt;
			int32_t m_iPkgType;
			CData *m_pPkg;
            CCommand::CCmdAddr m_cAddr;
            string m_strHost;
            uint16_t m_iPort;

			void Init(int32_t iHandle, bufferevent *pBufevt,  int32_t iPkgType)
			{
				m_iHandle = iHandle;
				m_pBufevt = pBufevt;
				m_iPkgType = iPkgType;
				m_pPkg = CData::New(iPkgType);
			}
		};

		CNetThread():m_pEventBase(NULL){}
		~CNetThread()
		int32_t Init(int32_t iThreadType, int32_t iThreadId, void *pArg);
		static void OnRead(struct bufferevent *pBufevt, void *arg);
		static void OnError(struct bufferevent *pBufevt, short iTemp, void *arg);
		int  SendData(int iHandle, std::string *pString);
		void RealProcessCmd(const CCommand &cCmd);
		void RealRun();
	private:
		event_base *m_pEventBase;
		std::map<int, SNetContext *> m_mapNetContext;
	};

	class CListenThread:public CThread
	{
	public:
		struct SListenInfo
		{
            string m_strHost;
            uint16_t m_iPort;
			int m_iPkgType;	
			evconnlistener *m_pListener;
            CCommand::CCmdAddr m_cAddr;
		};

		int32_t Init(int32_t iThreadType, int32_t iThreadId, void *pArg);
		void RealRun();
		static void OnRead(struct evconnlistener *pListener, evutil_socket_t iAcceptHandle, 
                            struct sockaddr *pAddr, int socklen, void *pArg);		
		void RealProcessCmd(const CCommand &cCmd);
	private:
		event_base *m_pEventBase;
		std::map<int, SListenInfo*> m_mapListen;
	};
};
#endif
