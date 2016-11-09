//
//  DRTMsgDispatch.hpp
//  MsgServer
//
//  Created by hp on 2/18/16.
//  Copyright © 2016 DYNC. All rights reserved.
//

#ifndef DRTMsgDispatch_hpp
#define DRTMsgDispatch_hpp

#include <stdio.h>
#include "RTDispatch.h"
#include "RTObserverConnection.h"

#define DEF_PROTO 1

class DRTMsgDispatch
    : public RTDispatch
    , public RTObserverConnection{
public:
    DRTMsgDispatch();
    virtual ~DRTMsgDispatch();

public:
    //for RTDispatch
    virtual void OnRecvEvent(const char*pData, int nLen) {}
    virtual void OnSendEvent(const char*pData, int nLen);
    virtual void OnWakeupEvent(const char*pData, int nLen) {}
    virtual void OnPushEvent(const char*pData, int nLen);
    virtual void OnTickEvent(const char*pData, int nLen) {}
    virtual void OnRedisEvent(const char*pData, int nLen) {}
    virtual void OnErrorEvent(const char*pData, int nLen) {}

// from RTObserverConnection
public:
    virtual void ConnectionDisconnected();
};

#endif /* DRTMsgDispatch_hpp */
