#include "protocol.h"
#include <stdio.h>

namespace NSQTOOL
{
	CData *CData::New(int32_t iPkgType)
	{
        switch(iPkgType)
        {
            case 1:
                return new CDataTest();
        }
		return NULL;
	}

    int32_t CDataTest::Need(const char *pData, int32_t iLength)
    {
        m_strStream += pData;
        return 0;
    }

    int32_t CDataTest::Process()
    {
        fprintf(stdout, "recv = %s\n", m_strStream.c_str());
        m_strStream.clear();
        return 0;
    }
};
