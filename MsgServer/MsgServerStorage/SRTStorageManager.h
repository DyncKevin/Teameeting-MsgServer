//
//  SRTStorageManager.h
//  MsgServerStorage
//
//  Created by hp on 12/3/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#ifndef __MsgServerStorage__SRTStorageManager__
#define __MsgServerStorage__SRTStorageManager__

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
#include "SRTStorageRedis.h"

#include "ProtoCommon.h"

class SRTTransferSession;

class SRTStorageManager : public RTSingleton< SRTStorageManager >{
    friend class RTSingleton< SRTStorageManager >;
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
    SRTStorageManager()
    {

    }

    ~SRTStorageManager()
    {

    }
private:
    std::vector<std::string>            m_RedisHosts;
};

#endif /* defined(__MsgServerStorage__SRTStorageManager__) */
