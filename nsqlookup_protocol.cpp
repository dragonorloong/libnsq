#include "nsqlookup_protocol.h"

namespace NSQTOOL
{

/*int32_t CNsqLookupResponse::Process(CNetThread::SNetContext *pContext, CNetThread *pThread)
{
    fprintf(stdout, "CNsqLookupResponse:Process\n");
	DecodeResponseBody();
}

void CNsqLookupResponse::OnConnect(CNetThread::SNetContext *pContext, CNetThread *pThread)
{
    fprintf(stdout, "CNsqLookupResponse:OnConnect\n");
    CHttpRequest cHttpRequest;
    cHttpRequest.SetRequest("/lookup?topic=lhb", "127.0.0.1:4161");
    fprintf(stdout, "senddata = %s\n", cHttpRequest.Encode().c_str());
    pThread->SendData(pContext->m_iHandle, &cHttpRequest.Encode());
}

void CNsqLookupResponse::OnError(CNetThread::SNetContext *pContext, CNetThread *pThread, short iEvent)
{
    fprintf(stdout, "CNsqLookupResponse:OnError, iEvent = %d\n", iEvent);
}
*/

void CNsqLookupResponse::Decode()
{
    string &strBodyTemp = CHttpResponse::GetBody();
    const char *chBody = strBodyTemp.c_str();
    Json::Reader reader;
    Json::Value root;

    
    if (!reader.parse(chBody, root))
    {
        fprintf(stdout, "parse root failed!\n");
        return ;
    }

    m_strStatus = root["status_code"].asString();
    m_strStatusTxt = root["status_txt"].asString();

    if (m_strStatus != "200")
    {
        fprintf(stdout, "response return failed errorinfo is:%s\n" ,m_strStatusTxt.c_str());
        return ;
    }

    m_vecChannels.clear();
    m_vecProducers.clear();

    const Json::Value data = root["data"];
    const Json::Value channel = data["channels"];

    fprintf(stdout, "channel size = %d\n" ,  channel.size());

    for (size_t i = 0; i < channel.size(); ++i)
    {
        m_vecChannels.push_back(channel[int(i)].asString());	
    }

    const Json::Value producers = data["producers"];
    fprintf(stdout, "producers size = %d\n", producers.size());

    for (size_t i = 0; i < producers.size(); ++i)
    {
        SProducers item;
        item.m_strBroadcastAddres = producers[int(i)]["broadcast_address"].asString();
        item.m_strHostName = producers[int(i)]["hostname"].asString();
        item.m_iTcpPort = producers[int(i)]["tcp_port"].asInt();
        item.m_iHttpPort = producers[int(i)]["http_port"].asInt();
        item.m_strVersion = producers[int(i)]["version"].asString();

        fprintf(stdout, "i = %ld, boardAddres = %s, hostName = %s, tcpPort = %d, httpPort = %d, version = %s\n", 
               i, item.m_strBroadcastAddres.c_str(), item.m_strHostName.c_str(), item.m_iTcpPort, item.m_iHttpPort, item.m_strVersion.c_str());

        m_vecProducers.push_back(item);
    }

    return ;
}

};

