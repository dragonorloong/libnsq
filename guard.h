/*************************************************************************
	> File Name: guard.h
	> Author:
	> Mail:
	> Created Time: 2015年10月20日 星期二 17时04分11秒
 ************************************************************************/

#ifndef _GUARD_H
#define _GUARD_H
#include <pthread.h>
#include <stdint.h>

namespace NSQTOOL
{
    class CLock
    {
    public:

        CLock(int recursive = 1)
        {
            pthread_mutexattr_t attr;
            pthread_mutexattr_init(&attr);

            if (recursive == 1)
            {
                pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE_NP);
            }
            else
            {
                pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK_NP);
            }

            pthread_mutex_init(&m_mutex, &attr);
            pthread_mutexattr_destroy(&attr);
        }

        ~CLock()
        {
            pthread_mutex_destroy(&m_mutex);
        }

        int32_t Lock()
        {
            return pthread_mutex_lock(&m_mutex);
        }

        int32_t UnLock()
        {
            return pthread_mutex_unlock(&m_mutex);
        }

    private:
        pthread_mutex_t m_mutex;
    };

    class CRWLock
    {
    public:
        CRWLock()
        {
            pthread_rwlock_init(&m_rwlock, NULL);
        }

        ~CRWLock()
        {
            pthread_rwlock_destroy(&m_rwlock);
        }

        int32_t RLock()
        {
            return pthread_rwlock_rdlock(&m_rwlock);
        }

        int32_t WLock()
        {
            return pthread_rwlock_wrlock(&m_rwlock);
        }

        int32_t UnLock()
        {
            return pthread_rwlock_unlock(&m_rwlock);
        }

    private:
        pthread_rwlock_t m_rwlock;
    };

    template<typename T>
    class CGuard
    {
    public:
        CGuard(T *lock)
        {
            m_lock = lock;
            m_lock->Lock();
            m_is_lock = true;
        }

        ~CGuard()
        {
            if (m_is_lock)
            {
                m_lock->UnLock();
            }
        }

        int32_t UnLock()
        {
            if (m_is_lock)
            {
                m_is_lock = false;
                return m_lock->UnLock();
            }

            return -1;
        }

    private:
        T *m_lock;
        bool m_is_lock;
    };

    template<typename T>
    class CRWGuard
    {
    public:
        CRWGuard(T *lock, int32_t wlock=0)
        {
            m_lock = lock;

            if (wlock == 0)
            {
                m_lock->RLock();
            }
            else
            {
                m_lock->WLock();
            }

            m_is_lock = true;
        }

        ~CRWGuard()
        {
            if (m_is_lock)
            {
                m_lock->UnLock();
            }
        }

        int32_t UnLock()
        {
            if (m_is_lock)
            {
                m_is_lock = false;
                return m_lock->UnLock();
            }

            return -1;
        }

    private:
        T *m_lock;
        bool m_is_lock;
    };
};

#endif
