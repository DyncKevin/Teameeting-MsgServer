#ifndef __MsgServerRTLive_LRTRTLIVE_H__
#define __MsgServerRTLive_LRTRTLIVE_H__
#include "TCPListenerSocket.h"
#include "LRTRTLiveListener.h"
#include "LRTGroupListener.h"
#include "LRTPushListener.h"
#include "RTConfigParser.h"
#include "RTServerSession.h"

#include <iostream>
#include <map>
#include <vector>

class LRTRTLive
{
protected:
	LRTRTLive(void);
public:

	virtual ~LRTRTLive(void);

	static void PrintVersion();
	static void Initialize(int evTbSize);
	static void DeInitialize();
	static LRTRTLive* Inst();

    static void NodeAddCb(const std::string& nodePath);
    static void NodeDelCb(const std::string& nodePath);
public:
	int		Start(const RTConfigParser& conf);

	void	DoTick();
	void	Stop();

private:

    LRTRTLiveListener    *m_pRTLiveListener;
    LRTGroupListener    *m_pGroupListener;
    LRTPushListener    *m_pPushListener;

    RTServerSession                mServerSession;
};

#endif	// __MsgServerRTLive_LRTRTLIVE_H__
