//
//  SRTSequenceGenerator.h
//  MsgServerSequence
//
//  Created by hp on 11/26/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#ifndef __MsgServerSequence__SRTSequenceGenerator__
#define __MsgServerSequence__SRTSequenceGenerator__

#include <stdio.h>
#include <iostream>
#include <vector>
#include "RTEventLooper.h"
#include "SRTRedisManager.h"
#include "RTObserverConnection.h"

#include "ProtoCommon.h"

#define REQUEST_DATA_SIZE (int(sizeof(SRTSequenceGenerator::RequestData)))

class SRTTransferSession;

class SRTSequenceGenerator : public RTEventLooper, public RTObserverConnection{
public:
    SRTSequenceGenerator();
    virtual ~SRTSequenceGenerator();

    void Init(SRTTransferSession* sess);
    void Unin();

public:


// from RTEventLooper
public:
    virtual void OnPostEvent(const char*pData, int nSize);
    virtual void OnSendEvent(const void*pData, int nSize) {}
    virtual void OnWakeupEvent(const void*pData, int nSize){}
    virtual void OnPushEvent(const char*pData, int nSize);
    virtual void OnTickEvent(const void*pData, int nSize) {}
    virtual void OnRedisEvent(const char*pData, int nSize) {}
    virtual void OnErrorEvent(const char*pData, int nSize) {}

// from RTObserverConnection
public:
    virtual void ConnectionDisconnected();
private:
    long long mSendResponse;
    SRTTransferSession*                 m_Session;
    std::vector<std::string>            m_RedisHosts;
    SInt64                              m_LastUpdateTime;
    SRTRedisManager                     m_RedisManager;
};

#endif /* defined(__MsgServerSequence__SRTSequenceGenerator__) */
