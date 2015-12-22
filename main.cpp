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
int g_iListenType = 1;
int g_iRecvType = 2;

void ConsumerCallBack(int iProtocolId, const string &strMsgId, const string &strMsgBody)
{
    printf("iProtocolId = %d, msg body = %s\n", iProtocolId, strMsgBody.c_str());
}

int main()
{
    CMainThread::InitSuperServer();
    CMainThread::SetConsumer("10.10.159.130", 4161, "Login", "test", ConsumerCallBack);
    CMainThread::StartSuperServer();
    while(1)
    sleep(100);
}

