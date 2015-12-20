#ifndef _NSQLOOKUP_HANDLER_
#define _NSQLOOKUP_HANDLER_
#include <iostream>
#include <string>
using namespace std;

namespace NSQTOOL
{
    class CTcpHandler;

    class CNsqLookupHandler:public CTcpHandler
    {
    public: 
        CNsqLookupHandler(int iProtocolType, int iProtocolId,
                uint64_t iHandleId, CThread *pThread);
        virtual void OnConnect();
        virtual void OnError(int iErrorNo);
        virtual void ProcessRead();
        virtual void ProcessCmd(CCommand &cCmd);
    };
};
#endif
