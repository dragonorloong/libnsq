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
#include "nsq_thread.h"
#include <stdlib.h>
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
    //线程个数，每个nsqd连接个数，日志级别，日志回调函数
    CNsqThread::InitSuperServer(1, 2, LOG_DEBUG, NsqLogCallBack);
    //设置消费者，lookup host,lookup_port topic channel，消息回调函数 
    CNsqThread::SetConsumer("127.0.0.1", 4161, "lhb", "test", ConsumerCallBack);
    //设置生产者,参数同上
//    CNsqThread::SetProducer("127.0.0.1", 4161, "lhb", ConsumerCallBack);
    // 启动服务
    //

    CNsqThread::StartSuperServer();
//    CNsqThread::StopSuperServer();
    sleep(100);
    exit(0);
    int iCount = 0;
	while (1)
	{
        //生产消息
       int iRet = CNsqThread::ProducerMsg("lhb", "testdjfklsk");
       // sleep(10);
        //
        iCount++;

        if (iRet != 0)
            printf("ProducerMsg iRet = %d\n", iRet);

        if (iCount % 1000000 == 0)
        {
            break;
        }
	}

}

