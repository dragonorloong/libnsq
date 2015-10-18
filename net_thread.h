#ifndef _NET_THREAD_H_
#define _NET_THREAD_H_
#include "nsq_thread.h"
#include "protocol.h"
#include "event2/event.h"
#include "event2/bufferevent.h"
#include "event2/buffer.h"
#include <string>
#include <set>
#include <map>
#include <stdint.h>
#include <errno.h>
#include "listener.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

namespace NSQTOOL
{
	class CNetThread:public CThread
	{
	public:
		struct SNetContext
		{
			int32_t m_iHandle;
			bufferevent *m_pBufevt;
			int32_t m_iPkgType;
			CData *m_pPkg;

			SNetContext(int32_t iHandle, bufferevent *pBufevt,  int32_t iPkgType)
			{
				m_iHandle = iHandle;
				m_pBufevt = pBufevt;
				m_iPkgType = iPkgType;
				m_pPkg = CData::New(iPkgType);
			}
		};

		CNetThread():m_pEventBase(NULL){}
		~CNetThread()
		{
			if (m_pEventBase != NULL)
			{
				event_base_free(m_pEventBase);
			}	
		}

		int32_t Init(void *pArg)
		{
			m_pEventBase = event_base_new();			
		}

		static void OnRead(struct bufferevent *pBufevt, void *arg)
		{
			CNetThread *pThis = (CNetThread*)arg;	
			std::map<int, SNetContext *> m_mapNetContext;
			SNetContext *pContext = pThis->m_mapNetContext[bufferevent_getfd(pBufevt)];
			//内存拷贝三次，太浪费，后续改进
			int iLength = evbuffer_get_length(bufferevent_get_input(pBufevt));
			char *pData = new char[iLength];
			bufferevent_read(pBufevt, pData, iLength);			

			if (pContext->m_pPkg->Need(pData, iLength) == 0)
			{
				pContext->m_pPkg->Process();	
			}

			delete pData;
		}

		static void OnError(struct bufferevent *pBufevt, short iTemp, void *arg)
		{
			
		}

		int  SendData(int iHandle, std::string *pString)
		{
			if (m_mapNetContext[iHandle] == NULL)
			{
				return -1;	
			}
			
			int iRet = bufferevent_write(m_mapNetContext[iHandle]->m_pBufevt, pString->c_str(), pString->size());	
			
			if (iRet != 0)
			{
				fprintf(stderr, "send data field, iHandle = %d", iHandle);
			}

			return iRet;
		}

		void ProcessCmd(CCommand *pCmd)
		{
			switch(pCmd->GetCmdType())
			{
				case NET_CONNECT_TYPE:
				{
					//只做tcp的connect
					sockaddr_in *pAddr = (sockaddr_in *)pCmd->GetLData();		
					int32_t iPacketType = (int64_t)pCmd->GetRData();
					fprintf(stdout, "NET_CONNECT_TYPE:%s,%d\n", inet_ntoa(pAddr->sin_addr), pAddr->sin_port);		
					int iHandle = socket(AF_INET, SOCK_STREAM, 0);	
					
					if (iHandle == -1)
					{
						fprintf(stderr, "socket return error, errno = %d\n", errno);		
					}

					struct bufferevent * bufevt = bufferevent_socket_new(m_pEventBase, iHandle, 0);
					int32_t iRet = bufferevent_socket_connect(bufevt, (sockaddr*)pAddr, sizeof(sockaddr_in));
					
					if (iRet != 0)
					{
						bufferevent_free(bufevt);
						bufevt = NULL;
						//不肯定bufferevent_free是否会释放socket，后续测试
						fprintf(stderr, "socket connect return error: %s,%d\n", inet_ntoa(pAddr->sin_addr), pAddr->sin_port);
						break;
					}

					bufferevent_setcb(bufevt, OnRead, NULL, OnError, this);
					bufferevent_enable(bufevt, EV_READ);		

					SNetContext *pNetContext= new SNetContext(iHandle, bufevt, iPacketType);
					m_mapNetContext[iHandle] = pNetContext;

					//回一个包给源线程
					CCommand::CCmdAddr &cAddr = pCmd->GetAddr();	
					CCommand::CCmdAddr cSendAddr;
					cSendAddr.m_iSrcTid = GetThreadId();
					cSendAddr.m_iSrcThreadType = GetThreadType();
					cSendAddr.m_iDstTid = cAddr.m_iSrcTid;
					cSendAddr.m_iDstThreadType = cAddr.m_iSrcThreadType;
					CCommand cmd(NET_CONNECT_TYPE);
					cmd.SetAddr(cSendAddr);
					cmd.SetLData((int32_t*)iHandle);
					//cmd.SetCmdType(NET_CONNECT_TYPE);
					CThreadMgrSingleton::GetInstance()->SendCmd(cAddr.m_iSrcThreadType, cmd, cAddr.m_iSrcTid);

					//内存释放
					delete pAddr;
				}
				break;
				case NET_DEL_TYPE:
				{
					int iHandle = (int64_t)pCmd->GetLData();

					if (m_mapNetContext[iHandle]->m_pBufevt != NULL)	
					{
						bufferevent_free(m_mapNetContext[iHandle]->m_pBufevt);
						m_mapNetContext[iHandle]->m_pBufevt = NULL;
					}
					
					delete m_mapNetContext[iHandle];
					m_mapNetContext.erase(iHandle);
					
				}
				case NET_ADD_TYPE:
				{
					//listen上来的包，默认由pkg的process函数
					int iHandle = (int64_t)pCmd->GetLData();
					int32_t iPacketType = (int64_t)pCmd->GetRData();
					struct bufferevent * bufevt = bufferevent_socket_new(m_pEventBase, iHandle, 0);
					bufferevent_setcb(bufevt, OnRead, NULL, OnError, this);
					bufferevent_enable(bufevt, EV_READ);		
					SNetContext *pNetContext= new SNetContext(iHandle, bufevt, iPacketType);
					m_mapNetContext[iHandle] = pNetContext;
				}
				break;
				case NET_SEND_TYPE:
				{
					int iHandle = (int64_t)pCmd->GetLData();
					std::string *pString = (std::string*)pCmd->GetRData();

					if (SendData(iHandle, pString) != 0)
					{
						//发送失败应该回包给源线程，暂时忽略
					}

					delete pString;
				};
				break;
				
			}
		}	
		
