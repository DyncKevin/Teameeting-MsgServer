#include "DRTServerManager.h"
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "RTUtils.hpp"
#include "rtklog.h"

#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/stringbuffer.h"

std::set<std::string> DRTServerManager::msSetNodePath;

DRTServerManager::DRTServerManager(const std::string& svrSh, const std::string& svrNm, const std::string& delim)
    : mOfflineNodePath("")
      , mIsNodesOk(true)
      , mIsShutdown(false)
{
    //Split(delim, svrSh, mVecSvrShells);
    //Split(delim, svrNm, mVecSvrNames);
    for (std::string item : mVecSvrShells)
    {
         LI("mVecSvrShells item:%s\n", item.c_str());
    }
    for (std::string item : mVecSvrNames)
    {
         LI("mVecSvrNames item:%s\n", item.c_str());
    }
    RTZKClient::Instance().SetNodeCallback(DRTServerManager::NodeAddCb, DRTServerManager::NodeDelCb);
    RTZKClient::Instance().SetDataWatchCallback(DRTServerManager::DataWatchCb);
    mServerSession.Init();
}

DRTServerManager::~DRTServerManager()
{
    mServerSession.Unin();
}

int DRTServerManager::StartAllServer()
{
    std::string c("start");
    return CmdAllServer(c);
}

int DRTServerManager::StopAllServer()
{
    mIsShutdown = true;
    std::string c("stop");
    return CmdAllServer(c);
}

int DRTServerManager::CmdAllServer(const std::string& cmd)
{
    if (cmd.length()==0||cmd.c_str()==nullptr) return -1;
    int ret = 0, i=0;
    std::map<int, std::string> mapPidShs;

    for (i=0;i<mVecSvrShells.size();i++)
    {
        ret = fork();
        if (ret==0)
        {
            LI("DRTServerManager::CmdAllServer in child process pid:%d, cmd:%s\n", getpid(), cmd.c_str());
            mapPidShs.insert(make_pair(getpid(), mVecSvrShells[i]));
            break;
        }
        usleep(1000*200);
    }
    if (ret==-1)
    {
        LE("DRTServerManager::CmdAllServer ret:%d, error process pid:%d, cmd:%s\n", ret, getpid(), cmd.c_str());
        return 0;
    } else if (ret==0) {
        // in child process
        std::map<int, std::string>::iterator it = mapPidShs.find(getpid());
        if (it != mapPidShs.end())
        {
            std::string item = it->second;
            std::vector<std::string> args;
            args.push_back(item);
            args.push_back(cmd);
            std::string node = item.substr(0, item.find_first_of("."));
            LI("DRTServerManager::StartAllServer node dir name:%s, cmd:%s, child pid:%d\n", node.c_str(), cmd.c_str(), getpid());
            mServerLauncher.ParseServerCmdsAndLaunch(item, args);
        }
    } else {
        // in parent process
        return 0;
    }
    return 0;
}

bool DRTServerManager::LoopCheckServer()
{
    LI("DRTServerManager::LoopCheckServer mIsNodesOk:%d, msSetNodePath.size:%d\n", mIsNodesOk, msSetNodePath.size());
    if (mIsShutdown) return mIsShutdown;
    if (mIsNodesOk)
    {
        for(auto item : msSetNodePath)
        {
            if (CheckServerExist(item))
            {
                LI("DRTServerManager::LoopCheckServer exist node path:%s\n", item.c_str());
                continue;
            } else {
                LI("DRTServerManager::LoopCheckServer not exist node path:%s\n", item.c_str());
                mIsNodesOk = false;
                mOfflineNodePath = item;
                mServerSession.PostData(item.c_str(), item.length());
                break;
            }
        }
    } else {
        LI("DRTServerManager::LoopCheckServer mOfflineNodePath is:%s\n", mOfflineNodePath.c_str());
        if (mOfflineNodePath.length()>0 && CheckServerExist(mOfflineNodePath))
        {
            mIsNodesOk = true;
            mOfflineNodePath = "";
        }
    }
    return mIsNodesOk;
}


bool DRTServerManager::CheckServerExist(const std::string& nodePath)
{
    return RTZKClient::Instance().CheckNodeExists(nodePath);
}

void DRTServerManager::NodeAddCb(const std::string& nodePath)
{
    if (nodePath.length()>0)
    {
        msSetNodePath.insert(nodePath);
        LI("DRTServerManager::NodeAddCb was called, nodePath:%s was added\n", nodePath.c_str());
    }
}

void DRTServerManager::NodeDelCb(const std::string& nodePath)
{
    if (nodePath.length()>0)
    {
        //msSetNodePath.erase(nodePath);
        LI("DRTServerManager::NodeDelCb was called, not nodePath:%s was removed\n", nodePath.c_str());
    }
}

void DRTServerManager::DataWatchCb(const std::string& data, const std::string& extra)
{
    if (data.c_str()==nullptr || data.length()==0) return;
    rapidjson::Document jDoc;
    if (!jDoc.ParseInsitu<0>((char*)data.c_str()).HasParseError())
    {
         if(jDoc.HasMember("node")&&jDoc["node"].IsString())
         {
             const char* strNode = jDoc["node"].GetString();
             if (strNode==nullptr)return;
             rapidjson::Document jDocNode;
             if (!jDocNode.ParseInsitu<0>((char*)strNode).HasParseError())
             {
                  if (jDocNode.HasMember("node")&&jDocNode["node"].IsString())
                  {
                      const char* snode = jDocNode["node"].GetString();
                      LI("DRTServerManager::DataWatchCb parse to snode:%s\n", snode);
                  }
             }
         }
    }
}
