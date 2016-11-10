#ifndef _DRTSERVER_MANAGER_H_
#define _DRTSERVER_MANAGER_H_

#include <string>
#include <vector>
#include <set>
#include "RTZKClient.hpp"
#include "RTServerSession.h"
#include "RTServerLauncher.h"

class DRTServerManager{
    public:
        DRTServerManager(const std::string& svrSh, const std::string& svrNm, const std::string& delim=":");
        ~DRTServerManager();

        int StartAllServer();
        int StopAllServer();
        int CmdAllServer(const std::string& cmd);

        bool LoopCheckServer();

        bool CheckServerExist(const std::string& nodePath);

        static void NodeAddCb(const std::string& nodePath);
        static void NodeDelCb(const std::string& nodePath);
        static void DataWatchCb(const std::string& data, const std::string& extra);

    private:
        std::vector<std::string> mVecSvrShells;
        std::vector<std::string> mVecSvrNames;
        static std::set<std::string>    msSetNodePath;

        RTServerSession                mServerSession;
        RTServerLauncher               mServerLauncher;
        std::string                     mOfflineNodePath;
        bool                            mIsNodesOk;
        bool                            mIsShutdown;
};

#endif // _DRTSERVER_MANAGER_H_

