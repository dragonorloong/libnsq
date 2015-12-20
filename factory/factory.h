#ifndef _FACTORY_
#define _FACTORY_

namespace NSQTOOL
{
    class CFactory
    {
    public:
        Thread *GenThread(int iThreadType, int iThreadId) = 0;
        CProtocol *GenProtocol(int iProtocolType) = 0;
        CHandler *GenHandler(int iProtocolType, int iProtocolId, int iHandlerId, CThread *pThread)=0;
    };

    class CNsqFactory:public CFactory
    {
    friend CSingleton<CNsqFactory>;

    private:
        CNsqFactory()
        {
        
        }

    public:
        Thread *GenThread(int iThreadType, int iThreadId);
        CProtocol *GenProtocol(int iProtocolType);
        CHandler *GenHandler(int iProtocolType, int iProtocolId, int iHandlerId, CThread *pThread);
    };

    typedef CSingleton<CNsqFactory> CSingletonNsqFactory;
};
#endif
