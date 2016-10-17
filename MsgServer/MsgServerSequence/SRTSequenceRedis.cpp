//
//  SRTSequenceRedis.cpp
//  dyncRTSequence
//
//  Created by hp on 11/26/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#include "SRTSequenceRedis.h"
#include "SRTRedisManager.h"
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

void SRTSequenceRedis::Init(SRTRedisManager* manager, const std::string& ip, int port)
{
    //LI("SRTSequenceRedis::Init was called, ip:%s, port:%d\n", ip.c_str(), port);

    m_Ip = ip;
    m_Port = port;
    m_RedisManager = manager;

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

    WriteResponse.connect(m_RedisManager, &SRTRedisManager::OnWriteSeqn);
    ReadResponse.connect(m_RedisManager, &SRTRedisManager::OnReadSeqn);
}

void SRTSequenceRedis::Unin()
{
    //LI("SRTSequenceRedis::Unin was called, host:%s\n", GetHostForTest().c_str());
    ReadResponse.disconnect(m_RedisManager);
    WriteResponse.disconnect(m_RedisManager);
    m_RedisManager = nullptr;
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
    m_xRedisClient.Release();
}

// from RTEventLooper
// post for read
void SRTSequenceRedis::OnPostEvent(const char*pData, int nSize)
{
    LI("SRTSequenceRedis::OnPostEvent for read, nsize:%d\n", nSize);
    if (!pData || nSize<=0) return;
    std::string str(pData, nSize);
    pms::StorageMsg request;
    if (!request.ParseFromString(str)) return;
    {
        if (request.mflag()==pms::EMsgFlag::FGROUP)
        {
			LI("SRTSequenceRedis::OnPostEvent for read group, nsize:%d\n", nSize);
            std::string str("");
            char key[512] = {'\0'};
            sprintf(key, "grp:%s", request.storeid().c_str());
            switch (request.tsvrcmd())
            {
                case pms::EServerCmd::CSYNCSEQN:
                case pms::EServerCmd::CSSEQN4DATA:
                    {
                        if (m_xRedisClient.exists(*m_RedisDBIdx, key))
                        {
                            int64 seq = 0;
                            m_RedisDBIdx->CreateDBIndex(key, APHash, CACHE_TYPE_1);
                            if (m_xRedisClient.get(*m_RedisDBIdx, key, str))
                            {
								LI("SRTSequenceRedis::OnPostEvent g read seqn key:%s ok\n", key);
                                seq = std::stoll(str);
                                request.set_result(Err_Redis_Ok);
                                ReadResponse(request, seq);
                            } else {
                                LE("SRTSequenceRedis::OnPostEvent g read seqn: get error, key:%s\n", key);
                            }
                        } else {
#if 0
                            LI("SRTSequenceRedis::OnPostEvent g read seqn key:%s not exists, do nothing\n", key);
                            request.set_result(Err_Redis_Key_Not_Exist); // groupid not exists
                            ReadResponse(request, 0);
#else
                            LI("SRTSequenceRedis::OnPostEvent g read seqn key:%s not exists, do nothing\n", key);
                            m_RedisDBIdx->CreateDBIndex(key, APHash, CACHE_TYPE_1);
                            m_xRedisClient.set(*m_RedisDBIdx, key, "0");
                            request.set_result(Err_Redis_Ok);
                            LI("SRTSequenceRedis::OnPostEvent g read seqn key not exists, so set 0\n", key);
                            ReadResponse(request, 0);
#endif
                        }
                    }
                    break;
                default:
                    {
                        LI("SRTSequenceRedis::OnPostEvent not handled flag:%d, cmd:%d, tcmd:%d,  ruserid:%s, storeid:%s, key:%s\n"\
                                , request.mflag()\
                                , request.rsvrcmd()\
                                , request.tsvrcmd()\
                                , request.ruserid().c_str()\
                                , request.storeid().c_str()\
                                , key);
                    }
                    break;
            }
        } else if (request.mflag()==pms::EMsgFlag::FSINGLE)
        {
            std::string str("");
            char key[512] = {'\0'};
            sprintf(key, "sgl:%s", request.storeid().c_str());
            switch (request.tsvrcmd())
            {
                case pms::EServerCmd::CSYNCSEQN:
                case pms::EServerCmd::CSSEQN4DATA:
                    {
                        if (m_xRedisClient.exists(*m_RedisDBIdx, key))
                        {
                            int64 seq = 0;
                            m_RedisDBIdx->CreateDBIndex(key, APHash, CACHE_TYPE_1);
                            if (m_xRedisClient.get(*m_RedisDBIdx, key, str))
                            {
                                seq = std::stoll(str);
                                request.set_result(Err_Redis_Ok);
                                ReadResponse(request, seq);
                            } else {
                                LE("SRTSequenceRedis::OnPostEvent s read seqn: get error, key:%s\n", key);
                            }
                        } else {
                            m_RedisDBIdx->CreateDBIndex(key, APHash, CACHE_TYPE_1);
                            m_xRedisClient.set(*m_RedisDBIdx, key, "0");
                            request.set_result(Err_Redis_Ok);
                            ReadResponse(request, 0);
                        }
                    }
                    break;
                default:
                    {
                        LI("SRTSequenceRedis::OnPostEvent not handled flag:%d, cmd:%d, tcmd:%d, ruserid:%s, storeid:%s, key:%s\n"\
                                , request.mflag()\
                                , request.rsvrcmd()\
                                , request.tsvrcmd()\
                                , request.ruserid().c_str()\
                                , request.storeid().c_str()\
                                , key);
                    }
                    break;
            }

        }
    }
}

