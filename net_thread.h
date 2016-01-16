#ifndef _NET_THREAD_H_
#define _NET_THREAD_H_
#include "event_thread.h"
#include "event2/event.h"
#include "event2/bufferevent.h"
#include "event2/buffer.h"
#include "event2/listener.h"
#include <string>
#include <set>
#include <map>
#include <stdint.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

using namespace std;

namespace NSQTOOL
{
    class CTcpHandler;
    class CListenHandler;

	class CNetThread:public CEventThread
	{
	public:
	    CNetThread(int32_t iThreadType, int iThreadId);
		static void OnStaticRead(struct bufferevent *pBufevt, void *arg);
		static void OnStaticWrite(struct bufferevent *pBufevt, void *arg);
		static void OnStaticError(struct bufferevent *pBufevt, 
                                short iTemp, void *arg);
	};

	class CListenThread:public CEventThread
	{
	public:
		CListenThread(int32_t iThreadType, int iThreadId);
		static void OnStaticRead(struct evconnlistener *pListener, 
                            evutil_socket_t iAcceptHandle, 
                            struct sockaddr *pAddr, int socklen, void *pArg);		
	};
};
#endif
