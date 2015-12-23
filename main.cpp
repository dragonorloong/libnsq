/*************************************************************************
	> File Name: main.cpp
	> Author: EastQ 
	> Mail: log2013@163.com 
	> Created Time: 2015年10月19日 星期一 22时11分49秒
 ************************************************************************/

#include<iostream>
#include <string.h>
#include "net_thread.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "main_thread.h"
using namespace std;
using namespace NSQTOOL;

void ConsumerCallBack(int iProtocolId, const string &strMsgId, const string &strMsgBody)
{
    printf("iProtocolId = %d, msg body = %s\n", iProtocolId, strMsgBody.c_str());
}

void NsqLogCallBack(int iLogLevel, const char *pLogMsg)
{
    printf("%s\n", pLogMsg);
}

int main()
{
    CMainThread::InitSuperServer(1, 1, LOG_DEBUG, NsqLogCallBack);
    CMainThread::SetConsumer("10.10.159.130", 4161, "Login", "test", ConsumerCallBack);
   // CMainThread::SetProducer("10.10.159.130", 4161, "Login", ConsumerCallBack);
    CMainThread::StartSuperServer();

    //sleep(40);
    //int iRet = CMainThread::ProducerMsg("Login", "test");

    //printf("ProducerMsg iRet = %d\n", iRet);

    while(1)
    sleep(100);
}

