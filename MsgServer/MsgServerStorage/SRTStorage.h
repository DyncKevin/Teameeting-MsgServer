#ifndef __MsgServerStorage_SRTSTORAGE_H__
#define __MsgServerStorage_SRTSTORAGE_H__
#include "TCPListenerSocket.h"
#include "SRTStorageListener.h"
#include "RTConfigParser.h"
#include "RTServerSession.h"

#include <iostream>
#include <map>
#include <vector>

class SRTStorage
{
protected:
	SRTStorage(void);
public:

	virtual ~SRTStorage(void);

	static void PrintVersion();
	static void Initialize(int evTbSize);
	static void DeInitialize();
	static SRTStorage* Inst();

    static void NodeAddCb(const std::string& nodePath);
    static void NodeDelCb(const std::string& nodePath);

public:
	int		Start(const RTConfigParser& conf);
	void	DoTick();
	void	Stop();

private:

    SRTStorageListener    *m_pStorageListener;
    RTServerSession                mServerSession;

};

#endif	// __MsgServerStorage_SRTSTORAGE_H__
