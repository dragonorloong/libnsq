#include "nsqd_protocol.h"

namespace NSQTOOL
{

int32_t CNsqdResponse::Need(const char *pData, int32_t iLength)
{
    if ((iLength==0) && (m_strStream.empty()))
    {
        fprintf(stdout, "null, needLen = 4\n");
        return 4;
    }

    if (iLength != 0)
    {
	    m_strStream.append(pData, iLength);
    }


	size_t needLen=0;
	if(m_strStream.length()<4){
		needLen=4;
	}else{
		needLen = ntohl(*(int32_t*)m_strStream.c_str()) + 4;
	}

	if(needLen<=m_strStream.length()){
        m_iCurPkgLength = needLen;
		return 0;
	}

	return needLen - m_strStream.length();
}

/*int32_t CNsqdResponse::Process(CNetThread::SNetContext *pContext, CNetThread *pThread)
{
    fprintf(stdout, "CNsqdResponse::Process\n");
	if (GetFrameType() == CNsqdResponse::FrameTypeResponse)
	{
		if (GetResponce() ==  "_heartbeat_")
		{
            fprintf(stdout, "_hearbeat_\n");
			CNsqdRequest cNsqdRequest;
			cNsqdRequest.Nop();
			//PutMsgToSendList(buff, cNsqdRequest.GetBuff().size(), true);
            pThread->SendData(pContext->m_iHandle, &cNsqdRequest.Encode());
		}

        fprintf(stdout, "response = %s\n", GetResponce().c_str());
	}
	else if (GetFrameType() == CNsqdResponse::FrameTypeMessage)
	{
        std::string &strMsgId = GetMsgId();
        std::string &strBody = GetBody();
        fprintf(stdout, "msg:%s\n", strBody.c_str());
		CNsqdRequest cNsqdRequest;
		cNsqdRequest.Finish(strMsgId);
        pThread->SendData(pContext->m_iHandle, &cNsqdRequest.Encode());
    }		
}

void CNsqdResponse::OnConnect(CNetThread::SNetContext *pContext, CNetThread *pThread)
{
    fprintf(stdout, "NsqdResponse:OnConnect\n");
	CNsqdRequest cNsqdRequest;
	cNsqdRequest.Megic();	
	cNsqdRequest.Subscribe("lhb", "test");
	cNsqdRequest.Ready(100);
    fprintf(stdout, "OnConnect:m_iHandle = %d\n", pContext->m_iHandle);
    pThread->SendData(pContext->m_iHandle, &cNsqdRequest.Encode());
}

void CNsqdResponse::OnError(CNetThread::SNetContext *pContext, CNetThread *pThread, short iEvent)
{
    fprintf(stdout, "OnError, iEvent = %d\n", iEvent);
}
*/

};

