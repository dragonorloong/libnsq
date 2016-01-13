#ifndef _HTTP_FACTORY_
#define _HTTP_FACTORY_
#include "singleton.h"
#include "factory.h"

namespace NSQTOOL
{
    enum EHttpCommand
    {
        HTTP_TEST = 1,
    };

    class CHttpFactory:public CFactory
    {
    public:
        CHttpFactory()
        {
        
        }

    public:
        CThread *GenThread(int iThreadType, int iThreadId);
        CProtocol *GenProtocol(int iCmdType, int iCmdId);
        CHandler *GenHandler(int iCmdType, int iCmdId, int iHandlerId, CThread *pThread);
    };
};
#endif
