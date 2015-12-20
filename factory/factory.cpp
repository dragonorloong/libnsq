#include "factory.h"
#include "net_thread.h"
#include "listen_thread.h"
#include "timer_thread.h"
#include "main_thread.h"
#include "nsqd_protocol.h"
#include "nsqlookup_protocol.h"
#include "nsqd_handler.h"
#include "nsqlookup_handler.h"

namespace NSQTOOL
{
    Thread *CNsqFactory::GenThread(int iThreadType, int iThreadId)
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

        CProtocol *CNsqFactory::GenProtocol(int iProtocolType)
        {
            switch (iProtocolType) 
            {
                case NSQLOOKUP_TYPE:
                {
                    return new CNsqLookupResponce(); 
                }
                case NSQ_TYPE:
                {
                    return new CNsqdResponce(); 
                }
                default:
                {
                    return NULL; 
                }
            }
        }

        CHandler *CNsqFactory::GenHandler(int iProtocolType, int iProtocolId, 
                int iHandlerId, CThread *pThread)
        {
            switch (iPorotocolType) 
            {
                case NSQLOOKUP_TYPE:
                {
                    return new CNsqLookupHandler(iProtocolType, iProtocolId, 
                            iHandlerId, pThread);
                }
                case NSQD_TYPE:
                {
                    return new CNsqdHandler(iProtocolType, iProtocolId, 
                            iHandlerId, pThread);
                }
            }
        }

};
