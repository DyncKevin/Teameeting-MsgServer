//
//  PRTXRedis.cpp
//  dyncRTStorage
//
//  Created by hp on 11/26/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#include "PRTXRedis.h"
#include "rtklog.h"
#include "StatusCode.h"

static unsigned int APHash(const char *str) {
    unsigned int hash = 0;
    int i;
    for (i=0; *str; i++) {
        if ((i&  1) == 0) {
            hash ^= ((hash << 7) ^ (*str++) ^ (hash >> 3));
        } else {
            hash ^= (~((hash << 11) ^ (*str++) ^ (hash >> 5)));
        }
    }
    return (hash&  0x7FFFFFFF);
}

enum {
 CACHE_TYPE_1,
 CACHE_TYPE_2,
 CACHE_TYPE_MAX,
};

void PRTXRedis::Init(const std::string& ip, int port)
{
    //LI("PRTXRedis::Init was called, ip:%s, port:%d\n", ip.c_str(), port);
    m_Ip = ip;
    m_Port = port;
    m_RedisNodeNum = 1;

    m_RedisList = new RedisNode[m_RedisNodeNum];
    for (int i=0;i<m_RedisNodeNum;++i) {
        m_RedisList[i].dbindex = i;
        m_RedisList[i].host = m_Ip.c_str();
        m_RedisList[i].port = m_Port;
        m_RedisList[i].passwd = "";
        m_RedisList[i].poolsize = 8;
        m_RedisList[i].timeout = 5;
        m_RedisList[i].role = 0;
    }

    m_xRedisClient.Init(CACHE_TYPE_MAX);
    m_xRedisClient.ConnectRedisCache(m_RedisList, 1, CACHE_TYPE_1);
    m_RedisDBIdx = new RedisDBIdx(&m_xRedisClient);
    if (!m_RedisDBIdx)
    {
        LE("GRTSubChannel::Init new RedisDBIdx failed\n");
        if (m_RedisList)
        {
            delete m_RedisList;
            m_RedisList = nullptr;
        }
    }
}

void PRTXRedis::Unin()
{
    if (m_RedisList)
    {
        delete[] m_RedisList;
        m_RedisList = nullptr;
    }
    if (m_RedisDBIdx)
    {
         delete m_RedisDBIdx;
         m_RedisDBIdx = nullptr;
    }
    m_xRedisClient.Release();
}

bool PRTXRedis::Echo()
{
    m_RedisDBIdx->CreateDBIndex("echo", APHash, CACHE_TYPE_1);

     std::string val;
     m_xRedisClient.echo(*m_RedisDBIdx, "echo", val);
     if (val.compare("echo")==0)
     {
        return true;
     } else {
        return false;
     }
}

// redis user push setting key:'push:set:module:userid' by hmset, 'token':token, 'push':enablepush
bool PRTXRedis::SetSettingPush(const std::string& userid, int module, const std::string& field, const std::string& value)
{
    VDATA vdata;
    vdata.push_back(field);
    vdata.push_back(value);

    char key[128] = {0};
    sprintf(key, "push:set:%d:%s", module, userid.c_str());
    m_RedisDBIdx->CreateDBIndex(key, APHash, CACHE_TYPE_1);

    if (!m_xRedisClient.hmset(*m_RedisDBIdx, key, vdata))
    {
        char* err = m_RedisDBIdx->GetErrInfo();
        if (err) {
            LE("PRTXRedis::SetSettingPush m_xRedisClient.hmset err:%s\n", err);
        }
        return false;
    }
    return true;
}

bool PRTXRedis::GetSettingPush(const std::string& userid, int module, const std::string& field, std::string& value)
{
    ArrayReply reply;
    VDATA vdata;
    vdata.push_back(field);

    char key[128] = {0};
    sprintf(key, "push:set:%d:%s", module, userid.c_str());
    m_RedisDBIdx->CreateDBIndex(key, APHash, CACHE_TYPE_1);

    if (!m_xRedisClient.hmget(*m_RedisDBIdx, key, vdata, reply))
    {
        char* err = m_RedisDBIdx->GetErrInfo();
        if (err) {
            LE("PRTXRedis::StoreSettingPush m_xRedisClient.hmset err:%s\n", err);
        }
        return false;
    }
    value = reply.at(0).str;
    return true;
}

// redis push msg key:'push:msg:module:userid'
bool PRTXRedis::SetNeedPushMsg(const std::string& devType, const std::string& value)
{
    char key[128] = {0};
    sprintf(key, "push:msg:%s", devType.c_str());
    VALUES vVal;
    vVal.push_back(value);
    int64_t count = 0;
    // rpush push to the end
    m_RedisDBIdx->CreateDBIndex(key, APHash, CACHE_TYPE_1);

    if (m_xRedisClient.rpush(*m_RedisDBIdx, key, vVal, count)) {
        LI("%s success %ld \r\n", __PRETTY_FUNCTION__, count);
        return true;
    }
    else {
        LE("%s error [%s] \r\n", __PRETTY_FUNCTION__, m_RedisDBIdx->GetErrInfo());
        return false;
    }
}

bool PRTXRedis::GetNeedPushMsg(const std::string& devType, ArrayReply& reply, int64_t start, int64_t stop)
{
    char key[128] = {0};
    sprintf(key, "push:msg:%s", devType.c_str());
    if (start==stop) {
        std::string value;
        m_RedisDBIdx->CreateDBIndex(key, APHash, CACHE_TYPE_1);

        if (m_xRedisClient.lpop(*m_RedisDBIdx, key, value)) {
            LI("%s success val: %s \r\n", __PRETTY_FUNCTION__, value.c_str());
            DataItem item;
            item.type = 0; // here means string
            item.str.assign(value);
            reply.push_back(item);
            return true;
        }
        else {
            LE("%s error [%s] \r\n", __PRETTY_FUNCTION__, m_RedisDBIdx->GetErrInfo());
            return false;
        }
    } else {
        // get list msg stop-start once, and them remove them
        bool res = false;
        {
            //TODO:maybe has dead lock???
            OSMutexLocker locker(&m_MutexLPush);

            m_RedisDBIdx->CreateDBIndex(key, APHash, CACHE_TYPE_1);
            if (m_xRedisClient.lrange(*m_RedisDBIdx, key, start, stop, reply)) {
                LI("xRedisClient lrange ok~~~~~~~\n");
                m_xRedisClient.ltrim(*m_RedisDBIdx, key, stop+1, -1);
                res = true;
            } else {
                LI("xRedisClient lrange failed, start:%d, stop:%d, key:%s~~~~~~~\n", start, stop, key);
            }
        }
        return res;
    }
}

bool PRTXRedis::LenListMsg(const std::string& devType, int64_t& count)
{
    char key[128] = {0};
    sprintf(key, "push:msg:%s", devType.c_str());
    // length list

    m_RedisDBIdx->CreateDBIndex(key, APHash, CACHE_TYPE_1);
    if (m_xRedisClient.llen(*m_RedisDBIdx, key, count)) {
        LI("%s success count:%lld \r\n", __PRETTY_FUNCTION__, count);
        return true;
    }
    else {
        LE("%s error [%s] \r\n", __PRETTY_FUNCTION__, m_RedisDBIdx->GetErrInfo());
        return false;
    }
}


///////////////////////////////////////////////////////
