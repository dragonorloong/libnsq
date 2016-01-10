#include "factory.h"

namespace NSQTOOL
{
    CFactory *g_pFactory = NULL;

    void CFactory::SetFactory(CFactory *pFactory)
    {
        g_pFactory = pFactory; 
    }
};
