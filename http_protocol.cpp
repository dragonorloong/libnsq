/*************************************************************************
	> File Name: httpprotocol.cpp
	> Author: 
	> Mail: 
	> Created Time: 2015年10月25日 星期日 15时53分52秒
 ************************************************************************/
#include "http_protocol.h"
#include <stdio.h>
#include <string>

namespace NSQTOOL
{
    void CHttpRequest::SetRequest(const char *pUrl, const char *pHost,
                                  const char *pMethod, const char *pVersion)
    {
        m_strStream.append(pMethod);
        m_strStream.append(" ");
        m_strStream.append(pUrl);
        m_strStream.append(" ");
        m_strStream.append(pVersion);
        m_strStream.append("\r\n");
        m_strStream.append("Host:");
        m_strStream.append(pHost);
        m_strStream.append("\r\n");
        m_strStream.append("Accept:*/*");
        m_strStream.append("\r\n");
        m_strStream.append("\r\n");
    }

    void CHttpRequest::SetContentLength(int32_t iLength)
    {
        m_strStream.append("Content-Length:");
        char buff[64] = {0};
        snprintf(buff, sizeof(buff), "%d", iLength+2);
        m_strStream.append(buff);
        m_strStream.append("\r\n");
    }

    void CHttpRequest::SetBody(const char *pBody, int32_t iLength)
    {
        SetContentLength(iLength);
        m_strStream.append("\r\n");
        m_strStream.append(pBody, iLength); 
    }

    std::string &CHttpRequest::Encode()
    {
        return m_strStream;
    }

    int32_t CHttpResponse::Need(const char *pData, int32_t iLength)
    {
        if (iLength == 0 && m_strStream.empty())    
        {
            return 19; 
        }

        m_strStream.append(pData, iLength);

        size_t iHeadEndPos = m_strStream.find("\r\n\r\n"); 
        if (iHeadEndPos == std::string::npos)
        {
            return 1;
        }
        else
        {
            size_t iContentLengthPos = m_strStream.find("Content-Length:");

            if (iContentLengthPos == std::string::npos)
            {
                size_t iResultBegin = m_strStream.find(" ");
                size_t iResultEnd = m_strStream.find(" ", iResultBegin + 1);
                m_strResult = m_strStream.substr(iResultBegin + 1, iResultEnd - iResultEnd);
                m_strBody = "";
                m_iCurPkgLength = iHeadEndPos;
                return 0;
            }

            size_t iContentEndPos = m_strStream.find("\r\n", iContentLengthPos);

            if (iContentEndPos == std::string::npos)
            {
                return 1;
            }

            std::string strBody = m_strStream.substr(iContentLengthPos + 15, iContentEndPos - iContentLengthPos - 15); 
            int iBodyLength;
            sscanf(strBody.c_str(), "%d", &iBodyLength);
            int32_t iNeedLength = m_strStream.length() - iHeadEndPos - 4 - iBodyLength;

            if (iNeedLength >= 0)
            {
                size_t iResultBegin = m_strStream.find(" ");
                size_t iResultEnd = m_strStream.find(" ", iResultBegin + 1);
                m_strResult = m_strStream.substr(iResultBegin + 1, iResultEnd - iResultEnd);
                m_strBody = m_strStream.substr(iHeadEndPos + 4, iBodyLength);
                m_iCurPkgLength = iHeadEndPos + 4 + iBodyLength;
                return 0;
            }

            return -iNeedLength;
        }

    }

    std::string &CHttpResponse::GetBody()
    {
        return m_strBody;
    }

    std::string &CHttpResponse::GetResult()
    {
        return m_strResult;
    }
}
