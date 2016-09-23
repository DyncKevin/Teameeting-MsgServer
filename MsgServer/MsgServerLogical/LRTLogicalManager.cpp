//
//  LRTLogicalManager.cpp
//  dyncRTLogical
//
//  Created by hp on 12/3/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#include "LRTLogicalManager.h"
#include <assert.h>
#include <algorithm>
#include "LRTTransferSession.h"
#include "OS.h"


#define DEF_PROTO 1

#define MAX_PROCESS_MSG_ONCE 200

static long long gRecvCounter = 0;
static long long gSendCounter = 0;
static SInt64 gLastRecvTime = 0;
static SInt64 gLastSendTime = 0;

bool LRTLogicalManager::RecvRequestCounter()
{
    SInt64 curTime = OS::Milliseconds();
    char buf[128] = {0};
    sprintf(buf, "recv_time:%lld:gRecvCounter:%lld\n", curTime, ++gRecvCounter);
    memset(buf, 0, 128);
    return false;
}

bool LRTLogicalManager::SendResponseCounter()
{
    SInt64 curTime = OS::Milliseconds();
    char buf[128] = {0};
    sprintf(buf, "send_time:%lld:gSendCounter:%lld\n", curTime, ++gSendCounter);
    memset(buf, 0, 128);
    return false;
}

bool LRTLogicalManager::InsertDataWrite(LRTTransferSession* sess, pms::StorageMsg*  storeMsg)
{
    if (storeMsg->storeid().length()==0) return false;
    char id[1024] = {0};
    sprintf(id, "%s:%s", storeMsg->storeid().c_str(), storeMsg->msgid().c_str());
    TransMsgInfo info;
    info.sess = sess;
    info.smsg = *storeMsg;
    {
        OSMutexLocker locker(&m_mutexDataWrite);
        //check if this id already in
        m_dataWriteMap.insert(std::make_pair(id, info));
    }
    return true;
}

bool LRTLogicalManager::UpdateDataWrite(pms::StorageMsg** storeMsg)
{
    if ((*storeMsg)->storeid().length()==0) return false;
    char id[1024] = {0};
    sprintf(id, "%s:%s", (*storeMsg)->storeid().c_str(), (*storeMsg)->msgid().c_str());

    {
        OSMutexLocker locker(&m_mutexDataWrite);
        TransMsgInfoMapIt it = m_dataWriteMap.find(id);
        if (it!=m_dataWriteMap.end())
        {
            // store sequence, get content, send to store

            it->second.smsg.set_sequence((*storeMsg)->sequence());
            it->second.smsg.set_msgid((*storeMsg)->msgid());
            (*storeMsg)->set_mtag(it->second.smsg.mtag());
            (*storeMsg)->set_content(it->second.smsg.content());
        } else {
            LE("ERROR HERE, UpdateDataWrite msg id:%s, ruserid:%s is not here\n", (*storeMsg)->msgid().c_str(), (*storeMsg)->ruserid().c_str());
        }
    }
    return true;
}

bool LRTLogicalManager::DeleteDataWrite(pms::StorageMsg*  storeMsg)
{
    if (storeMsg->storeid().length()==0) return false;
    char id[1024] = {0};
    sprintf(id, "%s:%s", storeMsg->storeid().c_str(), storeMsg->msgid().c_str());
    {
        OSMutexLocker locker(&m_mutexDataWrite);
        TransMsgInfoMapIt it = m_dataWriteMap.find(id);
        if (it!=m_dataWriteMap.end())
        {
            // after get from store, delete this msg in map
            if (it->second.sess && it->second.sess->IsLiveSession())
            {
                it->second.sess->PushWriteMsg(*storeMsg);
            }
            m_dataWriteMap.erase(it);
        } else {
            LE("ERROR HERE, DeleteDataWrite msg id:%s, ruserid:%s is not here\n", storeMsg->msgid().c_str(), storeMsg->ruserid().c_str());
        }
    }
    return true;
}


bool LRTLogicalManager::InsertDataRead(LRTTransferSession* sess, pms::StorageMsg*  storeMsg)
{
    if (storeMsg->storeid().length()==0) return false;
    char id[1024] = {0};
    sprintf(id, "%s:%s", storeMsg->storeid().c_str(), storeMsg->msgid().c_str());
    TransMsgInfo info;
    info.sess = sess;
    info.smsg = *storeMsg;
    {
        OSMutexLocker locker(&m_mutexDataRead);
        //check if this id already in
        m_dataReadMap.insert(std::make_pair(id, info));
    }
    return true;
}

bool LRTLogicalManager::UpdateDataRead(pms::StorageMsg** storeMsg)
{
    LE("UpdateDataRead NOT implement!!!\n\n");

    return true;
}

