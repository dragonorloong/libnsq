#include "nsqlookup_protocol.h"

namespace NSQTOOL
{

/*int32_t CNsqLookupResponse::Process(CNetThread::SNetContext *pContext, CNetThread *pThread)
{
	DecodeResponseBody();
}

void CNsqLookupResponse::OnConnect(CNetThread::SNetContext *pContext, CNetThread *pThread)
{
    CHttpRequest cHttpRequest;
    cHttpRequest.SetRequest("/lookup?topic=lhb", "127.0.0.1:4161");
    pThread->SendData(pContext->m_iHandle, &cHttpRequest.Encode());
}

void CNsqLookupResponse::OnError(CNetThread::SNetContext *pContext, CNetThread *pThread, short iEvent)
{
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
        return ;
    }

    m_strStatus = root["status_code"].asString();
    m_strStatusTxt = root["status_txt"].asString();

    if (m_strStatus != "200")
    {
        return ;
    }

    m_vecChannels.clear();
    m_vecProducers.clear();

    const Json::Value data = root["data"];
    const Json::Value channel = data["channels"];


    for (size_t i = 0; i < channel.size(); ++i)
    {
        m_vecChannels.push_back(channel[int(i)].asString());	
    }

    const Json::Value producers = data["producers"];

    for (size_t i = 0; i < producers.size(); ++i)
    {
        SProducers item;
        item.m_strBroadcastAddres = producers[int(i)]["broadcast_address"].asString();
        item.m_strHostName = producers[int(i)]["hostname"].asString();
        item.m_iTcpPort = producers[int(i)]["tcp_port"].asInt();
        item.m_iHttpPort = producers[int(i)]["http_port"].asInt();
        item.m_strVersion = producers[int(i)]["version"].asString();


        m_vecProducers.push_back(item);
    }

    return ;
}

};

