#ifndef _LISTEN_HANDLER_
#define _LISTEN_HANDLER_
#include <iostream>
#include <string>
#include <map>
#include "event2/bufferevent.h"
#include "event2/listener.h"

using namespace std;
namespace NSQTOOL
{
    class CListenHandler:public CHandler
    {
    public:
        CListenHandler(int iCmdType, int iCmdId,
                uint64_t iHandlerId, CThread *pThread);
        ~CListenHandler();

        virtual void OnError(int iErrorNo = 0)
        {

        }

        virtual void ProcessCmd(CCommand *pCmd);

        virtual void TcpListenAdd(CCommand *pCmd);
        virtual void TcpListenDel(CCommand *pCmd)
        {
        }

        virtual void TcpListenAccept(int iAcceptFd, struct sockaddr *pAddr, int socklen);

    protected:
	    evconnlistener *m_pListener;
        string m_strHost;
        uint16_t m_iPort;
        int m_iCmdType;
        int m_iCmdId;
    };
};

#endif
