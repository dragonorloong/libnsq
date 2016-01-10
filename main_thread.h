#ifndef _MAIN_THREAD__
#define _MAIN_THREAD__
#include "thread.h"

namespace NSQTOOL
{
    class CMainThread:public CThread
    {
    public:
        CMainThread(int32_t iThreadType, int32_t iThreadId);
    };
};
#endif
