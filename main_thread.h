/*************************************************************************
	> File Name: main_thread.h
	> Author: 
	> Mail: 
	> Created Time: 2015年10月25日 星期日 22时41分15秒
 ************************************************************************/

#ifndef _MAIN_THREAD_H
#define _MAIN_THREAD_H
#include "nsq_thread.h"
namespace NSQTOOL
{
    class CMainThread:public CThread
    {
    public:
        enum MAINTHREADTIMER
        {
            LOOKUP_TIMER = 1,
        };

        void RealProcessCmd(CCommand &cCmd); 
        void RealRun(); 
    private:
    };
        
};

#endif