// push for write
void SRTSequenceRedis::OnPushEvent(const char*pData, int nSize)
{
    if (!pData || nSize<=0)
    {
        LE("SRTSequenceRedis::OnPushEvent pData is null or nSize:%d", nSize);
        return;
    }
    LI("SRTSequenceRedis::OnPushEvent for write, nsize:%d\n", nSize);
    std::string str(pData, nSize);
    pms::StorageMsg request;
    if (!request.ParseFromString(str)) return;
    {
        if (request.mflag()==pms::EMsgFlag::FGROUP)
        {
            char key[512] = {'\0'};
            sprintf(key, "grp:%s", request.storeid().c_str());
            switch (request.tsvrcmd())
            {
                case pms::EServerCmd::CNEWMSGSEQN:
                    {
#if 0
                        if (m_xRedisClient.exists(*m_RedisDBIdx, key))
                        {
                            int64_t seq = 0;
                            m_RedisDBIdx->CreateDBIndex(key, APHash, CACHE_TYPE_1);
                            m_xRedisClient.incr(*m_RedisDBIdx, key, seq);
                            LI("OnPushEvent group IncrSeq is:%ld\n", seq);
                            request.set_result(Err_Redis_Ok);
                            WriteResponse(request, seq);
                        } else {
                            LI("SRTSequenceRedis::OnPushEvent group newmsg seqn key is not exists!, error\n");
                            request.set_result(Err_Redis_Key_Not_Exist);// group id not exists
                            WriteResponse(request, 0);
                        }
#else
                        int64_t seq = 0;
                        m_RedisDBIdx->CreateDBIndex(key, APHash, CACHE_TYPE_1);
                        m_xRedisClient.incr(*m_RedisDBIdx, key, seq);
                        request.set_result(Err_Redis_Ok);
                        WriteResponse(request, seq);
#endif
                    }
                    break;
                case pms::EServerCmd::CCREATESEQN:
                    {
                        if (!m_xRedisClient.exists(*m_RedisDBIdx, key))
                        {
                            m_RedisDBIdx->CreateDBIndex(key, APHash, CACHE_TYPE_1);
                            m_xRedisClient.set(*m_RedisDBIdx, key, "0");
                            request.set_result(Err_Redis_Ok);
                            WriteResponse(request, 0);
                        } else {
                            request.set_result(Err_Redis_Key_Exist); // groupid already exists
                            WriteResponse(request, 0);
                        }
                    }
                    break;
                case pms::EServerCmd::CDELETESEQN:
                    {
                        m_RedisDBIdx->CreateDBIndex(key, APHash, CACHE_TYPE_1);
                        m_xRedisClient.del(*m_RedisDBIdx, key);
                        request.set_result(Err_Redis_Ok);
                        WriteResponse(request, 0);
                    }
                    break;
                default:
                    {
                        LI("SRTSequenceRedis::OnPushEvent not handled flag:%d, cmd:%d, tcmd:%d, ruserid:%s, storeid:%s, key:%s\n"\
                                , request.mflag()\
                                , request.rsvrcmd()\
                                , request.tsvrcmd()\
                                , request.ruserid().c_str()\
                                , request.storeid().c_str()\
                                , key);
                    }
                    break;
            }
        } else if (request.mflag()==pms::EMsgFlag::FSINGLE)
        {
            char key[512] = {'\0'};
            sprintf(key, "sgl:%s", request.storeid().c_str());
            switch (request.tsvrcmd())
            {
                case pms::EServerCmd::CNEWMSGSEQN:
                    {
                        int64_t seq = 0;
                        m_RedisDBIdx->CreateDBIndex(key, APHash, CACHE_TYPE_1);
                        m_xRedisClient.incr(*m_RedisDBIdx, key, seq);
                        request.set_result(Err_Redis_Ok);
                        WriteResponse(request, seq);
                    }
                    break;
                default:
                    {
                        LI("SRTSequenceRedis::OnPushEvent not handled flag:%d, cmd:%d, tcmd:%d, ruserid:%s, storeid:%s, key:%s\n"\
                                , request.mflag()\
                                , request.rsvrcmd()\
                                , request.tsvrcmd()\
                                , request.ruserid().c_str()\
                                , request.storeid().c_str()\
                                , key);
                    }
                    break;
            }
        } else {
            LI("SRTSequenceRedis::OnPushEvent mflag:%d not handled!!!!\n", request.mflag());
        }
    }
}

void SRTSequenceRedis::ConnectionDisconnected()
{
     LI("%s was called\n", __FUNCTION__);
}

///////////////////////////////////////////////////////
