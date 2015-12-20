#ifndef __SINGLETON_H_
#define __SINGLETON_H_
#include <stdio.h>
namespace NSQTOOL
{
	//暂时不加锁,以后有时间再做
	template<typename T>
	class CSingleton
	{
	public:	
		static T *GetInstance()
		{
			if (m_pInstance == NULL)
			{
				m_pInstance = new T;
			}

			return m_pInstance;
		}

		static void RealseInstance()
		{
			if (m_pInstance != NULL)
			{
				delete m_pInstance;
				m_pInstance = NULL;
			}
		}

	private:
		CSingleton()
		{
		}
		static T *m_pInstance;
	};

	template<typename T>
	T *CSingleton<T>::m_pInstance = NULL;
	
};
#endif
