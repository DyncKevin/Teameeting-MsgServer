#ifndef __RT_HTTP_SENDER_H__
#define __RT_HTTP_SENDER_H__
#include "RTTcp.h"
#include "RTConnHttp.h"

#include "../../ProtoCommon.h"

class RTHttpSender
	: public RTTcp
	, public RTConnHttp
{
public:
	RTHttpSender(void);
    RTHttpSender(int cmd, pms::RelayMsg& rmsg, pms::Entity& msg);
	virtual ~RTHttpSender(void);

public:
    bool ConnHttpHost(const std::string& addr, const unsigned short port, const std::string& host);
    void SendRequest(const char* pData, int nLen);
    http_method&  GetMethod() { return m_method; }
    int           GetCmd() { return m_cmd; }
    pms::RelayMsg&     GetTransmsg() { return m_transmsg; }
    pms::Entity&      GetMeetmsg() { return m_meetmsg; }

public:
	//* For RCTcp
	virtual void OnRecvData(const char*data, int size);
    virtual void OnSendEvent(const char*pData, int nLen) {}
    virtual void OnWakeupEvent(const char*pData, int nLen) {}
    virtual void OnPushEvent(const char*pData, int nLen) {}
    virtual void OnTickEvent(const char*pData, int nLen) {}
    virtual void OnRedisEvent(const char*pData, int nLen) {}
public:
	//* For RTConnHttp
	virtual void OnResponse(const char*pData, int nLen) = 0;

private:
	char			*m_pBuffer;
	int				m_nBufLen;
	int				m_nBufOffset;
    http_method     m_method;
    int             m_cmd;
    pms::RelayMsg        m_transmsg;
    pms::Entity          m_meetmsg;
};

#endif	// __MsgServerMeeting_MRT_HTTP_SENDER_H__
