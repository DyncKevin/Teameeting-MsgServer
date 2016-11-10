#ifndef __MsgServerConnector_CRTCONNECTOR_H__
#define __MsgServerConnector_CRTCONNECTOR_H__
#include "TCPListenerSocket.h"
#include "CRTConnListener.h"
#include "CRTModuleListener.h"
#include "CRTConnTcpListener.h"
#include "CRTWebServerListener.h"
#include "RTConfigParser.h"
#include "RTServerSession.h"

#include <iostream>
#include <map>

class CRTConnector
{
protected:
	CRTConnector(void);
public:

	virtual ~CRTConnector(void);

	static void PrintVersion();
	static void Initialize(int evTbSize);
	static void DeInitialize();
	static CRTConnector* Inst();

    static void NodeAddCb(const std::string& nodePath);
    static void NodeDelCb(const std::string& nodePath);

public:
	int		Start(const RTConfigParser& conf);
	void	DoTick();
	void	Stop();

    void UpdateProcessStatus(std::string& status);

private:

	CRTConnListener		*m_pConnListener;
    CRTModuleListener    *m_pModuleListener;
    CRTConnTcpListener   *m_pConnTcpListener;
    CRTWebServerListener   *m_pWebSvrListener;

    RTServerSession                mServerSession;

};

#endif	// __MsgServerConnector_CRTCONNECTOR_H__
