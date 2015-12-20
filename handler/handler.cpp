#include "thread.h"
#include "handler.h"

namespace NSQTOOL
{
    CHnadler::CHandler(uint64_t iHandlerId, CThread *pThread):
        m_iHandlerId(iHandlerId), m_pThread(pThread)
    {
         
    }

    virtual CHandler::~CHandler()
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

    void CHandler::SetHandlerId(uint64_t iHandleId)
    {
        m_iHandleId = iHandleId; 
    }

    void CHandler::SetThread(CThread *pThread)
    {
        m_pThread = pThread; 
    }
};
