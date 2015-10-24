#ifndef _PROTOCOL_H_
#define _PROTOCOL_H_
#include <stdint.h>
#include<string>
#include "net_thread.h"

namespace NSQTOOL
{
	class CData
	{
	public:
        CData()
        {
            m_strStream = "";
        }
		virtual int32_t Need(const char *pData, int32_t iLength) = 0;
        virtual int32_t Process(CNetThread::SNetContext *pContext, CNetThread *pThread) = 0;

        virtual void OnConnect(CNetThread::SNetContext *pContext, CNetThread *pThread)
        {
            fprintf(stdout, "OnConnect\n");
        }

        virtual void OnError(CNetThread::SNetContext *pContext, CNetThread *pThread, short iEvent)
        {
            fprintf(stdout, "OnError\n");
        }

        virtual void OnAccept(CNetThread::SNetContext *pContext, CNetThread *pThread)
        {
            fprintf(stdout, "OnAccept\n");
        }

        virtual int32_t NextPkg()
        {
            fprintf(stdout, "NextPkg:curPkg = %d, strStream = %d\n", m_iCurPkgLength, m_strStream.length());
            if (m_iCurPkgLength > m_strStream.length())
            {
                m_strStream.clear();
            }
            else
            {
                m_strStream.erase(0, m_iCurPkgLength);
            }

            m_iCurPkgLength = 0;
            fprintf(stdout, "NextPkg2:curPkg = %d, strStream = %d\n", m_iCurPkgLength, m_strStream.length());
            return 0;
        }

		static CData *New(int32_t iPkgType);
    protected:
        std::string m_strStream;
        size_t m_iCurPkgLength;
	};

    class CDataTest:public CData
    {
    public:
        int32_t Need(const char *pData, int32_t iLength);
        int32_t Process(CNetThread::SNetContext *pContext, CNetThread *pThread);
        void OnConnect(CNetThread::SNetContext *pContext, CNetThread *pThread);
        void OnError(CNetThread::SNetContext *pContext, CNetThread *pThread, short iEvent);
        void OnAccept(CNetThread::SNetContext *pContext, CNetThread *pThread);
    };
};
#endif
