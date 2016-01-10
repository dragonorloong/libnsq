#include "command.h"
namespace NSQTOOL
{
    CCommand::CCommand(int32_t iCmdType, int32_t iCmdId)
        :m_iCmdType(iCmdType), m_iCmdId(iCmdId), m_pLData(NULL), m_pRData(NULL)
    {
       gettimeofday(&m_cTimeBegin, NULL);  
    }

    //不负责释放
    CCommand::~CCommand()
    {

    }

    void CCommand::SetCmdType(int32_t iCmdType)
    {
        m_iCmdType = iCmdType;	
    }

    void CCommand::SetLData(void *pData)
    {
        m_pLData = pData;
    }

    void CCommand::SetRData(void *pData)
    {
        m_pRData = pData;
    }

    void *CCommand::GetLData()
    {
        return m_pLData;
    }	
    
    void *CCommand::GetRData()
    {
        return m_pRData;
    }

    int32_t CCommand::GetCmdType()
    {
        return m_iCmdType;
    }	

    int32_t CCommand::GetCmdId()
    {
        return m_iCmdId; 
    }

    CCmdAddr &CCommand::GetAddr()
    {
        return m_cAddr;
    }

    void CCommand::SetAddr(CCmdAddr &cCmdAddr)
    {
        m_cAddr = cCmdAddr;	
    }	

    int CCommand::CheckTimeout()
    {
        int64_t iProcessTimeMs = GetIntervalNow(&m_cTimeBegin);
        if (iProcessTimeMs >  g_iCmdProcessTime)
        {
            return 0; 
        }

        return  -1;
    }

};
