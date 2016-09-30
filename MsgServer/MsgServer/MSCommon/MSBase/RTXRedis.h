//
//  RTXRedis.h
//  MsgServerPusher
//
//  Created by hp on 11/26/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#ifndef __MsgServerPusher__RTXRedis__
#define __MsgServerPusher__RTXRedis__

#include <hiredis/hiredis.h>
#include "xRedisClient.h"

#include "ProtoCommon.h"

class RTXRedis {

public:
    RTXRedis(){}
    virtual ~RTXRedis(){}

    void Init(const std::string& ip, int port);
    void Unin();

    bool Echo();
    bool KeyExists(const std::string& key);
    bool ListRPush(const std::string& key, const std::string& value, int64_t& count);
    bool ListLPush(const std::string& key, const std::string& value, int64_t& count);
    bool ListLPop(const std::string& key, std::string& reply);
    bool ListRPop(const std::string& key, std::string& reply);
    bool ListLLen(const std::string& key, int64_t& count);
private:
    std::string                      m_Ip;
    int                              m_Port;
    int                              m_RedisNodeNum;

    xRedisClient                     m_xRedisClient;
    RedisNode*                       m_RedisList;
};

#endif /* defined(__MsgServerPusher__RTXRedis__) */
