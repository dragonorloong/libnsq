#include "nsqprotocol.h"

namespace NSQTOOL
{
int32_t CNsqdResponse::Need(const char *pData, int32_t iLength)
{
	m_strResponse.append(pData, iLength);
	size_t needLen=0;
	if(iLength<4){
		needLen=4;
	}else{
		needLen = ntohl(*(uint32_t*)m_strResponse.c_str()) + 4;
	}
	if(needLen>=iLength){
		Decode(m_strResponse.c_str(), needLen);
		return 0;
	}

	return needLen - iLength;
}

int32_t CNsqdResponse::Process()
{
	return 0;
}


int32_t CNsqLookupMsg::Need(const char *pData, int32_t iLength)
{
	/*size_t needLen=0;
	if(m_pStream->size()<4){
		needLen=1000;
	}else{
		needLen = GetLen();
	}
	if(needLen>=m_pStream->size()){
		return needLen-m_pStream->size();
	}
	*/
	return 0;
}

int32_t CNsqLookupMsg::Process()
{
/*	if (need() == 0)
	{
		m_rec.Decode(m_pStream->buffer(), this->GetLen());
		m_data.set(&m_rec);
		return STATUS_FINISH;
	}
	else
	{
		return STATUS_EGAIN;
	}
*/
	return 0;
}

};

