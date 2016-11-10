#ifndef __MsgServerLogical_LRTLOGICAL_H__
#define __MsgServerLogical_LRTLOGICAL_H__
#include "TCPListenerSocket.h"
#include "LRTLogicalListener.h"
#include "RTConfigParser.h"
#include "RTServerSession.h"

#include <iostream>
#include <map>
#include <vector>

class LRTLogical
{
protected:
	LRTLogical(void);
public:

	virtual ~LRTLogical(void);

	static void PrintVersion();
	static void Initialize(int evTbSize);
	static void DeInitialize();
	static LRTLogical* Inst();

    static void NodeAddCb(const std::string& nodePath);
    static void NodeDelCb(const std::string& nodePath);
public:
	int		Start(const RTConfigParser& conf);
	void	DoTick();
	void	Stop();

private:

    LRTLogicalListener    *m_pLogicalListener;
    RTServerSession                mServerSession;

};

#endif	// __MsgServerLogical_LRTLOGICAL_H__
