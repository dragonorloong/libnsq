/*************************************************************************
	> File Name: httpprotocol.h
	> Author: 
	> Mail: 
	> Created Time: 2015年10月25日 星期日 13时21分32秒
 ************************************************************************/

#ifndef _HTTP_PROTOCOL_H
#define _HTTP_PROTOCOL_H
#include "protocol.h"
//简单的http粘包处理
//所有请求都不继承CData，后续有时间加上
namespace NSQTOOL
{
    class CHttpRequest
    {
    public:
        void SetRequest(const char *pUrl, const char *pHost, 
                        const char *pMethod = "GET", const char *pVersion = "HTTP/1.1");
        void SetBody(const char *pBody, int32_t iLength);
        std::string &Encode();
    private:
        void SetContentLength(int32_t iLength);
    private:
        std::string m_strStream;
    };

    class CHttpResponse:public CProtocol
    {
    public:
        int32_t Need(const char *pData, int32_t iLength);
        std::string &GetBody();
        std::string &GetResult();
    private:
        std::string m_strBody; //响应包体
        std::string m_strResult; //响应状态
    }; 

};
#endif
