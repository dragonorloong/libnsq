/*************************************************************************
	> File Name: nsq_config.h
	> Author: 
	> Mail: 
	> Created Time: 2015年11月01日 星期日 09时36分20秒
 ************************************************************************/

#ifndef _NSQ_CONFIG_H
#define _NSQ_CONFIG_H
namespace NSQTOOL
{
    class CNsqCfg
    {
    public:
        struct SProducers
        {
            string m_strBroadcastAddres;	
            string m_strHostName;
            int m_iTcpPort;
            int m_iHttpPort;
            string m_strVersion;
            int m_iConnectSize;
        };

        struct SNsqlookupInfo
        {
            string m_strHost;    
            int m_iPort;
            vector<string> m_iChannel;
            string m_strTopic;
        };

        void SetNsqlookupInfo(const SNsqlookupInfo & sNsqlookupInfo)
        {
           m_vecNsqlookupInfo.push_back(SNsqlookupInfo); 
        }
        
        vector<SNsqlookupConnectInfo> GetNsqlookupInfo()
        {
            return m_vecNsqlookupInfo;
        }

        string GetTopic(string strHost, int iPort)
        {
            vector<SNsqlookupConnectInfo>::iterator iter =  m_vecNsqlookupInfo.begin();

            for (; iter != m_vecNsqlookupInfo.end(); ++iter)
            {
                if (iter->m_strHost == strHost && iPort == iter->m_iPort)
                {
                    return m_strTopic;
                }
            }

            return string();
        }

        void SetMaxConnect(int iMaxSize)
        {
            m_iMaxSie = iMaxSize;
        }

        int GetProducers(const string &strTopic, const string &strChannel, SProducers &sProducers)
        {
            vector<SProducers>::iterator iter = m_mapProducers[strTopic][strChannel].begin();

            for (; iter != m_mapProducers[strTopic][strChannel].end(); ++iter)
            {
               if (iter->m_iConnectSize < m_iMaxSize) 
                {
                    iter->iConnectSize ++;
                    sProducers = *iter;
                    return 1;
                }
            }

            return 0;
        }
        
    public:
        map<string, map<string, vertor<SProducers> > > m_mapProducers; //topic_channel_producers  
        vector<SNsqlookupConnectInfo> m_vecNsqlookupInfo;
        int m_iMaxSize;
    };
};
#endif
