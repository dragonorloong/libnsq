/*************************************************************************
	> File Name: main.cpp
	> Author: longhongbo 
	> Mail: longhongbo@xunlei.com
	> Created Time: 2015年10月19日 星期一 22时11分49秒
 ************************************************************************/

#include<iostream>
#include <string.h>
#include "net_thread.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
using namespace std;
using namespace NSQTOOL;
int32_t g_iListenType = 1;
int32_t g_iRecvType = 2;

int main()
{
    CThreadPool<CListenThread> *pListenThreadPool = new CThreadPool<CListenThread>();
    pListenThreadPool->Init(g_iListenType, 1, (int32_t *)g_iListenType);
    CThreadMgrSingleton::GetInstance()->RegisterThreadPool(pListenThreadPool);
    CThreadPool<CNetThread> *pNetThreadPool = new CThreadPool<CNetThread>();
    pNetThreadPool->Init(g_iRecvType, 1, (int32_t *)g_iRecvType);
    CThreadMgrSingleton::GetInstance()->RegisterThreadPool(pNetThreadPool);
    pListenThreadPool->Run();
    pNetThreadPool->Run();
    CCommand cmd(NET_LISTEN_TYPE);
    sockaddr_in *addr = new sockaddr_in();
    memset(addr, 0, sizeof(sockaddr_in));
    addr->sin_addr.s_addr = inet_addr("0.0.0.0");
    addr->sin_port = htons(8888);
    addr->sin_family = AF_INET;
    cmd.SetLData(addr);
    cmd.SetRData((int32_t*)1);
    CCommand::CCmdAddr cCmdAddr;
    cCmdAddr.m_iDstTid = 0;
    cCmdAddr.m_iDstThreadType = 2;
    cmd.SetAddr(cCmdAddr);
    CThreadMgrSingleton::GetInstance()->SendCmd(g_iListenType, cmd, 0);
    sleep(100);
}

