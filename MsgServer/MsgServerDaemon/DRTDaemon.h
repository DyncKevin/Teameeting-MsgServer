#ifndef __MsgServerDaemon_DRTDAEMON_H__
#define __MsgServerDaemon_DRTDAEMON_H__

#include "RTConfigParser.h"

#include <iostream>
#include <map>
#include <vector>

#include "DRTServerManager.h"

class DRTDaemon
{
protected:
	DRTDaemon(void);
public:

	virtual ~DRTDaemon(void);

	static void PrintVersion();

	static void Initialize(int evTbSize);

	static void DeInitialize();

	static DRTDaemon* Inst();

public:
	int		Start(const RTConfigParser& conf);
	void	DoTick();
	void	Stop();

private:
    DRTServerManager*    mServerManager;
};

#endif	// __MsgServerDaemon_DRTDAEMON_H__
