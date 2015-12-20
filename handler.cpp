#include "thread.h"
#include "handler.h"

namespace NSQTOOL
{
    CHandler::CHandler(uint64_t iHandlerId, CThread *pThread):
        m_iHandlerId(iHandlerId), m_pThread(pThread)
    {
         
    }

    CHandler::~CHandler()
    {
    }

    uint64_t CHandler::GetHandlerId()
    {
        return m_iHandlerId; 
    }

    CThread *CHandler::GetThread()
    {
        return m_pThread; 
    }

    void CHandler::SetHandlerId(uint64_t iHandlerId)
    {
        m_iHandlerId = iHandlerId; 
    }

    void CHandler::SetThread(CThread *pThread)
    {
        m_pThread = pThread; 
    }
};
