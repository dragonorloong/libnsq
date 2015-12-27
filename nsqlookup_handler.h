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
        CNsqLookupHandler(int iCmdType, int iCmdId,
                uint64_t iHandleId, CThread *pThread);
        virtual void OnConnect();
        virtual int ProcessRead();
        void ProcessCmd(CCommand *pCmd);
    private:
        string m_strTopic;
        string m_strChannel;
    };
};
#endif
