#include "nsq_factory.h"
#include "thread.h"
#include "net_thread.h"
#include "timer_thread.h"
#include "nsq_thread.h"
#include "nsqd_protocol.h"
#include "nsqlookup_protocol.h"
#include "handler.h"
#include "tcp_handler.h"
#include "listen_handler.h"
#include "nsqd_handler.h"
#include "nsqlookup_handler.h"
#include "common.h"

namespace NSQTOOL
{
    CThread *CNsqFactory::GenThread(int iThreadType, int iThreadId)
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
            case NSQ_THREAD_TYPE:
            {
                return new CNsqThread(iThreadType, iThreadId); 
            }
            default:
            {
                return NULL; 
            }
        }
    }

    CProtocol *CNsqFactory::GenProtocol(int iCmdType, int iCmdId)
    {
        switch (iCmdId) 
        {
            case NSQLOOKUP_TYPE:
            {
                return new CNsqLookupResponse(); 
            }
            case NSQD_TYPE:
            {
                return new CNsqdResponse(); 
            }
            default:
            {
                return NULL; 
            }
        }
    }

    CHandler *CNsqFactory::GenHandler(int iCmdType, int iCmdId, 
            int iHandlerId, CThread *pThread)
    {
        switch (iCmdId) 
        {
            case NSQLOOKUP_TYPE:
            {
                return new CNsqLookupHandler(iCmdType, iCmdId, 
                        iHandlerId, pThread, ESHORTCONNECT);
            }
            case NSQD_TYPE:
            {
                return new CNsqdHandler(iCmdType, iCmdId, 
                        iHandlerId, pThread, ELONGCONNECT);
            }
            default:
            {
                return NULL;
            }
        }
    }

//    extern CFactory *g_pFactory;
 //   CNsqFactory g_cNsqFactory;
  //  g_pFactory = &g_cNsqFactory;
};
