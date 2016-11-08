//
//  DRTServerSession.cpp
//  dyncRTDaemon
//
//  Created by hp on 11/26/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#include "DRTServerSession.h"

DRTServerSession::DRTServerSession()
{
    AddObserver(this);
}

DRTServerSession::~DRTServerSession()
{
    DelObserver(this);
}

void DRTServerSession::Init()
{
}

void DRTServerSession::Unin()
{
}

// from RTEventLooper
// Post for read
void DRTServerSession::OnPostEvent(const char*pData, int nSize)
{
    if (nullptr == pData || nSize==0) return;
    std::string path(pData, nSize);
    LI("DRTServerSession::OnPostEvent path:%s, pData:%s\n", path.c_str(), pData);

    std::string item("");
    NodePath2Shell(path, item);
    if (item.length()==0) return;
    std::vector<std::string> args;
    args.push_back(item);
    args.push_back("start");
    std::string node = item.substr(0, item.find_first_of("."));

    LI("DRTServerSession::OnPostEvent parse item is:%s\n", item.c_str());

    if (fork()==0)
    {
        mServerLauncher.ParseServerCmdsAndLaunch(item, args);
    }
}

// Push for write
void DRTServerSession::OnPushEvent(const char*pData, int nSize)
{

}

void DRTServerSession::ConnectionDisconnected()
{
     LI("%s was called\n", __FUNCTION__);
}

///////////////////////////////////////////////////////


void DRTServerSession::NodePath2Shell(const std::string& path, std::string& shell)
{
    if (path.length()>128) return;
    char tmp[128] = {0};
    memcpy(tmp, path.c_str(), path.length());

    int count = 0;
    char name[16] = {0};
    int i = 0, j=0;
    while(i<strlen(tmp))
    {
         if (tmp[i++]=='/')
         {
             count++;
         }
         if (count==4) break;
         if (count==3)
         {
            name[j++] = tmp[i];
         }
    }
    name[j-1] = '.';
    name[j] = 's';
    name[j+1] = 'h';
    shell.assign(name);
}
