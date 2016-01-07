#ifndef _COMMAND_H_
#define _COMMAND_H_
#include <stdint.h>
#include <pthread.h>
#include <string>
#include "event2/event.h" 

using namespace std;
namespace NSQTOOL
{
    void NsqLogPrintf(int iLogLevel, const char *pFormat, ...);

	enum EInternalCmdType
	{
		STOP_TYPE = -1,
		TCP_CONNECT_TYPE = -2,
		TCP_LISTEN_ADD_TYPE = -3,
        TCP_LISTEN_DEL_TYPE = -4,
        TCP_LISTEN_ACCEPT_TYPE = -11,
		TCP_DEL_TYPE = -5,
		TCP_ADD_TYPE = -6,
		TCP_SEND_TYPE = -7,
        TCP_READ_TYPE = -8, 
        TIMER_ADD_TYPE = -9,
        TIMER_DEL_TYPE = -10
	};

    enum EInternalThreadType
    {
        NET_THREAD_TYPE = -1,
        LISTEN_THREAD_TYPE = -2,
        TIMER_THREAD_TYPE = -3,
        NSQ_THREAD_TYPE = -4,
    };

    enum ENsqProtocolType
    {
        NSQLOOKUP_TYPE = -1,
        NSQD_TYPE = -2,
    };

    enum LOG_LEVEL
    {
        LOG_DEBUG = -3,
        LOG_ERROR = -2
    };


    struct CAddr
    {
        int32_t m_iThreadType;
        int32_t m_iThreadId;
        int32_t m_iHandlerId;

        CAddr()
        {
           m_iThreadType = -1;  
           m_iThreadId = -1;
           m_iHandlerId = -1;
        }
    };

    struct CCmdAddr
    {
        CAddr m_cSrcAddr;
        CAddr m_cDstAddr;
    };

    class CThread;

	class CCommand
	{
	public:
		CCommand(int32_t iCmdType, int32_t iCmdId);
		//不负责释放
		virtual ~CCommand();
		void SetCmdType(int32_t iCmdType);
        void SetCmdId(int32_t iCmdId);
		void SetLData(void *pData);
		void SetRData(void *pData);
		void *GetLData();
		void *GetRData();
		int32_t GetCmdType();
        int32_t GetCmdId();
		CCmdAddr &GetAddr();
		void SetAddr(CCmdAddr &cCmdAddr);
	private:
		int32_t m_iCmdType;
        int32_t m_iCmdId;
		void *m_pLData;
		void *m_pRData;
		CCmdAddr m_cAddr;
	}; 

    class CTcpConnectCommand:public CCommand
    {
    public: 
       CTcpConnectCommand(const string &strHost, uint16_t iPort, int32_t iCmdId)
           : CCommand(TCP_CONNECT_TYPE, iCmdId)
           , m_strHost(strHost), m_iPort(iPort) 
       {

       }

    public:
       string m_strHost;
       uint16_t m_iPort;
    };

    class CTcpAddCommand:public CCommand
    {
    public:
        CTcpAddCommand(int32_t iFd, int32_t iCmdId)
            :CCommand(TCP_ADD_TYPE, iCmdId), m_iFd(iFd)
        {
        
        }

    public:
        int32_t m_iFd;
    };

    class CTcpSendCommand:public CCommand
    {
    public:
        CTcpSendCommand(const char *pData, int32_t iLength)
            : CCommand(TCP_SEND_TYPE, -1)
            , m_pSendData(pData), m_iLength(iLength)
        {
             
        }

    public:
        const char *m_pSendData;
        int32_t m_iLength;
    };

    class CTcpReadCommand:public CCommand
    {
    public:         
        CTcpReadCommand():CCommand(TCP_READ_TYPE, -1)
        {
        
        }
    };

    class CTcpDelCommand:public CCommand
    {
    public:
        CTcpDelCommand(short iErrorType)
            : CCommand(TCP_DEL_TYPE, -1)
            , m_iErrorType(iErrorType)
        {
        
        }

    public:
        short m_iErrorType;
    };

    class CTcpListenAddCommand:public CCommand
    {
    public: 
       CTcpListenAddCommand(const string &strHost, uint16_t iPort,
               int32_t iCmdId):CCommand(TCP_LISTEN_ADD_TYPE, iCmdId)
               , m_strHost(strHost), m_iPort(iPort)
        {
         
        }

    public:
       string m_strHost;
       uint16_t m_iPort;
    };

    class CTcpListenDelCommand:public CCommand
    {
    public:
        CTcpListenDelCommand(const string &strHost, uint16_t iPort)
            :CCommand(TCP_LISTEN_DEL_TYPE, -1)
            , m_strHost(strHost), m_iPort(iPort)
        {
             
        }

    public:
       string m_strHost;
       uint16_t m_iPort;
    };

    class CTcpListenAcceptCommand:public CCommand
    {
    public:
        CTcpListenAcceptCommand(int iAcceptFd)
            : CCommand(TCP_LISTEN_ACCEPT_TYPE, -1)
            , m_iAcceptFd(iAcceptFd)
        {
        
        }

    public:
        int m_iAcceptFd;

    };

    class CTimerAddCommand:public CCommand
    {
    public: 
        CTimerAddCommand(struct timeval &cTimeval, 
                int iPersist, int iTimerType)
            : CCommand(TIMER_ADD_TYPE, -1)
            , m_cTimeval(cTimeval)
            , m_iPersist(iPersist)
            , m_iTimerType(iTimerType)
        {
             
        }

    public:
        struct timeval m_cTimeval;
        int m_iPersist;
        int m_iTimerType;
        event *m_pEvent;
        CThread *m_pThread;
    };

    class CNsqLookupCommand:public CTcpConnectCommand
    {
    public:
        CNsqLookupCommand(const string &strTopic, const string &strChannel, 
                const string &strHost, uint16_t iPort)
            : CTcpConnectCommand(strHost, iPort, NSQLOOKUP_TYPE)
            , m_strTopic(strTopic), m_strChannel(strChannel)
        {
                
        }

    public:
        string m_strTopic;
        string m_strChannel;
    };

    class CNsqdCommand:public CTcpConnectCommand
    {
    public:
        CNsqdCommand(const string &strTopic, const string &strChannel, 
                const string &strHost, uint16_t iPort)
            : CTcpConnectCommand(strHost, iPort, NSQD_TYPE)
            , m_strTopic(strTopic), m_strChannel(strChannel)
        {
                
        }

    public:
        string m_strTopic;
        string m_strChannel;
    };
};
#endif
