//
//  RTXRedis.cpp
//  dyncRTStorage
//
//  Created by hp on 11/26/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#include "RTXRedis.h"
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

void RTXRedis::Init(const std::string& ip, int port)
{
    //LI("RTXRedis::Init was called, ip:%s, port:%d\n", ip.c_str(), port);
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
}

void RTXRedis::Unin()
{
    if (m_RedisList)
    {
        delete[] m_RedisList;
        m_RedisList = nullptr;
    }
    m_xRedisClient.Release();
}

bool RTXRedis::Echo()
{
     std::string val("");
     char k[64] = {0};
     sprintf(k, "%s", "echo");

    RedisDBIdx dbi(&m_xRedisClient);
    bool bRet = dbi.CreateDBIndex(k, APHash, CACHE_TYPE_1);
    if (bRet) {
        m_xRedisClient.echo(dbi, k, val);
        if (val.compare("echo")==0)
        {
           return true;
        } else {
           return false;
        }
    } else {
         return false;
    }
}

bool RTXRedis::KeyExists(const std::string& key)
{
    char k[64] = {0};
    sprintf(k, "%s", key.c_str());

    RedisDBIdx dbi(&m_xRedisClient);
    bool bRet = dbi.CreateDBIndex(k, APHash, CACHE_TYPE_1);
    if (bRet) {
        return m_xRedisClient.exists(dbi, k);
    } else {
         return false;
    }
}

// redis rpush
bool RTXRedis::ListRPush(const std::string& key, const std::string& value, int64_t& count)
{
    VALUES vVal;
    vVal.push_back(value);
    // rpush push to the end
    char k[64] = {0};
    sprintf(k, "%s", key.c_str());

    RedisDBIdx dbi(&m_xRedisClient);
    bool bRet = dbi.CreateDBIndex(k, APHash, CACHE_TYPE_1);
    if (bRet) {
        if (m_xRedisClient.rpush(dbi, k, vVal, count)) {
            LI("%s success %ld \r\n", __PRETTY_FUNCTION__, count);
            return true;
        }
        else {
            if (dbi.GetErrInfo())
                LE("%s error [%s] \r\n", __PRETTY_FUNCTION__, dbi.GetErrInfo());
            else
                LE("%s error [return false] \r\n", __PRETTY_FUNCTION__);
            return false;
        }
    } else {
        return false;
    }
}

// redis lpush
bool RTXRedis::ListLPush(const std::string& key, const std::string& value, int64_t& count)
{
    VALUES vVal;
    vVal.push_back(value);
    // lpush push to the front
    char k[64] = {0};
    sprintf(k, "%s", key.c_str());

    RedisDBIdx dbi(&m_xRedisClient);
    bool bRet = dbi.CreateDBIndex(k, APHash, CACHE_TYPE_1);
    if (bRet) {
        if (m_xRedisClient.lpush(dbi, k, vVal, count)) {
            LI("%s success %ld \r\n", __PRETTY_FUNCTION__, count);
            return true;
        }
        else {
            if (dbi.GetErrInfo())
                LE("%s error [%s] \r\n", __PRETTY_FUNCTION__, dbi.GetErrInfo());
            else
                LE("%s error [return false] \r\n", __PRETTY_FUNCTION__);
            return false;
        }
    } else {
         return false;
    }
}

bool RTXRedis::ListLPop(const std::string& key, std::string& reply)
{
    char k[64] = {0};
    sprintf(k, "%s", key.c_str());

    RedisDBIdx dbi(&m_xRedisClient);
    bool bRet = dbi.CreateDBIndex(k, APHash, CACHE_TYPE_1);
    if (bRet) {
        if (m_xRedisClient.lpop(dbi, k, reply)) {
            LI("%s success val.length: %d \r\n", __PRETTY_FUNCTION__, reply.length());
            return true;
        }
        else {
            if (dbi.GetErrInfo())
                LE("%s error [%s] \r\n", __PRETTY_FUNCTION__, dbi.GetErrInfo());
            else
                LE("%s error [return false] \r\n", __PRETTY_FUNCTION__);
            return false;
        }
    } else {
         return false;
    }
}

bool RTXRedis::ListRPop(const std::string& key, std::string& reply)
{
    char k[64] = {0};
    sprintf(k, "%s", key.c_str());

    RedisDBIdx dbi(&m_xRedisClient);
    bool bRet = dbi.CreateDBIndex(k, APHash, CACHE_TYPE_1);
    if (bRet) {
        if (m_xRedisClient.rpop(dbi, k, reply)) {
            LI("%s success val.length: %d \r\n", __PRETTY_FUNCTION__, reply.length());
            return true;
        }
        else {
            if (dbi.GetErrInfo())
                LE("%s error [%s] \r\n", __PRETTY_FUNCTION__, dbi.GetErrInfo());
            else
                LE("%s error [return false] \r\n", __PRETTY_FUNCTION__);
            return false;
        }
    } else {
         return false;
    }
}

bool RTXRedis::ListLLen(const std::string& key, int64_t& count)
{
    // list length
    char k[64] = {0};
    sprintf(k, "%s", key.c_str());

    RedisDBIdx dbi(&m_xRedisClient);
    bool bRet = dbi.CreateDBIndex(k, APHash, CACHE_TYPE_1);
    if (bRet) {
        if (m_xRedisClient.llen(dbi, k, count)) {
            LI("%s success count:%lld \r\n", __PRETTY_FUNCTION__, count);
            return true;
        }
        else {
            if (dbi.GetErrInfo())
                LE("%s error [%s] \r\n", __PRETTY_FUNCTION__, dbi.GetErrInfo());
            else
                LE("%s error [return false] \r\n", __PRETTY_FUNCTION__);
            return false;
        }
    } else {
         return false;
    }
}


///////////////////////////////////////////////////////
