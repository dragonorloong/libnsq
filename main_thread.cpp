#include "main_thread.h"
#include <unistd.h>

namespace NSQTOOL
{
    CMainThread::CMainThread(int32_t iThreadType, int32_t iThreadId)
        : CThread(iThreadType, iThreadId)
    {
        daemon(0, 0);  
    }
};
