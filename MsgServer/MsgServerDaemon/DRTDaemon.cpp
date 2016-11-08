#include <iostream>
#include <stdio.h>
#include <assert.h>

#include "DRTDaemon.h"
#include "atomic.h"
#include "OS.h"
#include "OSThread.h"
#include "IdleTask.h"
#include "Socket.h"
#include "SocketUtils.h"
#include "TimeoutTask.h"
#include "RTUtils.hpp"


static bool		g_inited = false;
static char*	g_pVersion = (char*)"0.01.20150810";
static DRTDaemon*	g_pDaemon = NULL;

const char* defSvrSh = "sequence.sh:storage.sh:logical.sh:connector.sh:dispatcher.sh:rtlive.sh:grouper.sh:pusher.sh";
const char* defSvrNm = "MsgServerSequence:MsgServerStorage:MsgServerLogical:MsgServerConnector:MsgServerDispatcher:MsgServerRTLive:MsgServerGrouper:MsgServerPusher";

static int g_test_count = 0;

void DRTDaemon::PrintVersion()
{
	printf("<******** DYNC MSG Server ********>\r\n");
	printf("* Version:\t %s \r\n", g_pVersion);
	printf("* Authed:\t Xmc & Mzw \r\n");
	printf("* \r\n");
	printf("* Copyright:\t www.dync.cc \r\n");
	printf("<**********************************>\r\n");
	printf("\r\n");
}


void DRTDaemon::Initialize(int evTbSize)
{
	// Initialize utility classes
    LI("Hello daemon server...\n");
	bool inDontFork = false;
	OS::Initialize();//initialize
	OSThread::Initialize();// initialize
    IdleTask::Initialize();// initialize IdleTaskThread

	// Initialize socket classes
	Socket::Initialize(evTbSize);// initialize EventThread
	SocketUtils::Initialize(!inDontFork);
#if !MACOSXEVENTQUEUE
	//Initialize the select() implementation of the event queue
	::select_startevents();
#endif

	{
		SInt32 numShortTaskThreads = 0;
		SInt32 numBlockingThreads = 0;
		SInt32 numThreads = 0;
		SInt32 numProcessors = 0;
		numProcessors = OS::GetNumProcessors();
		//////LI("Processer core Num:%lu\n",numProcessors);
		// Event thread need 1 position;
		numShortTaskThreads = numProcessors*2 - 1;
		if(numShortTaskThreads == 0)
			numShortTaskThreads = 2;
		numBlockingThreads = 1;
		numThreads = numShortTaskThreads + numBlockingThreads;
		// ////LI("Add threads shortask=%lu blocking=%lu\n",numShortTaskThreads, numBlockingThreads);
		TaskThreadPool::SetNumShortTaskThreads(numShortTaskThreads);
		TaskThreadPool::SetNumBlockingTaskThreads(numBlockingThreads);
		TaskThreadPool::AddThreads(numThreads);//initialize TaskThread & Start
	}

	TimeoutTask::Initialize();// initialize Task

	Socket::StartThread();// start EventThread
	OSThread::Sleep(100);
	g_pDaemon = new DRTDaemon();
	g_inited = true;
}

void DRTDaemon::DeInitialize()
{
	delete g_pDaemon;
	g_pDaemon = NULL;
	g_inited = false;
	TaskThreadPool::RemoveThreads();

	//Now, make sure that the server can't do any work
	TimeoutTask::UnInitialize();

	SocketUtils::UnInitialize();
    sleep(1);
    IdleTask::UnInitialize();

#if !MACOSXEVENTQUEUE
	::select_stopevents();
#endif

	Socket::Uninitialize();// uninitialize EventThread
	OS::UnInitialize();
    LI("ByeBye daemon server...\n");
}

DRTDaemon* DRTDaemon::Inst()
{
	Assert(g_pDaemon != NULL);
	return g_pDaemon;
}

DRTDaemon::DRTDaemon(void)
:mServerManager(nullptr){

}

DRTDaemon::~DRTDaemon(void)
{

}

int	DRTDaemon::Start(const RTConfigParser& conf)
{
	Assert(g_inited);

    std::string strSvrSh = conf.GetValue("server", "server_shell", defSvrSh);
    LI("read from param.conf strSvrSh is:%s\n", strSvrSh.c_str());
    std::string strSvrNm = conf.GetValue("server", "server_name", defSvrNm);
    LI("read from param.conf strSvrNm is:%s\n", strSvrNm.c_str());

    mServerManager = new DRTServerManager(strSvrSh, strSvrNm);

    mServerManager->StartAllServer();
	return 0;
}

void DRTDaemon::DoTick()
{
#if 1
    if (mServerManager)
    {
         mServerManager->LoopCheckServer();
    }
    if (g_test_count++ == 60)
    {
         //mServerManager->StopAllServer();
    }
#endif
}

void DRTDaemon::Stop()
{
    if (mServerManager)
    {
        mServerManager->StopAllServer();
         delete mServerManager;
         mServerManager = nullptr;
    }
}
