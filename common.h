#ifndef __COMMON_H_
#define __COMMON_H_
namespace NSQTOOL
{
    enum EConnectKeepType
    {
        ESHORTCONNECT = 0,
        ELONGCONNECT = 1,
    };

    extern int g_iShortConnectTimeout ;   //300ms
    extern int g_iLongConnectTimeout ; //30s
};
#endif
