/*************************************************************************
	> File Name: nsq_thread.h
	> Author: 
	> Mail: 
	> Created Time: 2015年10月25日 星期日 22时41分15秒
 ************************************************************************/

#ifndef _NSQ_THREAD_H
#define _NSQ_THREAD_H
#include "thread.h"
#include "handler.h"
#include "tcp_handler.h"
#include "nsqlookup_protocol.h"
#include "guard.h"

namespace NSQTOOL
{
    struct CHandlerContext
    {
        CAddr m_cAddr; 
        string m_strNsqdHost;
        uint16_t m_iNsqdPort;
        string m_strTopic;
        string m_strChannel;
    };

    typedef void (*BIZCALLBACK)(int, const string&, const string&);


    typedef void (*LOGCALLBACK)(int, const char *pLogMsg);

    struct CNsqLookupContext
    {
        string m_strLookupHost;
        uint16_t m_iLookupPort;
        string m_strTopic;
        string m_strChannel;
        BIZCALLBACK m_funcCallBack;
    };

    void NsqLogPrintf(int iLogLevel, const char *pFormat, ...);

    class CNsqThread:public CThread
    {
    public:
        enum MAINTHREADTIMER
        {
            LOOKUP_TIMER = 1,
        };

        CNsqThread(int iThreadType, int iThreadId)
           : CThread(iThreadType, iThreadId)
        {
        
        }

        void RealProcessCmd(CCommand *pCmd); 

        static void InitSuperServer(int iThreadNum, 
                             int iConnectNum, 
                             int iLogLevel = LOG_DEBUG, 
                             LOGCALLBACK pLogFunc = NULL
                             );

        static void StartSuperServer();
        static void StopSuperServer();

        static void SetProducer(const string &strLookupHost,
                                uint16_t iLookupPort, 
                                const string &strTopic, 
                                BIZCALLBACK pFunc
                                );

        static void SetConsumer(const string &strLookupHost, 
                                uint16_t iLookupPort,
                                const string &strTopic, 
                                const string &strChannel,
                                BIZCALLBACK pFunc
                                );

        static void LookupReadCallBack(const string &strTopic, const string &strChannel,
                        const vector<string> &vecChannels, const vector<CNsqLookupResponse::SProducers> &vecProducers);

        static void NewNsqdConnect(const string &strTopic, const string &strChannel, 
                            const string &strHost, uint16_t iPort);

        static void NsqdConnectCallBack(
            CAddr &cAddr, const string &strNsqdHost
            , const uint16_t iPort, const string &strTopic
            , const string &strChannel);

        static void NsqdReadCallBack(const string &strTopic, 
                                       const string &strChannel,
                                       const string &strMsgId, 
                                       const string &strMsgBody);

        static void NsqdErrorCallBack(CAddr &cAddr
            , const string &strTopic
            , const string &strChannel);
        static int ProducerMsg(const string &strTopic, const string &strMsg);
        static void OnTimeOut();
    private:
        static map<string, vector<CHandlerContext> > m_mapTopic2Handler;
        static map<string, CNsqLookupContext> m_mapLookupContext;
        static int m_iConnectNum;
        static int m_iThreadNum;
        static CLock m_cLock;
    public:
        static LOGCALLBACK m_pLogFunc;
        static int m_iLogLevel;
    };
};

#endif
