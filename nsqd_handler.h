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
    class CNsqdHandler:public CTcpHandler
    {
    public: 
        CNsqdHandler(int iProtocolType, int iProtocolId, 
                uint64_t iHandleId, CThread *pThread);
        virtual void OnConnect();
        virtual void OnError(int iErrorNo);
        virtual int ProcessRead();
        virtual void ProcessCmd(CCommand &cCmd);
    };
};
#endif
