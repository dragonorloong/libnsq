#include "http_factory.h"
#include "thread.h"
#include "net_thread.h"
#include "timer_thread.h"
#include "handler.h"
#include "tcp_handler.h"
#include "listen_handler.h"
#include "http_handler.h"
#include "common.h"
#include "http_protocol.h"

namespace NSQTOOL
{
    CThread *CHttpFactory::GenThread(int iThreadType, int iThreadId)
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
            default:
            {
                return NULL; 
            }
        }
    }

    CProtocol *CHttpFactory::GenProtocol(int iCmdType, int iCmdId)
    {
        switch (iCmdId) 
        {
            case HTTP_TEST:
            {
                return new CHttpRequest(); 
            }
            default:
            {
                return NULL; 
            }
        }
    }

    CHandler *CHttpFactory::GenHandler(int iCmdType, int iCmdId, 
            int iHandlerId, CThread *pThread)
    {
        switch (iCmdType) 
        {
            case TCP_LISTEN_ADD_TYPE:
            {
                return new CListenHandler(iCmdType, iCmdId, 
                        iHandlerId, pThread);
                    
            }
            case TCP_ADD_TYPE:
            {
                return new CHttpHandler(iCmdType, iCmdId, 
                        iHandlerId, pThread, ESHORTCONNECT);
            }
            default:
            {
                return NULL;
            }
        }
    }
};
