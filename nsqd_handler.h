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
        CNsqdHandler(int iCmdType, int iCmdId, 
                uint64_t iHandleId, CThread *pThread);
        virtual void OnConnect();
        virtual void OnError(int iErrorNo);
        virtual int ProcessRead();
        void ProcessCmd(CCommand *pCmd);
    private:
        string m_strTopic;
        string m_strChannel;
    };
};
#endif
