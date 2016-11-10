#ifndef __MsgServerPusher_PRTPUSHER_H__
#define __MsgServerPusher_PRTPUSHER_H__
#include "TCPListenerSocket.h"
#include "PRTPusherListener.h"
#include "RTConfigParser.h"
#include "PRTApnsPusher.h"
#include "RTServerSession.h"

#include <iostream>
#include <map>
#include <vector>

class PRTPusher
{
protected:
	PRTPusher(void);
public:

	virtual ~PRTPusher(void);

	static void PrintVersion();
	static void Initialize(int evTbSize);
	static void DeInitialize();
	static PRTPusher* Inst();

    static void NodeAddCb(const std::string& nodePath);
    static void NodeDelCb(const std::string& nodePath);

public:
	int		Start(const RTConfigParser& conf);
	void	DoTick();
	void	Stop();

private:

    PRTPusherListener    *m_pModuleListener;
    PRTApnsPusher        *m_apnsPusher;

    RTServerSession                mServerSession;

};

#endif	// __MsgServerPusher_PRTPUSHER_H__
