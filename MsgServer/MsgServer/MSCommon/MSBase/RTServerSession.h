//
//  RTServerSession.h
//  MsgServer
//
//  Created by hp on 11/26/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#ifndef __MsgServer__RTServerSession__
#define __MsgServer__RTServerSession__

#include <stdio.h>
#include <iostream>
#include <vector>
#include "RTEventLooper.h"
#include "RTObserverConnection.h"
#include "RTServerLauncher.h"

#include "ProtoCommon.h"

class RTServerSession : public RTEventLooper, public RTObserverConnection{
public:
    RTServerSession();
    virtual ~RTServerSession();

    void Init();
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
    void NodePath2Shell(const std::string& path, std::string& shell);

private:
    RTServerLauncher           mServerLauncher;
};

#endif /* defined(__MsgServer__RTServerSession__) */
