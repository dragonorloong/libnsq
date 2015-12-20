#ifndef _HANDLER_
#define _HANDLER_
#include <iostream>
#include <string>
using namespace std;
namespace NSQTOOL
{
    class CThread;
    class CHandler
    {
    public:
        CHandler(uint64_t iHandlerId, CThread *pThread):
            m_iHandlerId(iHandlerId), m_pThread(pThread);
        virtual ~CHandler();
        uint64_t GetHandlerId();
        CThread *GetThread();
        void SetHandlerId(uint64_t iHandleId);
        void SetThread(CThread *pThread);
        virtual int32_t Process() = 0;
    private:
        uint64_t m_iHandlerId;
        CThread *m_pThread;
    };
}
#endif
