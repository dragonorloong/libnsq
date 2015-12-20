#include "protocol.h"

namespace NSQTOOL
{
    int32_t CProtocol::NextPkg()
    {
        if (m_iCurPkgLength > m_strStream.length())
        {
            m_strStream.clear();
        }
        else
        {
            m_strStream.erase(0, m_iCurPkgLength);
        }

        m_iCurPkgLength = 0;
        return 0;
    }
};
