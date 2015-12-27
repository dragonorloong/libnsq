#include "factory.h"
#include "thread.h"
#include "net_thread.h"
#include "timer_thread.h"
#include "main_thread.h"
#include "nsqd_protocol.h"
#include "nsqlookup_protocol.h"
#include "handler.h"
#include "tcp_handler.h"
#include "listen_handler.h"
#include "nsqd_handler.h"
#include "nsqlookup_handler.h"

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
                        iHandlerId, pThread);
            }
            case NSQD_TYPE:
            {
                return new CNsqdHandler(iCmdType, iCmdId, 
                        iHandlerId, pThread);
            }
            default:
            {
                return NULL;
            }
        }
    }

};
