//
//  SRTStorageRedis.cpp
//  dyncRTStorage
//
//  Created by hp on 11/26/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#include "SRTStorageRedis.h"
#include "SRTRedisGroup.h"
#include "OSThread.h"
#include "StatusCode.h"

#include "MsgServer/proto/storage_msg_type.pb.h"
#include "MsgServer/proto/storage_msg.pb.h"
#include "MsgServer/proto/common_msg.pb.h"

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

void SRTStorageRedis::Init(SRTRedisGroup* group, const std::string& ip, int port)
{
    //LI("SRTStorageRedis::Init was called, ip:%s, port:%d\n", ip.c_str(), port);
    m_Ip = ip;
    m_Port = port;
    m_RedisGroup = group;

    m_RedisList = new RedisNode;
    m_RedisList->dbindex = 0;
    m_RedisList->host = m_Ip.c_str();
    m_RedisList->port = m_Port;
    m_RedisList->passwd = "";
    m_RedisList->poolsize = 8;
    m_RedisList->timeout = 5;
    m_RedisList->role = 0;

    m_xRedisClient.Init(CACHE_TYPE_MAX);
    m_xRedisClient.ConnectRedisCache(m_RedisList, 1, CACHE_TYPE_1);
    m_RedisDBIdx = new RedisDBIdx(&m_xRedisClient);

    for (int i=0;i<PACKED_MSG_ONCE_NUM;++i)
    {
        m_ReadedMsg.add_msgs();
    }
}

void SRTStorageRedis::Unin()
{
    m_RedisGroup = nullptr;
    if (m_RedisList)
    {
        delete m_RedisList;
        m_RedisList = nullptr;
    }
    if (m_RedisDBIdx)
    {
         delete m_RedisDBIdx;
         m_RedisDBIdx = nullptr;
    }
}

// from RTEventLooper

// for read
void SRTStorageRedis::OnWakeupEvent(const void*pData, int nSize)
{
    if (m_QueuePostMsg.size()==0) return;
    pms::StorageMsg store = m_QueuePostMsg.front();

    if (store.sequence()==store.sdmaxseqn())
    {
        //LI("SRTStorageRedis::OnWakeupEvent ruserid:%s, storeid:%s, sequence:%lld, sdmaxseqn:%lld\n", store.ruserid().c_str(), store.storeid().c_str(), store.sequence(), store.sdmaxseqn());
        // 1, clean up
        for (int i=0;i<PACKED_MSG_ONCE_NUM;++i)
        {
            m_ReadedMsg.mutable_msgs(i)->Clear();
        }
        *store.mutable_content() = "";
        // 2, get msg and store
        int times = 1;
        ProcessRead(&store, times);
        // 3, send back
        if (m_RedisGroup)
        {
            m_RedisGroup->PostData(store.SerializeAsString());
        }
    } else {
        int64 s = store.sequence(), ms = store.sdmaxseqn();
        int count = ms-s;// the msg numbers client can sync
        // because when the sequence == sdmaxseqn, this 'sequence' is the msg you wanted
        // if sequence < sdmaxseqn, this 'sequence' is the client's current sequence
        // so here sequence+1
        store.set_sequence(s+1);
        for(int j=0;j<=count;j+=PACKED_MSG_ONCE_NUM)
        {
            //LI("SRTStorageRedis::OnWakeupEvent ruserid:%s, storeid:%s, sequence:%lld, sdmaxseqn:%lld, s:%lld, ms:%lld, count:%d, j:%d\n"\
                    , store.ruserid().c_str()\
                    , store.storeid().c_str()\
                    , store.sequence()\
                    , store.sdmaxseqn()\
                    , s\
                    , ms\
                    , count\
                    , j);
            //at most, sync PACKED_MSG_ONCE_NUM msgs once, if more msgs, then sync more times
            int times = (count < PACKED_MSG_ONCE_NUM)?(count):(PACKED_MSG_ONCE_NUM);
            // 1, clean up
            for (int i=0;i<PACKED_MSG_ONCE_NUM;++i)
            {
                m_ReadedMsg.mutable_msgs(i)->Clear();
            }
            // clean last useless msg
            store.set_sequence(s+j);
            *store.mutable_content() = "";
            // 2, get msg and store
            ProcessRead(&store, times);
            //LI("SRTStorageRedis::OnWakeupEvent times:%d, store.content.length:%d\n", times, store.content().length());
            // 3, send back
            if (m_RedisGroup)
            {
                m_RedisGroup->PostData(store.SerializeAsString());
            }
        }
    }

    {
        OSMutexLocker locker(&m_MutexRecvPost);
        m_QueuePostMsg.pop();
    }
    if (m_QueuePostMsg.size()>0)
    {
        this->Signal(Task::kWakeupEvent);
    }
}

// for write
void SRTStorageRedis::OnTickEvent(const void*pData, int nSize)
{
    if (m_QueuePushMsg.size()==0) return;
    pms::StorageMsg store = m_QueuePushMsg.front();
    ProcessWrite(&store);
    if (m_RedisGroup)
    {
        m_RedisGroup->PushData(store.SerializeAsString());
    }

    {
        OSMutexLocker locker(&m_MutexRecvPush);
        m_QueuePushMsg.pop();
    }
    if (m_QueuePushMsg.size()>0)
    {
        this->Signal(Task::kIdleEvent);
    }
}

// for write
void SRTStorageRedis::PushToQueue(pms::StorageMsg request)
{
    {
        OSMutexLocker locker(&m_MutexRecvPost);
        m_QueuePushMsg.push(request);
    }
    this->Signal(Task::kIdleEvent);
}

// for read
void SRTStorageRedis::PostToQueue(pms::StorageMsg request)
{
    {
        OSMutexLocker locker(&m_MutexRecvPush);
        m_QueuePostMsg.push(request);
    }
    this->Signal(Task::kWakeupEvent);
}

