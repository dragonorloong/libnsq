#ifndef _PROTOCOL_H_
#define _PROTOCOL_H_
#include <stdint.h>
namespace NSQTOOL
{
	class CData
	{
	public:
		virtual int32_t Need(const char *pData, int32_t iLength) = 0;
		virtual int32_t Process() = 0;
		static CData *New(int32_t iPkgType);
	};
};
#endif
