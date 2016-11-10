#ifndef __MsgServerDispatcher_DRTDISPATCHER_H__
#define __MsgServerDispatcher_DRTDISPATCHER_H__
#include "TCPListenerSocket.h"
#include "DRTModuleListener.h"
#include "DRTTransferSession.h"
#include "RTConfigParser.h"
#include "RTServerSession.h"

#include <iostream>

class DRTDispatcher
{
protected:
	DRTDispatcher(void);
public:
	virtual ~DRTDispatcher(void);

	static void PrintVersion();
	static void Initialize(int evTbSize);
	static void DeInitialize();
	static DRTDispatcher* Inst();

    static void NodeAddCb(const std::string& nodePath);
    static void NodeDelCb(const std::string& nodePath);

public:
	int		Start(const RTConfigParser& conf);
	void	DoTick();
	void	Stop();

private:
    DRTModuleListener                *m_pModuleListener;
    DRTTransferSession               *m_pTransferSession;

    RTServerSession                mServerSession;
};

#endif	// __MsgServerDispatcher_DRTDISPATCHER_H__
