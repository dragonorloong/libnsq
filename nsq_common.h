#ifndef _NSQ_COMMON_H
#define _NSQ_COMMON_H
#include "nsqlookupthread.h"
enum CLIENTTYPE
{
	ERROR_TYPE = -1,
	SUB_TYPE,
	PUB_TYPE
};


#define NSQLOOKUP_ENTITY		27
#define NSQD_ENTITY				28
#define NSQDSUB_ENTITY			100	



#define NSQ_INIT \
do{ \
	CBaseConf & conf=CMyserver::GetInstance()->GetBaseConf(); \
	int nsqd_thread_num = conf.GetInt("NSQ","NsqdThreadNum",5); \
	AddGeneralEntity(nsqd_thread_num,NSQD_ENTITY); \
	AddGeneralEntity(1,NSQLOOKUP_ENTITY); \
\
	string strTopicNum = conf.GetStr("NSQ", "topic_num", 0); \
	int iTopicNum = atoi(strTopicNum.c_str()); \
	string strTopicName; \
	string strChannelName; \
	string strNsqType; \
	int iHandleEntity; \
	int iHandleCmdType; \
	char buff[50]; \
 \
	for (int i = 0; i < iTopicNum; ++i) \
	{ \
		snprintf(buff, sizeof(buff), "topic_%d", i); \
		strTopicName = conf.GetStr("NSQ", buff, ""); \
		snprintf(buff, sizeof(buff), "channel_%d", i);	\
		strChannelName = conf.GetStr("NSQ", buff, ""); \
		snprintf(buff, sizeof(buff), "nsq_type_%d", i);	 \
		strNsqType = conf.GetStr("NSQ", buff, ""); \
		snprintf(buff, sizeof(buff), "nsq_handle_entity_%d", i);	\
		iHandleEntity = conf.GetInt("NSQ", buff, 0); \
		snprintf(buff, sizeof(buff), "nsq_handle_cmd_%d", i);	\
		iHandleCmdType = conf.GetInt("NSQ", buff, 0); \
 \
		if (!strTopicName.empty() && !strNsqType.empty()) \
		{ \
			if (strNsqType == "SUB") \
			{ \
				REGISTER_NSQD(strTopicName,strChannelName,SUB_TYPE,iHandleEntity,iHandleCmdType); \
			} \
			else if (strNsqType == "PUB") \
			{ \
				REGISTER_NSQD(strTopicName,strChannelName,PUB_TYPE,iHandleEntity,iHandleCmdType); \
			} \
		} \
	} \
}while(0); \

#endif
