#ifndef __NSQD_PROTOCOL_H_
#define __NSQD_PROTOCOL_H_
#include <string>
#include <arpa/inet.h>
#include "protocol.h"
#include "json/json.h" 
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <vector>

using namespace std;
namespace NSQTOOL
{
	class CNsqdRequest
	{
	public:
		void Megic(const char *chMegic = "  V2")
		{
			m_strBuff += chMegic;	
		}

		void Indetify(const string  &strIndetifyJson, int len)
		{
			m_strBuff += "IDENTIFY\n";
			int32_t nLen = htonl(len);
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

		void PubLish(const string &strTopic, const string &strMsgData, int32_t len)
		{
			m_strBuff += "PUB ";
			m_strBuff += strTopic;
			m_strBuff += "\n";
			int32_t nLen = htonl(len);
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

	private:
		string m_strBuff;
	};

	class CNsqdResponse:public CProtocol 
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

		void Decode()
		{
            const char *buf = m_strStream.c_str();
			m_iTotalLen = ntohl(*(int32_t*)buf);	
			m_iFrameType = ntohl(*(int32_t*)(buf+4));


			if (m_iFrameType == FrameTypeMessage)
			{
				m_iTimestamp = (int64_t)ntoh64((uint8_t*)(buf+8));
				m_iAttempts = ntohs(*(uint16_t *)(buf+16));
				m_strMsgId = string(buf+18, 16);
				m_strMsgBody = string(buf+34, m_iTotalLen - 34 + 4);
			}
			else if (m_iFrameType == FrameTypeResponse)
			{
				m_strResponse = string(buf+8, m_iTotalLen - 4);	
				
			}
			else if (m_iFrameType == FrameTypeError)
			{
				return ;
			}

			return ;
		}

		int32_t Encode(char *buf, int32_t &buflen)	
		{
			//暂时不需要	
			return 0;
		}

		int32_t Need(const char *pData, int32_t iLength);
	public:

		int32_t GetFrameType()
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

		string &GetMsgBody()
		{
			return m_strMsgBody;	
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
		int32_t	m_iTotalLen;
		int32_t	m_iFrameType;
		int64_t		m_iTimestamp;
		uint16_t	m_iAttempts;
		string		m_strMsgId;
		string		m_strMsgBody;
        string      m_strResponse;
	};
};
#endif
