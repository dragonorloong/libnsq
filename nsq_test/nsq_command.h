#ifndef _NSQ_COMMAND_H_
#define _NSQ_COMMAND_H_
#include "command.h"

namespace NSQTOOL
{
    enum ENsqProtocolType
    {
        NSQLOOKUP_TYPE = -1,
        NSQD_TYPE = -2,
    };

    class CNsqLookupCommand:public CTcpConnectCommand
    {
    public:
        CNsqLookupCommand(const string &strTopic, const string &strChannel, 
                const string &strHost, uint16_t iPort)
            : CTcpConnectCommand(strHost, iPort, NSQLOOKUP_TYPE)
            , m_strTopic(strTopic), m_strChannel(strChannel)
        {
                
        }

    public:
        string m_strTopic;
        string m_strChannel;
    };

    class CNsqdCommand:public CTcpConnectCommand
    {
    public:
        CNsqdCommand(const string &strTopic, const string &strChannel, 
                const string &strHost, uint16_t iPort)
            : CTcpConnectCommand(strHost, iPort, NSQD_TYPE)
            , m_strTopic(strTopic), m_strChannel(strChannel)
        {
                
        }

    public:
        string m_strTopic;
        string m_strChannel;
    };
};
#endif
