#ifndef _COMMAND_H_
#define _COMMAND_H_
#include <stdint.h>
#include <pthread.h>
namespace NSQTOOL
{
	enum EInternalCmdType
	{
		STOP_TYPE = -1,
		NET_CONNECT_TYPE = -2,
		NET_LISTEN_TYPE = -3,
		NET_DEL_TYPE = -4,
		NET_ADD_TYPE = -5,
		NET_SEND_TYPE = -6,
        TIMER_ADD_TYPE = -8,
        TIMER_DEL_TYPE = -9
	};

    enum EInternalThreadType
    {
        NET_THREAD_TYPE = -1,
        LISTEN_THREAD_TYPE = -2,
        TIMER_THREAD_TYPE = -3,
        MAIN_THREAD_TYPE = -4,
    };

	class CCommand
	{
	public:
		struct CCmdAddr
		{
			pthread_t m_iSrcTid;
			int32_t m_iSrcType;			
			pthread_t m_iDstTid;
			pthread_t m_iDstType;
		};

		CCommand(int32_t iType):m_iCmdType(iType),m_pLData(NULL), m_pRData(NULL)
		{
		}

		//不负责释放
		~CCommand()
		{
		}

		void SetCmdType(int32_t iCmdType)
		{
			m_iCmdType = iCmdType;	
		}

		void SetLData(void *pData)
		{
			m_pLData = pData;
		}

		void SetRData(void *pData)
		{
			m_pRData = pData;
		}

		void *GetLData()
		{
			return m_pLData;
		}	
		
		void *GetRData()
		{
			return m_pRData;
		}

		int32_t GetCmdType()
		{
			return m_iCmdType;
		}	

		CCmdAddr &GetAddr()
		{
			return m_cAddr;
		}

		void SetAddr(CCmdAddr &cCmdAddr)
		{
			m_cAddr = cCmdAddr;	
		}	
		
	private:
		int32_t m_iCmdType;
		void *m_pLData;
		void *m_pRData;
		CCmdAddr m_cAddr;
	}; 
};
#endif
