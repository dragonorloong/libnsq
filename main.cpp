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
using namespace std;
using namespace NSQTOOL;
int32_t g_iListenType = 1;
int32_t g_iRecvType = 2;

int main()
{
    CThreadPool<CListenThread> *pListenThreadPool = new CThreadPool<CListenThread>();
    pListenThreadPool->Init(g_iListenType, 1, NULL);
    CThreadMgrSingleton::GetInstance()->RegisterThreadPool(pListenThreadPool);
    CThreadPool<CNetThread> *pNetThreadPool = new CThreadPool<CNetThread>();
    pNetThreadPool->Init(g_iRecvType, 1, NULL);
    CThreadMgrSingleton::GetInstance()->RegisterThreadPool(pNetThreadPool);
    pListenThreadPool->Run();
    pNetThreadPool->Run();
    CCommand cmd(NET_CONNECT_TYPE);
    CNetThread::SNetContext *pNetContext = new CNetThread::SNetContext;
    pNetContext->m_strHost = "127.0.0.1";
    pNetContext->m_iPort = 4161;
    pNetContext->m_iPkgType = 3;
    pNetContext->m_cAddr.m_iDstType = 2;
    pNetContext->m_cAddr.m_iDstTid = 0;
    cmd.SetLData(pNetContext);
    CCommand::CCmdAddr cCmdAddr;
    cCmdAddr.m_iDstTid = 0;
    cCmdAddr.m_iDstType = 2;
    cmd.SetAddr(cCmdAddr);
    CThreadMgrSingleton::GetInstance()->PostCmd(g_iRecvType, cmd, 0);
    sleep(100);
}

