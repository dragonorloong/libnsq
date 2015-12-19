#ifndef _FACTORY_
#define _FACTORY_

namespace NSQTOOL
{
    class CFactory
    {
    public:
        Thread *GenThread(int iThreadType, int iThreadId) = 0;
        CProtocol *GenProtocol(int iProtocolType) = 0;
        CHandler *GenTcpHandler(int iProtocolType, int iProtocolId, CThread *pThread, uint64_t iHandleId, const string &strHost, uint16_t iPort)=0;
        CHandler *GenListenHandler(int iProtocolType, int iProtocolId, CThread *pThread, uint64_t iHandleId, const string &strHost, uint16_t iPort)=0;
    };

    class CNsqFactory:public CFactory
    {
    friend CSingleton<CNsqFactory>;

    private:
        CNsqFactory()
        {
        
        }

    public:
        Thread *GenThread(int iThreadType, int iThreadId)
        {
            switch(iThreadType)
            {
                case NET_THREAD_TYPE:
                {
                    return new CNetThread(iThreadType, iThreadId); 
                }
                case LISTEN_THREAD_TYPE:
                {
                    return new CListenThread(iThreadType, iThreadId); 
                }
                case TIMER_THREAD_TYPE:
                {
                    return new CTimerThread(iThreadType, iThreadId); 
                }
                case MAIN_THREAD_TYPE:
                {
                    return new CMainThread(iThreadType, iThreadId); 
                }
                default:
                {
                    return NULL; 
                }
            }
        }

        CProtocol *GenProtocol(int iProtocolType)
        {
            switch (iProtocolType) 
            {
                case NSQLOOKUP_TYPE:
                {
                    return new CNsqLookupResponce(); 
                }
                case NSQ_TYPE:
                {
                    return new CNsqResponce(); 
                }
                default:
                {
                    return NULL; 
                }
            }
        }

        CHandler *GenTcpHandler(int iProtocolType, int iProtocolId, CThread *pThread, uint64_t iHandleId, const string &strHost, uint16_t iPort)
        {
            switch (iPorotocolType) 
            {
                case NSQLOOKUP_TYPE:
                {
                    return new CNsqdHandler(iHandleId, pThread, iProtocolType, iProtocolType, strHost, iPort);
                }
                case NSQD_TYPE:
                {
                    return new CNsqdHandler(iHandleId, pThread, iProtocolType, iProtocolType, strHost, iPort);
                }
            }
        }
    };

    typedef CSingleton<CNsqFactory> CSingletonNsqFactory;
};
#endif
