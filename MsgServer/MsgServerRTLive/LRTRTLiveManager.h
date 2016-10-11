//
//  LRTRTLiveManager.h
//  MsgServerRTLive
//
//  Created by hp on 12/3/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#ifndef __MsgServerRTLive__LRTRTLiveManager__
#define __MsgServerRTLive__LRTRTLiveManager__

#include <stdio.h>
#include <iostream>
#include <unordered_map>
#include <utility>
#include <list>
#include <set>
#include <vector>
#include <algorithm>
#include "RTType.h"
#include "RTSingleton.h"

#include "ProtoCommon.h"

class LRTTransferSession;

class LRTRTLiveManager : public RTSingleton< LRTRTLiveManager >{
    friend class RTSingleton< LRTRTLiveManager >;
public:

    bool RecvRequestCounter();
    bool SendResponseCounter();

    bool InitManager()
    {

    }

    bool UninManager()
    {
        m_RedisHosts.clear();
    }

    void PushRedisHosts(std::string host)
    {
        m_RedisHosts.push_back(host);
    }

    void GetRedisHosts(std::vector<std::string>* hosts)
    {
         for(std::vector<std::string>::iterator it=m_RedisHosts.begin();it!=m_RedisHosts.end();++it)
         {
             hosts->push_back(*it);
         }
    }

    bool    SignalKill();
    bool    ClearAll();
protected:
    LRTRTLiveManager()
    {

    }

    ~LRTRTLiveManager()
    {

    }
private:
    std::vector<std::string>            m_RedisHosts;
    std::list<LRTTransferSession*>      m_TransferSessions;
};

#endif /* defined(__MsgServerRTLive__LRTRTLiveManager__) */
