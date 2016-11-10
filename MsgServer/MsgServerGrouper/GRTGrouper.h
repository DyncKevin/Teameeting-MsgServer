#ifndef __MsgServerGrouper_GRTGROUPER_H__
#define __MsgServerGrouper_GRTGROUPER_H__
#include "TCPListenerSocket.h"
#include "GRTGrouperListener.h"
#include "RTConfigParser.h"
#include "RTServerSession.h"

#include <iostream>
#include <map>
#include <vector>

class GRTGrouper
{
protected:
	GRTGrouper(void);
public:

	virtual ~GRTGrouper(void);

	static void PrintVersion();
	static void Initialize(int evTbSize);
	static void DeInitialize();
	static GRTGrouper* Inst();

    static void NodeAddCb(const std::string& nodePath);
    static void NodeDelCb(const std::string& nodePath);
public:
	int		Start(const RTConfigParser& conf);
	void	DoTick();
	void	Stop();

private:
    GRTGrouperListener    *m_pGrouperListener;
    RTServerSession                mServerSession;
};

#endif	// __MsgServerGrouper_GRTGROUPER_H__
