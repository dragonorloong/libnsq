#include "protocol.h"
#include <stdio.h>
#include "net_thread.h"
#include "nsqprotocol.h"

namespace NSQTOOL
{
	CData *CData::New(int32_t iPkgType)
	{
        switch(iPkgType)
        {
            case 1:
                return new CDataTest();
            case 2:
                return new CNsqdResponse();
        }
		return NULL;
	}

    int32_t CDataTest::Need(const char *pData, int32_t iLength)
    {
        m_iCurPkgLength = 10000;
        //每个包开始之前都返回包头的长度
        if (pData == NULL && m_strStream.empty())
        {
            return 4;            
        }
        else
        {
            m_strStream += pData;
        }

        return 0;
    }

    int32_t CDataTest::Process(CNetThread::SNetContext *pContext, CNetThread *pThread)
    {
        fprintf(stdout, "recv = %s\n", m_strStream.c_str());
        m_strStream.clear();
        return 0;
    }

    void CDataTest::OnConnect(CNetThread::SNetContext *pContext, CNetThread *pThread)
    {
        fprintf(stdout, "OnCreate\n");        
    }

    void CDataTest::OnError(CNetThread::SNetContext *pContext, CNetThread *pThread, short iEvent)
    {
        fprintf(stdout, "OnError\n");
    }

    void CDataTest::OnAccept(CNetThread::SNetContext *pContext, CNetThread *pThread)
    {
         fprintf(stdout, "OnAccept\n");
    }
};
