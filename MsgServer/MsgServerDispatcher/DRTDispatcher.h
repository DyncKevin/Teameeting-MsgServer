#ifndef __MsgServerDispatcher_DRTDISPATCHER_H__
#define __MsgServerDispatcher_DRTDISPATCHER_H__
#include "TCPListenerSocket.h"
#include "DRTModuleListener.h"
#include "DRTTransferSession.h"
#include "RTConfigParser.h"

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


public:
	int		Start(const RTConfigParser& conf);
	void	DoTick();
	void	Stop();

private:
    DRTModuleListener                *m_pModuleListener;
    DRTTransferSession               *m_pTransferSession;

};

#endif	// __MsgServerDispatcher_DRTDISPATCHER_H__
