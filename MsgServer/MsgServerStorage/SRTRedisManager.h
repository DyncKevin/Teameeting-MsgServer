//
//  SRTRedisManager.h
//  MsgServerStorage
//
//  Created by hp on 11/26/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#ifndef __MsgServerStorage__SRTRedisManager__
#define __MsgServerStorage__SRTRedisManager__

#include <stdio.h>
#include <iostream>
#include <utility>
#include <string>
#include <queue>
#include <vector>
#include "OSMutex.h"
#include "SRTRedisGroup.h"

#include "ProtoCommon.h"

class SRTTransferSession;

class SRTRedisManager {
public:
    SRTRedisManager();
    virtual ~SRTRedisManager();

    void Init(SRTTransferSession* sess);
    void Unin();

    void PushRedisRequest(const std::string& request);
    void PostRedisRequest(const std::string& request);

public:

private:
    int                                 m_RedisNum;
    std::vector<std::string>            m_RedisHosts;
    SInt64                              m_LastUpdateTime;
    SRTRedisGroup*                      m_RedisGroup;
};

#endif /* defined(__MsgServerStorage__SRTRedisManager__) */
