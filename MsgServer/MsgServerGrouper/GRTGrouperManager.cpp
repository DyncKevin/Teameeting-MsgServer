//
//  GRTGrouperManager.cpp
//  dyncRTGrouper
//
//  Created by hp on 12/3/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#include "GRTGrouperManager.h"
#include "GRTTransferSession.h"
#include <assert.h>
#include <algorithm>
#include "OS.h"


#define MAX_PROCESS_MSG_ONCE 200

static long long gRecvCounter = 0;
static long long gSendCounter = 0;

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


bool GRTGrouperManager::RecvRequestCounter()
{
    SInt64 curTime = OS::Milliseconds();
    char buf[128] = {0};
    sprintf(buf, "recv_time:%lld:gRecvCounter:%lld\n", curTime, ++gRecvCounter);
    memset(buf, 0, 128);
    return false;
}

bool GRTGrouperManager::SendResponseCounter()
{
    SInt64 curTime = OS::Milliseconds();
    char buf[128] = {0};
    sprintf(buf, "send_time:%lld:gSendCounter:%lld\n", curTime, ++gSendCounter);
    memset(buf, 0, 128);
    return false;
}

bool GRTGrouperManager::InitManager(const std::string& ip, int port)
{
    m_Ip = ip;
    m_Port = port;
    m_RedisNodeNum = 3;

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

    return true;
}

bool GRTGrouperManager::UninManager()
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
    return true;
}

bool GRTGrouperManager::GetGroupMembersRedis(const std::string& groupid)
{
    char grpKey[256] = {0};
    sprintf(grpKey, "anyrtc_live_%s", groupid.c_str());
    m_RedisDBIdx->CreateDBIndex(grpKey, APHash, CACHE_TYPE_1);
    VALUES vValue;
    if (m_xRedisClient.smembers(*m_RedisDBIdx, grpKey, vValue))
    {
        LE("GroupMembers num:%lu\n", vValue.size());
        Assert(m_MapGroupMembers.count(groupid)==0);
        std::unordered_set<std::string>* uset = new std::unordered_set<std::string>();
        for(std::string s : vValue)
        {
            LI("store insert GroupMembers:%s\n", s.c_str());
            uset->insert(s);
        }
        {
            //OSMutexLocker locker(&m_MutexGroupMembers);
            // here it modify m_MapGroupMembers so no need lock
            m_MapGroupMembers.insert(make_pair(groupid, uset));
        }
        return true;
    } else {
        LE("GetGroupMembers key:%s failed\n", grpKey);
        return false;
    }
}

void GRTGrouperManager::UpdateGroupMembers(const std::string& groupid, const GrpMembersStatusMap&  members)
{
    // add new members, del old members
    GroupMembersMapIt it = m_MapGroupMembers.find(groupid);
    if (it != m_MapGroupMembers.end())
    {
        OSMutexLocker locker(&m_MutexGroupMembers);
        for (auto & x : members)
        {
            if (x.second == ADD) {
                (*it).second->insert(x.first);
            } else if (x.second == DEL) {
                (*it).second->erase(x.first);
            }
        }
    } else {
        if (GetGroupMembersRedis(groupid))
        {
            GroupMembersMapIt it = m_MapGroupMembers.find(groupid);
            //OSMutexLocker locker(&m_MutexGroupMembers);
            // here it modify it->second so no need lock
            for (auto & x : members)
            {
                if (x.second == ADD) {
                    (*it).second->insert(x.first);
                } else if (x.second == DEL) {
                    (*it).second->erase(x.first);
                }
            }
        } else {
            LE("GRTGrouperManager::UpdateGroupMembers GetGroupMembersRedis not find groupid:%s\n", groupid.c_str());
        }
    }
}

void GRTGrouperManager::GetGroupMembersLocal(const std::string& groupid, std::unordered_set<std::string>** uset)
{
    if (!uset) return;
    GroupMembersMapIt it = m_MapGroupMembers.find(groupid);
    if (it != m_MapGroupMembers.end())
    {
         *uset = (*it).second;
    } else {
        *uset = NULL;
    }
}

bool GRTGrouperManager::FindTmpGroupId(const std::string& groupid)
{
    return ((m_MapTmpGroupMsg.find(groupid)!=m_MapTmpGroupMsg.end())?true:false);
}

void GRTGrouperManager::TmpStoreGroupMsg(const std::string& groupid, int64 seqn, bool exist)
{
    OSMutexLocker locker(&m_MutexTmpGroupMsg);
    if (exist) {
        m_MapTmpGroupMsg.find(groupid)->second = seqn;
    } else {
        m_MapTmpGroupMsg.insert(make_pair(groupid, seqn));
        m_ListTmpGroup.push_back(groupid);
        //Assert(m_MapTmpGroupMsg.size()==m_ListTmpGroup.size());
    }
}

void GRTGrouperManager::RemoveGroupMsg(const std::string& groupid)
{
    {
        OSMutexLocker locker(&m_MutexTmpGroupMsg);
        m_MapTmpGroupMsg.erase(groupid);
        m_ListTmpGroup.pop_front();
    }
    //Assert(m_MapTmpGroupMsg.size()==m_ListTmpGroup.size());
}

bool GRTGrouperManager::ProcessTmpGroupMsg(GRTTransferSession* pSess)
{
    if (m_ListTmpGroup.size()==0) return false;
    std::string grpid = m_ListTmpGroup.front();
    if (GetGroupMembersRedis(grpid))
    {
        int64 seqn = m_MapTmpGroupMsg[grpid];
        std::unordered_set<std::string>* users = m_MapGroupMembers[grpid];
        for(auto & l : *users)
        {
           pSess->GenGrpSyncDataNotify(l, grpid, seqn);
        }
    }
    RemoveGroupMsg(grpid);
    return ((m_ListTmpGroup.size()>0)?true:false);
}



bool GRTGrouperManager::SignalKill()
{
    return true;
}

bool GRTGrouperManager::ClearAll()
{
    return true;
}
