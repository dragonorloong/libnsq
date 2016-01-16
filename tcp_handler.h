#ifndef _TCP_HANDLER_
#define _TCP_HANDLER_
#include <iostream>
#include <string>
#include "factory.h"
#include "event2/bufferevent.h"
#include "command.h"
#include "handler.h"

using namespace std;

namespace NSQTOOL
{
    class CTcpHandler:public CHandler
    {
    public: 
        CTcpHandler(int iCmdType, int iCmdId,
                uint64_t iHandlerId, CThread *pThread, int iConnType);
        ~CTcpHandler();

        virtual void OnConnect();
        virtual void OnError(int iErrorNo);
        virtual int OnRead(const char *pData, int iLength);
        virtual int OnWrite(int iLength);
        virtual int ProcessRead();
        virtual void ProcessCmd(CCommand *pCmd);

        virtual void TcpConnect(CCommand *pCmd);
        virtual void TcpSend(CCommand *pCmd);
        virtual void TcpAdd(CCommand *pCmd); 
        virtual void TcpRead();
        virtual void TcpWrite();
        virtual void TcpDelete(int iErrorType);
        void SendData(const char *pData, int32_t iLength);
        void SetTimeout();

        void SetBufferevent(bufferevent *pBufevt)
        {
            m_pBufevt = pBufevt; 
        }

        bufferevent *GetBufferevent()
        {
            return m_pBufevt; 
        }

    protected:
        CProtocol *m_pProtocol;
        int m_iCmdType;
        int m_iCmdId;
        int m_iConnType;
        bufferevent *m_pBufevt;
        string m_strHost;
        uint16_t m_iPort;
    };
};
#endif
