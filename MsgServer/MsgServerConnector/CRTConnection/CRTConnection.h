#ifndef __MsgServerConnector_CRTCONNECTION_H__
#define __MsgServerConnector_CRTCONNECTION_H__
#include "RTTcp.h"
#include "CRTConnHttp.h"
#include "OSMutex.h"
#include "RTObserverConnection.h"

class CRTConnection
	: public RTTcp
	, public CRTConnHttp
    , public RTObserverConnection
{
public:
	CRTConnection(void);
	virtual ~CRTConnection(void);

public:
    int SendDispatch(const std::string& id, const std::string& msg);
public:
	//* For RCTcp
    virtual void OnRecvData(const char*pData, int nLen);
    virtual void OnSendEvent(const char*pData, int nLen) {}
    virtual void OnWakeupEvent(const char*pData, int nLen) {}
    virtual void OnPushEvent(const char*pData, int nLen) {}
    virtual void OnTickEvent(const char*pData, int nLen) {}
    virtual void OnRedisEvent(const char*pData, int nLen) {}

public:
	//* For RTConnHttp
    virtual void OnLogin(const char* pUserid, const char* pPass, const char* pNname);
    virtual void OnSndMsg(const char* pUserid, int mType, const char* pData, int dLen);
    virtual void OnGetMsg(const char* pUserid, int mType);
    virtual void OnLogout(const char* pUserid);
	virtual void OnResponse(const char*pData, int nLen);

public:
    //* For RTObserverConnection
    virtual void ConnectionDisconnected();
private:
	char			*m_pBuffer;
	int				m_nBufLen;
	int				m_nBufOffset;
    std::string     m_connectorId;
    std::string     m_userId;
    std::string     m_token;
    std::string     m_nname;
    OSMutex         m_mutex;
};

#endif	// __MsgServerConnector_CRTCONNECTION_H__
