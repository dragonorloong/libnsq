#ifndef _TCP_HANDLER_
#define _TCP_HANDLER_
#include <iostream>
#include <string>
using namespace std;
#include "factory.h"
#include "event2/bufferevent.h"

namespace NSQTOOL
{
    class CHandler;
    class CListenHandler;

    class CTcpHandler:public CHandler
    {
    public: 
        CTcpHandler(int iProtocolType, int iProtocolId,
                uint64_t iHandlerId, CThread *pThread);

        virtual void OnConnect();
        virtual void OnError(int iErrorNo);
        virtual int OnRead(const char *pData, int iLength);
        virtual void ProcessRead();
        virtual void ProcessCmd(CCommand &cCmd);

        void SetListenHandler(CListenHandler *pListenHandler)
        {
            m_pListenHandler = pListenHandler; 
        }

        CListenHandler * GetListenHandler()
        {
            return m_pListenHandler; 
        }

        void SetBufferevent(bufferevent *pBufevt)
        {
            m_pBufevt = pBufevt; 
        }

        bufferevent *GetBufferevent()
        {
            return m_pBufevt; 
        }

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

        void GetPort()
        {
            return m_iPort; 
        }

    protected:
        CProtocol *m_pProtocol;
        string m_strHost;
        uint16_t m_iPort;
        int m_iProtocolType;
        int m_iProtocolId;
        bufferevent *m_pBufevt;
        CListenHandler *m_pListenHandler;
    };
};
#endif
