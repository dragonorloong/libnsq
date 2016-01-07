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
        virtual CProtocol *GenProtocol(int iCmdType, int iCmdId) = 0;
        virtual CHandler *GenHandler(int iCmdType, int iCmdId, 
                int iHandlerId, CThread *pThread)=0;
    };

    class CNsqFactory:public CFactory
    {
    friend class CSingleton<CNsqFactory>;

    private:
        CNsqFactory()
        {
        
        }

    public:
        CThread *GenThread(int iThreadType, int iThreadId);
        CProtocol *GenProtocol(int iCmdType, int iCmdId);
        CHandler *GenHandler(int iCmdType, int iCmdId, int iHandlerId, CThread *pThread);
    };

    typedef CSingleton<CNsqFactory> CSingletonNsqFactory;
};
#endif
