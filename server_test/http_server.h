/*************************************************************************
	> File Name: nsq_thread.h
	> Author: 
	> Mail: 
	> Created Time: 2015年10月25日 星期日 22时41分15秒
 ************************************************************************/

#ifndef _HTTP_SERVER_H
#define _HTTP_SERVER_H
#include "main_thread.h"
#include "handler.h"
#include "tcp_handler.h"
#include "guard.h"

namespace NSQTOOL
{
    class CHttpServer
    {
    public:
        static void InitSuperServer(int iThreadNum, 
                             int iLogLevel = LOG_DEBUG, 
                             LOGCALLBACK pLogFunc = NULL
                             );

        static void StartSuperServer();
        static void StopSuperServer();
    };
};

#endif
