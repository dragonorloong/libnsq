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
    class CHandler;
    class CTcpHandler;
    class CListenHandler:public CHandler
    {
    public:
        CListenHandler(int iProtocolType, int iProtocolId,
                uint64_t iHandlerId, CThread *pThread);

        virtual void OnError(CTcpHandler *pHandler = NULL, int iErrorNo = 0);

        virtual void OnAccept(CTcpHandler *pHandler);

        virtual void ProcessCmd(CCommand &cCmd);

        void SetHost(const string &strHost)
        {
            m_strHost = strHost; 
        }

        string GetHost()
        {
            return m_strHost; 
        }

        void SetPort(uint16_t iPort)
        {
            m_iPort = iPort;
        }

        uint16_t GetPort()
        {
            return m_iPort; 
        }

        void SetEvlisten(evconnlistener *pListener)
        {
            m_pListener = pListener; 
        }

        evconnlistener *GetEvlisten()
        {
            return m_pListener; 
        }

        int GetProtocolType()
        {
            return m_iProtocolType; 
        }

        int GetProtocolId()
        {
            return m_iProtocolId; 
        }

    protected:
        map<uint64_t, CTcpHandler *> m_mapTcpHandlerMgr;
	    evconnlistener *m_pListener;
        string m_strHost;
        uint16_t m_iPort;
        int m_iProtocolType;
        int m_iProtocolId;
    };
};
#endif