bool LRTLogicalManager::DeleteDataRead(pms::StorageMsg*  storeMsg)
{
    if (storeMsg->storeid().length()==0) return false;
    char id[1024] = {0};
    sprintf(id, "%s:%s", storeMsg->storeid().c_str(), storeMsg->msgid().c_str());
    {
        OSMutexLocker locker(&m_mutexDataRead);
        TransMsgInfoMapIt it = m_dataReadMap.find(id);
        if (it!=m_dataReadMap.end())
        {
            // after get from store, delete this msg in map
            if (it->second.sess && it->second.sess->IsLiveSession())
            {
                it->second.sess->PushReadMsg(*storeMsg);
            }
            m_dataReadMap.erase(it);
        } else {
            LE("ERROR HERE, DeleteDataRead msg id:%s, ruserid:%s is not here\n", storeMsg->msgid().c_str(), storeMsg->ruserid().c_str());
        }
    }
    return true;
}


bool LRTLogicalManager::InsertSeqnRead(LRTTransferSession* sess, pms::StorageMsg*  storeMsg)
{
    if (storeMsg->storeid().length()==0) return false;
    char id[1024] = {0};
    sprintf(id, "%s:%s", storeMsg->storeid().c_str(), storeMsg->msgid().c_str());
    TransMsgInfo info;
    info.sess = sess;
    info.smsg = *storeMsg;
    {
        OSMutexLocker locker(&m_mutexSeqnRead);
        //check if this id already in
        m_seqnReadMap.insert(std::make_pair(id, info));
    }
    return true;
}

bool LRTLogicalManager::UpdateSeqnRead(pms::StorageMsg** storeMsg)
{
    LE("UpdateSeqnRead NOT implement!!!\n\n");
    return true;

}

bool LRTLogicalManager::DeleteSeqnRead(pms::StorageMsg*  storeMsg)
{
    if (storeMsg->storeid().length()==0) return false;
    char id[1024] = {0};
    sprintf(id, "%s:%s", storeMsg->storeid().c_str(), storeMsg->msgid().c_str());
    {
        OSMutexLocker locker(&m_mutexSeqnRead);
        TransMsgInfoMapIt it = m_seqnReadMap.find(id);
        if (it!=m_seqnReadMap.end())
        {
            // after get from store, delete this msg in map
            if (it->second.sess && it->second.sess->IsLiveSession())
            {
                it->second.sess->PushReadMsg(*storeMsg);
            }
            m_seqnReadMap.erase(it);
        } else {
            LE("ERROR HERE, DeleteSeqnRead ruserid:%s is not here\n", id);
        }
    }
    return true;
}

bool LRTLogicalManager::GetSessFromId(const std::string& ruserid, const std::string& msgid, LRTTransferSession** sess)
{
    assert(ruserid.length()>0);
    assert(msgid.length()>0);
    assert(sess);
    char id[1024] = {0};
    sprintf(id, "%s:%s", ruserid.c_str(), msgid.c_str());
    {
        OSMutexLocker locker(&m_mutexSeqnRead);
        TransMsgInfoMapIt it = m_seqnReadMap.find(id);
        if (it!=m_seqnReadMap.end())
        {
            *sess = it->second.sess;
        } else {
            LE("ERROR HERE, GetSessFromMsg ruserid:%s is not here\n", id);
        }
    }
    return true;
}

bool LRTLogicalManager::ReadLocalSeqn(pms::StorageMsg*  storeMsg, int64* seqn)
{
    if (storeMsg->storeid().length()==0) return false;
    bool found = false;
    {
        OSMutexLocker locker(&m_mutexLocalSeqn);
        UserLocalSeqnMapIt it = m_localSeqnMap.find(storeMsg->storeid());
        if (it!=m_localSeqnMap.end())
        {
            *seqn = it->second;
            found = true;
        } else {
            found = false;
        }
    }
    return found;
}

bool LRTLogicalManager::UpdateLocalSeqn(pms::StorageMsg*  storeMsg)
{
    if (storeMsg->storeid().length()==0) return false;
    {
        OSMutexLocker locker(&m_mutexLocalSeqn);
        UserLocalSeqnMapIt it = m_localSeqnMap.find(storeMsg->storeid());
        if (it!=m_localSeqnMap.end())
        {
            //assert(storeMsg->sequence() >= it->second);
            if (storeMsg->sequence()>it->second)
                it->second = storeMsg->sequence();
        } else {
            m_localSeqnMap.insert(std::make_pair(storeMsg->storeid(), storeMsg->sequence()));
        }
    }
    return true;
}

bool LRTLogicalManager::UpdateLocalMaxSeqn(pms::StorageMsg*  storeMsg)
{
    if (storeMsg->storeid().length()==0) return false;
    {
        OSMutexLocker locker(&m_mutexLocalSeqn);
        UserLocalSeqnMapIt it = m_localSeqnMap.find(storeMsg->storeid());
        if (it!=m_localSeqnMap.end())
        {
            //assert(storeMsg->maxseqn() >= it->second);
            if (storeMsg->maxseqn()>it->second)
                it->second = storeMsg->maxseqn();
        } else {
            m_localSeqnMap.insert(std::make_pair(storeMsg->storeid(), storeMsg->maxseqn()));
        }
    }
    return true;
}

bool LRTLogicalManager::SignalKill()
{
    return true;
}

bool LRTLogicalManager::ClearAll()
{
    return true;
}
