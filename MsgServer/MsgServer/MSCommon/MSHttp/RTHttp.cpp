//
//  RTHttp.cpp
//  dyncRTMeeting
//
//  Created by hp on 12/16/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#include "RTHttp.h"
#include "atomic.h"

unsigned int RTHttp::sHttpSessionIDCounter = kFirstHttpSessionID;

RTHttp::RTHttp()
: Task()
, fTickTime(0)
{
    ListZero(&m_listSend);
    fSessionID = (UInt32)atomic_add(&sHttpSessionIDCounter, 1);
}


RTHttp::~RTHttp()
{
    ListEmpty(&m_listSend);
}

int RTHttp::SendData(const char*pData, int nLen)
{
    if (nLen > 9999) {
        LE("RTHttp::SendData pData is over length:%d\n", nLen);
    }
    {
        char* ptr = (char*)malloc(sizeof(char)*(nLen+1));
        memcpy(ptr, pData, nLen);
        ptr[nLen] = '\0';
        {
            OSMutexLocker locker(&mMutexSend);
            ListAppend(&m_listSend, ptr, nLen);
        }
    }

    this->Signal(kWriteEvent);
    return nLen;
}

SInt64 RTHttp::Run()
{
    EventFlags events = this->GetEvents();
    this->ForceSameThread();

    // Http session is short connection, need to kill session when occur TimeoutEvent.
    // So return -1.
    if(events&Task::kKillEvent)
    {
        ObserverConnectionMapIt it = m_mapConnectObserver.find(this);
        if (it != m_mapConnectObserver.end()) {
            RTObserverConnection *conn = it->second;
            if (conn) {
                conn->ConnectionDisconnected();
            }
        }
        return -1;
    }

    while(true)
    {
        if(events&Task::kReadEvent)
        {
            OnReadEvent(NULL, 0);
            events -= Task::kReadEvent;
        }
        else if(events&Task::kWriteEvent)
        {
            ListElement *elem = NULL;
            if((elem = m_listSend.first) != NULL)
            {
                int theLengthSent = 0;
                int err = OnWriteEvent((char*)elem->content, elem->size, &theLengthSent);
                if (!err)
                {
                    {
                        OSMutexLocker locker(&mMutexSend);
                        ListRemoveHead(&m_listSend);
                    }
                    if(NULL != m_listSend.first) {
                        this->Signal(kWriteEvent);
                    }
                }
            }
            events -= Task::kWriteEvent;
        }
        else if(events&Task::kIdleEvent)
        {
            OnTickEvent();
            events -= Task::kIdleEvent;
        }
        else
        {
            return fTickTime;
        }
    }

    // If we are here because of a timeout, but we can't delete because someone
    // is holding onto a reference to this session, just reschedule the timeout.
    //
    // At this point, however, the session is DEAD.
    return 0;
}

void RTHttp::AddObserver(RTObserverConnection* conn)
{
    m_OCMItPair = m_mapConnectObserver.insert(std::make_pair(this, conn));
    if (!m_OCMItPair.second) {
        m_mapConnectObserver.erase(this);
        m_mapConnectObserver.insert(std::make_pair(this, conn));
    }
}

void RTHttp::DelObserver(RTObserverConnection* conn)
{
    m_mapConnectObserver.erase(this);
}
