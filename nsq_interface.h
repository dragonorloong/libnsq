#ifndef _NSQ_INTERFACE_H_
#define _NSQ_INTERFACE_H_

class CNsqInterface
{
public:
    struct SLookupCfg
    {
        string m_strHost;
        uint16_t m_iPort;
        std::vector<string> m_vecTopic;
    };

    struct SCustomerCfg
    {
        string m_strTopic;     
        string m_strChannel;
    };

    sturct SLookupResponse
    {
        string m_strTopic;
        string m_strChannel;
        string m_strHost;
        uint16_t m_strPort;
    };


private:


        
};
#endif

