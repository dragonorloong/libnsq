/*************************************************************************
	> File Name: singleton.h
	> Author: 
	> Mail: 
	> Created Time: 2015年10月20日 星期二 16时44分45秒
 ************************************************************************/

#ifndef _SINGLETON_H
#define _SINGLETON_H
#include "guard.h"

namespace NSQTOOL
{
    template<typename T>
    class CSingleton
    {
    public:
        static T *GetInstance()
        {
            if (m_instance == NULL)
            {
                CGuard<CLock> guard(&m_lock); 

                if (m_instance == NULL)
                {
                    m_instance = new T(); 
                }
            }

            return m_instance;
        }

        static void ReleaseInstance()
        {
            CGuard<CLock> guard(&m_lock);         

            if (m_instance != NULL)
            {
                delete m_instance; 
                m_instance = NULL;
            }
        }

    private:
        CSingleton()
        {
        }

    private:
        static T *m_instance;
        static CLock m_lock;
    };

    template<typename T>
    T *CSingleton<T>::m_instance = NULL;

    template<typename T>
    CLock CSingleton<T>::m_lock;
};
#endif
