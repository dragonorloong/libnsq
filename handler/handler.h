#ifndef _TASK_
#define _TASK_
#include <iostream>
#include <string>
using namespace std;
namespace NSQTOOL
{
    class CHandler
    {
    public:
        CHandler(uint64_t iHandleId, CThread *pThread):
            m_iHandleId(iHandleId), m_pThread(pThread)
        {
             
        }

        virtual ~CHandler()
        {
            m_pThread->Destory(m_iHandleId); 
        }

        uint64_t GetHandleId()
        {
            return m_iHandleId; 
        }

        CThread *GetThread()
        {
            return m_pThread; 
        }

        virtual int32_t Process() = 0;
    private:
        uint64_t m_iHandleId;
        CThread *m_pThread;
    };

    class CTcpHandler:public CHandler
    {
    public: 
        CTcpHandler(uint64_t iHandleId, CThread *pThread, int iProtocolType, 
                    const string & strHost, uint16_t iPort)
                    :CHandler(iHandleId, pThread)
                    ,m_strHost(strHost), m_iPort(iPort)
        {}

        virtual void OnConnect()
        {
        }

        virtual void OnError(int iErrorNo)
        {
            delete this;          
        }

        virtual int OnRead(const char *pData, int iLength)
        {
            int iNeed = m_pProtocol->Need(pData, iLength); 

            if (iNeed == 0)
            {
                ProcessRead(); 
            }
        }

        virtual void ProcessRead()
        {
             
        }

        virtual void OnWrite()
        {
             
        }

    private:
        CProtocol *m_pProtocol;
        string m_strHost;
        uint16_t m_iPort;
    };

    class CListenHandler:public CHandler
    {
    public:
        CListenHandler(uint64_t iHandleId, CThread *pThread, int iProtocolType, 
                    const string & strHost, uint16_t iPort)
                    :CHandler(iHandleId, pThread)
                    ,m_strHost(strHost), m_iPort(iPort)

    }
}
#endif
