#ifndef __MsgServerSequence_SRTSEQUENCE_H__
#define __MsgServerSequence_SRTSEQUENCE_H__
#include "TCPListenerSocket.h"
#include "SRTSequenceListener.h"
#include "RTConfigParser.h"
#include "RTServerSession.h"

#include <iostream>
#include <map>
#include <vector>

class SRTSequence
{
protected:
	SRTSequence(void);
public:

	virtual ~SRTSequence(void);

	static void PrintVersion();
	static void Initialize(int evTbSize);
	static void DeInitialize();
	static SRTSequence* Inst();

    static void NodeAddCb(const std::string& nodePath);
    static void NodeDelCb(const std::string& nodePath);
public:
	int		Start(const RTConfigParser& conf);
	void	DoTick();
	void	Stop();

private:

    SRTSequenceListener    *m_pSequenceListener;
    RTServerSession                mServerSession;

};

#endif	// __MsgServerSequence_SRTSEQUENCE_H__