		void RealRun()
		{
			while (!m_bStop)	
			{
				CThread::ProcessCmd();
				struct timeval sTm; 
				sTm.tv_sec =1;
				sTm.tv_usec = 0;
				event_base_loopexit(m_pEventBase, &sTm);	
			}
		}

	private:
		event_base *m_pEventBase;
		std::map<int, SNetContext *> m_mapNetContext;
	};

	class CListenThread:public CThread
	{
	public:
		struct SListenInfo
		{
			std::string m_strAddr;
			int m_iPkgType;	
			evconnlistener *m_pListener;
			int32_t m_iDstThreadType;
			int32_t m_iDstThreadNum;
		};

		void RealRun()
		{
			while (!m_bStop)	
			{
				CThread::ProcessCmd();
				struct timeval sTm; 
				sTm.tv_sec =1;
				sTm.tv_usec = 0;
				event_base_loopexit(m_pEventBase, &sTm);	
			}
		}

		static void OnRead(struct evconnlistener *pListener, evutil_socket_t iAcceptHandle, struct sockaddr *pAddr, int socklen, void *pArg)		
		{
			CListenThread* pThis = (CListenThread*)pArg;		
			CCommand cCmd(NET_ADD_TYPE);
			cCmd.SetLData((int32_t*)iAcceptHandle);
			cCmd.SetRData((int32_t*)pThis->m_mapListen[evconnlistener_get_fd(pListener)].m_iPkgType);
			CThreadMgrSingleton::GetInstance()->SendCmd(pThis->m_mapListen[evconnlistener_get_fd(pListener)].m_iDstThreadType, 
					cCmd,pThis->m_mapListen[evconnlistener_get_fd(pListener)].m_iDstThreadNum);
		}
		
		void ProcessCmd(CCommand *pCmd)
		{
			switch(pCmd->GetCmdType())
			{
				case NET_LISTEN_TYPE:
				{
					sockaddr_in *pAddr = (sockaddr_in *)pCmd->GetLData();		
					char buff[1024];
					snprintf(buff, sizeof(buff), "%s_%d", inet_ntoa(pAddr->sin_addr), pAddr->sin_port);
					fprintf(stdout, "NET_LISTEN_TYPE:%s\n", buff);		
					std::string strAddr = buff;

					 evconnlistener * pListener = evconnlistener_new_bind(m_pEventBase, OnRead, this, 
							LEV_OPT_THREADSAFE|LEV_OPT_REUSEABLE|LEV_OPT_CLOSE_ON_FREE, 10,
							(sockaddr*)pAddr, sizeof(sockaddr_in));
					int iHandle = evconnlistener_get_fd(pListener);	
					SListenInfo sListenInfo;
					sListenInfo.m_strAddr = buff;	
					sListenInfo.m_pListener = pListener;
					sListenInfo.m_iPkgType = (int64_t)pCmd->GetRData();
					sListenInfo.m_iDstThreadType = pCmd->GetAddr().m_iDstThreadType;
					sListenInfo.m_iDstThreadNum = pCmd->GetAddr().m_iDstTid;
					m_mapListen[iHandle] = sListenInfo;
					delete pAddr;
				}
				break;
				case NET_DEL_TYPE:
				{
					std::string *pString = (std::string *)pCmd->GetLData();
					std::map<int, SListenInfo>::iterator iter = m_mapListen.begin();
					
					for (; iter != m_mapListen.end(); ++iter)
					{
						if (iter->second.m_strAddr == *pString)
						{
							evconnlistener_free(iter->second.m_pListener);
							m_mapListen.erase(iter);
							break;
						}
					}

					delete pString;
				}
				break;
			}
		}
	private:
		event_base *m_pEventBase;
		std::map<int, SListenInfo> m_mapListen;
	};
};
#endif
