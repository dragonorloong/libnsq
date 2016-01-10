#ifndef _NSQ_FACTORY_
#define _NSQ_FACTORY_
#include "singleton.h"
#include "factory.h"
#include "nsq_command.h"

namespace NSQTOOL
{
    class CNsqFactory:public CFactory
    {
//    friend class CSingleton<CNsqFactory>;

//    private:
    public:
        CNsqFactory()
        {
        
        }

    public:
        CThread *GenThread(int iThreadType, int iThreadId);
        CProtocol *GenProtocol(int iCmdType, int iCmdId);
        CHandler *GenHandler(int iCmdType, int iCmdId, int iHandlerId, CThread *pThread);
    };

//    typedef CSingleton<CNsqFactory> CSingletonNsqFactory;
};
#endif
