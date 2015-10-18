#ifndef __NSQ_PROTOCOL_H_
#define __NSQ_PROTOCOL_H_
#include <string>
#include <arpa/inet.h>
#include "protocol.h"
//#include "json/json.h" 
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <vector>

using namespace std;
namespace NSQTOOL
{
	class CNsqdRequest:public CData
	{
	public:
		void Megic(const char *chMegic = "  V2")
		{
			m_strBuff += chMegic;	
		}

		void Indetify(const string  &strIndetifyJson, int len)
		{
			m_strBuff += "IDENTIFY\n";
			uint32_t nLen = htonl(len);
			char buff[4] = {0};
			memcpy(buff, &nLen, 4);
			m_strBuff += string(buff, 4);
			m_strBuff += strIndetifyJson;
		}

		void Subscribe(const string &strTopic, const string &strChannel)
		{
			m_strBuff += "SUB ";	
			m_strBuff += strTopic;
			m_strBuff += " ";
			m_strBuff += strChannel;
			m_strBuff += "\n"; 
		}

		void PubLish(const string &strTopic, const string &strMsgData, uint32_t len)
		{
			m_strBuff += "PUB ";
			m_strBuff += strTopic;
			m_strBuff += "\n";
			uint32_t nLen = htonl(len);
			char buff[4] = {0};
			memcpy(buff, &nLen, 4);
			m_strBuff += string(buff, 4);
			m_strBuff += strMsgData;
		}

		void Ready(int32_t iCount)
		{
			m_strBuff += "RDY ";	
			char buff[1024];
			snprintf(buff, sizeof(buff), "%d\n", iCount); 
			m_strBuff += buff; 
		}

		void Finish(const string &strMsgId)
		{
			m_strBuff += "FIN ";	
			m_strBuff += strMsgId;
			m_strBuff += "\n";
		}

		void Requeue(const string &strMsgId, int iTimeOut)
		{
			m_strBuff += "REQ ";	
			m_strBuff += strMsgId;
			char buff[50];
			snprintf(buff, sizeof(buff), "%d", iTimeOut);
			m_strBuff += " ";
			m_strBuff += buff;
			m_strBuff += "\n";
		}

		void Touch(const string &strMsgId)
		{
			m_strBuff += "TOUCH ";
			m_strBuff += strMsgId;
			m_strBuff += "\n";
		}

		void Cls()
		{
			m_strBuff += "CLS\n";	
		}

		void Nop()
		{
			m_strBuff += "NOP\n";
		}

		void Clear()
		{
			m_strBuff.clear();	
		}

	public:
		const string &Encode()
		{
			return m_strBuff;	
		}

		int32_t Decode()
		{
			return 0;
		}
		
		int32_t Need(const char *pData, int32_t iLength)
		{
			return -1;
		}

		int32_t Process()
		{
			return -1;
		}

	private:
		string m_strBuff;
	};

	class CNsqdResponse:public CData
	{
	public:	
		enum {FrameTypeResponse, FrameTypeError, FrameTypeMessage};

		uint64_t ntoh64(const uint8_t *data) 
		{
			return 	(uint64_t)(data[7]) | (uint64_t)(data[6])<<8 |
				(uint64_t)(data[5])<<16 | (uint64_t)(data[4])<<24 |
				(uint64_t)(data[3])<<32 | (uint64_t)(data[2])<<40 |
				(uint64_t)(data[1])<<48 | (uint64_t)(data[0])<<56;
		}

		int32_t Decode(const char *buf, int32_t buflen)
		{
			m_iTotalLen = ntohl(*(uint32_t*)buf);	
			m_iFrameType = ntohl(*(uint32_t*)(buf+4));


			if (m_iFrameType == FrameTypeMessage)
			{
				m_iTimestamp = (int64_t)ntoh64((uint8_t*)(buf+8));
				m_iAttempts = ntohs(*(uint16_t *)(buf+16));
				m_strMsgId = string(buf+18, 16);
				m_strBody = string(buf+34, m_iTotalLen - 34 + 4);
			}
			else if (m_iFrameType == FrameTypeResponse)
			{
				m_strResponse = string(buf+8, m_iTotalLen - 4);	
				
			}
			else if (m_iFrameType == FrameTypeError)
			{
				return -1;
			}

			return 0;
		}

		int32_t Encode(char *buf, int32_t &buflen)	
		{
			//暂时不需要	
			return 0;
		}

		int32_t Need(const char *pData, int32_t iLength);
		int32_t Process();
	public:

		uint32_t GetFrameType()
		{
			return m_iFrameType;	
		}
		
		int64_t GetTimestamp()
		{
			return m_iTimestamp;	
		}

		uint16_t GetAttempts()
		{
			return m_iAttempts;	
		}

		string &GetMsgId()
		{
			return m_strMsgId;	
		}

		string &GetBody()
		{
			return m_strBody;	
		}

		string &GetResponce()
		{
			return m_strResponse;	
		}