void SRTStorageRedis::ConnectionDisconnected()
{
    LI("%s was called\n", __FUNCTION__);
}

///////////////////////////////////////////////////////


int SRTStorageRedis::ProcessRead(pms::StorageMsg* store, int times)
{
    for (int i=0;i<times;++i)
    {
        store->set_sequence(store->sequence()+i);// modify the sequence, which msg you sync

        std::string str("");
        char key[512] = {'\0'};
        if (store->mflag()==pms::EMsgFlag::FGROUP)
        {
            sprintf(key, "grp:%s:%lld", store->storeid().c_str(), store->sequence());
        } else if (store->mflag()==pms::EMsgFlag::FSINGLE)
        {
            sprintf(key, "sgl:%s:%lld", store->storeid().c_str(), store->sequence());
        } else {
            store->set_result(Err_Redis_Key_Not_Exist);// key is not exists
            continue;
        }

        // grp:store->d:mtype:push:sequence
        m_RedisDBIdx->CreateDBIndex(key, APHash, CACHE_TYPE_1);
        if (m_xRedisClient.exists(*m_RedisDBIdx, key))
        {
            std::string keyType;
            if (m_xRedisClient.type(*m_RedisDBIdx, key, keyType))
            {
                VDATA vdata;
                vdata.push_back("ispush");
                vdata.push_back("mtype");
                vdata.push_back("cont");

                ArrayReply reply;
                if (strcmp(keyType.c_str(), "string")==0)
                {
                    m_xRedisClient.get(*m_RedisDBIdx, key, str);
                    if (str.length()==0) {
                        store->set_result(Err_Redis_Key_Expire_Or_Not_Exist);
                    } else {
                        store->set_result(Err_Redis_Ok);
                    }
                    *store->mutable_content() = str;
                } else if (strcmp(keyType.c_str(), "hash")==0)
                {
                    if (m_xRedisClient.hmget(*m_RedisDBIdx, key, vdata, reply))
                    {
                        store->set_result(Err_Redis_Ok);
                        *store->mutable_ispush() = reply.at(0).str;
                        *store->mutable_mtype() = reply.at(1).str;
                        *store->mutable_content() = reply.at(2).str;
                    } else {
                        char* err = m_RedisDBIdx->GetErrInfo();
                        if (err) {
                            LE("SRTStorageRedis::ProcessRead hmset err:%s\n", err);
                        }
                        store->set_result(Err_Redis_Key_Expire_Or_Not_Exist);
                    }
                }
            } else {
                store->set_result(Err_Redis_Type);// key is not exists
                *store->mutable_content() = str;
            }
        } else {
            store->set_result(Err_Redis_Key_Not_Exist);// key is not exists
            *store->mutable_content() = str;
        }
        // temp add each msg to PackedStoreMsg
        store->set_conttype(pms::EStoreContType::TRAWCONT);
        m_ReadedMsg.mutable_msgs(i)->MergeFrom(*store);
        //LI("SRTStorageRedis::ProcessRead m_ReadedMsg.msgs i:%d times, str.length:%d, store.length:%d\n", i, str.length(), store->SerializeAsString().length());
    }
    // replace the real msgs to StorageMsg's content
    *store->mutable_content() = m_ReadedMsg.SerializeAsString();
    store->set_conttype(pms::EStoreContType::TPACKEDSTORE);
    //LI("SRTStorageRedis::ProcessRead m_ReadedMsg.length:%d, final store.length:%d\n", m_ReadedMsg.SerializeAsString().length(), store->SerializeAsString().length());
    return 0;
}
int SRTStorageRedis::ProcessWrite(pms::StorageMsg* store)
{
    char key[512] = {'\0'};
    if (store->mflag()==pms::EMsgFlag::FGROUP)
    {
        sprintf(key, "grp:%s:%lld", store->storeid().c_str(), store->sequence());
    } else if (store->mflag()==pms::EMsgFlag::FSINGLE)
    {
        sprintf(key, "sgl:%s:%lld", store->storeid().c_str(), store->sequence());
    } else {
        store->set_result(Err_Redis_Key_Not_Exist);// key is not exists
        return -1;
    }
    m_RedisDBIdx->CreateDBIndex(key, APHash, CACHE_TYPE_1);

    if (store->version().compare("1.0.1")==0) {
        VDATA vdata;
        vdata.push_back("ispush");
        vdata.push_back(store->ispush());
        vdata.push_back("mtype");
        vdata.push_back(store->mtype());
        vdata.push_back("cont");
        vdata.push_back(store->content());

        if (m_xRedisClient.hmset(*m_RedisDBIdx, key, vdata))
        {
            store->set_result(Err_Redis_Ok);
            m_xRedisClient.expire(*m_RedisDBIdx, key, MAX_MESSAGE_EXPIRE_TIME);
        } else {
            char* err = m_RedisDBIdx->GetErrInfo();
            if (err) {
                LE("m_xRedisClient.hmset err:%s\n", err);
            }
            store->set_result(Err_Redis_Hmset);
        }
    } else if (store->version().compare("")==0) { // else if store.version.compare
        if (m_xRedisClient.setex(*m_RedisDBIdx, key, MAX_MESSAGE_EXPIRE_TIME, store->content().c_str()))
        {
            store->set_result(Err_Redis_Ok);
        } else {
            char* err = m_RedisDBIdx->GetErrInfo();
            if (err) {
                LE("m_xRedisClient.setex err:%s\n", err);
            }
            store->set_result(Err_Redis_Setex);
            //assert(false);
        }
    } else {
        store->set_result(Err_Vsersion_Not_Support);
    }

    return 0;
}
