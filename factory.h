#ifndef _FACTORY_
#define _FACTORY_
#include "singleton.h"

namespace NSQTOOL
{
    class CThread;
    class CProtocol;
    class CHandler;
    class CFactory
    {
    public:
        virtual CThread *GenThread(int iThreadType, int iThreadId) = 0;
        virtual CProtocol *GenProtocol(int iProtocolType) = 0;
        virtual CHandler *GenHandler(int iProtocolType, int iProtocolId, 
                int iHandlerId, CThread *pThread)=0;
    };

    class CNsqFactory:public CFactory
    {
    friend CSingleton<CNsqFactory>;

    private:
        CNsqFactory()
        {
        
        }

    public:
        CThread *GenThread(int iThreadType, int iThreadId);
        CProtocol *GenProtocol(int iProtocolType);
        CHandler *GenHandler(int iProtocolType, int iProtocolId, int iHandlerId, CThread *pThread);
    };

    typedef CSingleton<CNsqFactory> CSingletonNsqFactory;
};
#endif