		int32_t Size()
		{
			return m_iTotalLen;		
		}
			
	private:
		uint32_t	m_iTotalLen;
		uint32_t	m_iFrameType;
		int64_t		m_iTimestamp;
		uint16_t	m_iAttempts;
		string		m_strMsgId;
		string		m_strBody;

		string		m_strResponse;
	};

	// 每个 Record 只能是一种类型：请求包或者应答包
	class CNsqLookupMsg:public CData
	{
	public:
		struct SProducers
		{
			string m_strBroadcastAddres;	
			string m_strHostName;
			int m_iTcpPort;
			int m_iHttpPort;
			string m_strVersion;
		};

		CNsqLookupMsg():m_type(-1){}
		int32_t Need(const char *pData, int32_t iLength);
		int32_t Process();

		int32_t Size()
		{
		/*	if(m_type==0)
			{
				return m_httpMsgResponse.Size();
			}
			else
			{
				return m_httpMsgRequest.Size();
			}
		*/
		}

		int32_t Encode(char *buf, int32_t &buflen)
		{
		/*	if(NULL != buf)
			{
				if(m_type==0)
				{
					return m_httpMsgResponse.Encode(buf, buflen);
				}
				else
				{
					return m_httpMsgRequest.Encode(buf, buflen);
				}
			}
		*/
			return -1;
		}

		int32_t Decode(const char *buf, int32_t buflen)
		{
		/*
			if(NULL != buf)
			{
				if(strncmp("HTTP",buf,4)==0)
				{
					fprintf(stdout,"%s len=%d\n",buf,buflen);
					m_type=0;

					if (m_httpMsgResponse.Decode(buf, buflen) == 0)
					{
						return DecodeResponseBody();
					}

					return -1;
				}
				else
				{
					fprintf(stdout,"%s len=%d\n",buf,buflen);
					m_type=1;
					return m_httpMsgRequest.Decode(buf, buflen);
				}
			}
		*/
			return -1;
		}

		int32_t DecodeResponseBody()
		{
		/*	const char *chBody = m_httpMsgResponse.GetBody().c_str();
			Json::Reader reader;
			Json::Value root;

			
			if (!reader.parse(chBody, root))
			{
				LOG4CPLUS_DEBUG(logger, "parse root failed!");
				return -1;
			}

			m_strStatus = root["status_code"].asString();
			m_strStatusTxt = root["status_txt"].asString();

			if (m_strStatus != "200")
			{
				LOG4CPLUS_DEBUG(logger, "response return failed errorinfo is:" << m_strStatusTxt);
				return 0;
			}

			const Json::Value data = root["data"];
			const Json::Value channel = data["channels"];

			LOG4CPLUS_DEBUG(logger, "channel size = " << channel.size());

			for (size_t i = 0; i < channel.size(); ++i)
			{
				LOG4CPLUS_DEBUG(logger, "index = " << i << " channel = " << channel[int(i)].asString());
				m_vecChannels.push_back(channel[int(i)].asString());	
			}

			const Json::Value producers = data["producers"];
			LOG4CPLUS_DEBUG(logger, "producers size = " << producers.size());

			for (size_t i = 0; i < producers.size(); ++i)
			{
				SProducers item;
				item.m_strBroadcastAddres = producers[int(i)]["broadcast_address"].asString();
				item.m_strHostName = producers[int(i)]["hostname"].asString();
				item.m_iTcpPort = producers[int(i)]["tcp_port"].asInt();
				item.m_iHttpPort = producers[int(i)]["http_port"].asInt();
				item.m_strVersion = producers[int(i)]["version"].asString();


				LOG4CPLUS_DEBUG(logger, "broadcastAddres =  " << item.m_strBroadcastAddres << " host = " 
							<< item.m_strHostName << " tcpPort = " << item.m_iTcpPort 
							<< " httpPort = " << item.m_iHttpPort << " version = " << item.m_strVersion) ;
				m_vecProducers.push_back(item);
			}

		*/
			return 0;
		}
		
		/*CHttpMsgRequest &GetHttpRequest()
		{
			m_type=1;
		//	return m_httpMsgRequest;
		}

		CHttpMsgResponse &GetHttpResponse()
		{
			m_type=0;
		//	return m_httpMsgResponse;
		}
		*/
		// 0-应答包;1-请求包;-1-未初始化
		int32_t GetMsgType()
		{
			return m_type;
		}

		string &GetStatus()
		{
			return m_strStatus;	
		}

		string &GetStatusTxt()
		{
			return m_strStatusTxt;	
		}

		vector<string> &GetChannels()
		{
			return m_vecChannels;	
		}

		vector<SProducers> &GetProducers()
		{
			return m_vecProducers;	
		}

	private:
		string m_strStatus;
		string m_strStatusTxt;
		vector<string> m_vecChannels;
		vector<SProducers> m_vecProducers;
		// 1 request, 0 response
		int32_t          m_type;
	};
};
#endif
