#ifndef __MsgServerModule_MRTLOGICAL_H__
#define __MsgServerModule_MRTLOGICAL_H__
#include "TCPListenerSocket.h"
#include "MRTModuleListener.h"
#include "RTConfigParser.h"
#include "RTServerSession.h"

#include <iostream>
#include <map>
#include <vector>

class MRTModule
{
protected:
	MRTModule(void);
public:

	virtual ~MRTModule(void);

	static void PrintVersion();
	static void Initialize(int evTbSize);
	static void DeInitialize();
	static MRTModule* Inst();

    static void NodeAddCb(const std::string& nodePath);
    static void NodeDelCb(const std::string& nodePath);
public:
	int		Start(const RTConfigParser& conf);
	void	DoTick();
	void	Stop();

private:

    MRTModuleListener    *m_pModuleListener;
    RTServerSession                mServerSession;

};

#endif	// __MsgServerModule_MRTLOGICAL_H__
