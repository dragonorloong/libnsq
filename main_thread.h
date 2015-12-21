/*************************************************************************
	> File Name: main_thread.h
	> Author: 
	> Mail: 
	> Created Time: 2015年10月25日 星期日 22时41分15秒
 ************************************************************************/

#ifndef _MAIN_THREAD_H
#define _MAIN_THREAD_H
#include "thread.h"
#include "handler.h"

namespace NSQTOOL
{
    struct CHandlerContext
    {
        CHandler *m_pHandler; 
        int m_iProtocolId;
        string m_strNsqdHost;
        uint16_t m_iNsqdPort;
        string m_strTopic;
        string m_strChannel;
    };

    struct CNsqLookupContext
    {
        string m_strLookupHost;
        uint16_t m_iLookupPort;
        string m_strTopic;
        string m_strChannel;
    };

    class CMainThread:public CThread
    {
    public:
        enum MAINTHREADTIMER
        {
            LOOKUP_TIMER = 1,
        };

        CMainThread(int iThreadType, int iThreadId)
           : CThread(iThreadType, iThreadId)
        {
        
        }

        void RealProcessCmd(CCommand &cCmd); 

        static void SetConnectNum(int iProducerNum = 5); //每个生产者连接个数

        static void SetProducer(const string &strLookupHost,
                                uint16_t iLookupPort, 
                                const string &strTopic 
                                );

        static void SetConsumer(const string &strLookupHost, 
                                uint16_t iLookupPort,
                                const string &strTopic, 
                                const string &strChannel 
                                );

        static CNsqLookupContext GetLookupContext(int iProtocolId);
        static void StartSuperServer();
        
    private:
        static map<string, vector<CHandlerContext> > m_mapTopic2Handler;
        static map<int, string> m_mapHandler2Topic;
        static map<int, CNsqLookupContext> m_mapLookupContext;
        static int m_iConnectNum;
        static int m_iProtocolId;
        static pthread_mutex_t m_mutex;
    };
        
};

#endif
