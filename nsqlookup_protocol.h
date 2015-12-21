#ifndef __NSQLOOKUP_PROTOCOL_H_
#define __NSQLOOKUP_PROTOCOL_H_
#include <string>
#include <arpa/inet.h>
#include "protocol.h"
#include "json/json.h" 
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <vector>
#include "http_protocol.h"

using namespace std;
namespace NSQTOOL
{
	// 每个 Record 只能是一种类型：请求包或者应答包
	class CNsqLookupResponse:public CHttpResponse
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

		void Decode();
		
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
	};
};
#endif
