#ifndef _NSQD_HANDLER_
#define _NSQD_HANDLER_
#include <iostream>
#include <string>
using namespace std;
#include "factory.h"
#include "event2/bufferevent.h"

namespace NSQTOOL
{
    class CTcpHandler;
    class CHttpHandler:public CTcpHandler
    {
    public: 
        CHttpHandler(int iCmdType, int iCmdId, 
                uint64_t iHandleId, CThread *pThread, int iConnType);
        virtual void OnConnect();
        virtual void OnError(int iErrorNo);
        virtual int ProcessRead();
        void ProcessCmd(CCommand *pCmd);
    };
};
#endif
