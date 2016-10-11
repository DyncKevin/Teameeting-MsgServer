//
//  SRTStorageManager.cpp
//  dyncRTStorage
//
//  Created by hp on 12/3/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#include "SRTStorageManager.h"
#include <assert.h>
#include <algorithm>
#include "SRTTransferSession.h"
#include "OS.h"


static long long gRecvCounter = 0;
static long long gSendCounter = 0;
static SInt64 gLastRecvTime = 0;
static SInt64 gLastSendTime = 0;

bool SRTStorageManager::RecvRequestCounter()
{
    SInt64 curTime = OS::Milliseconds();
    char buf[128] = {0};
    sprintf(buf, "recv_time:%lld:gRecvCounter:%lld\n", curTime, ++gRecvCounter);
    //fwrite(buf, 1, 128, m_RecvFile);
    //fflush(m_RecvFile);
    memset(buf, 0, 128);
    return false;
}

bool SRTStorageManager::SendResponseCounter()
{
    SInt64 curTime = OS::Milliseconds();
    char buf[128] = {0};
    sprintf(buf, "send_time:%lld:gSendCounter:%lld\n", curTime, ++gSendCounter);
    //fwrite(buf, 1, 128, m_SendFile);
    //fflush(m_SendFile);
    memset(buf, 0, 128);
    return false;
}

bool SRTStorageManager::SignalKill()
{
    return true;
}

bool SRTStorageManager::ClearAll()
{
    return true;
}
