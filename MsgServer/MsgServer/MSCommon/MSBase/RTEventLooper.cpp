#include "RTEventLooper.h"
#include "atomic.h"


RTEventLooper::RTEventLooper()
: Task()
, fTimeoutTask(NULL, 120 * 1000)
, fTickTime(0)
{
	ListZero(&m_listRecv);
	ListZero(&m_listSend);
	ListZero(&m_listWakeup);
	ListZero(&m_listPush);

	fTimeoutTask.SetTask(this);
}

RTEventLooper::~RTEventLooper(void)
{
	ListEmpty(&m_listPush);
	ListEmpty(&m_listWakeup);
	ListEmpty(&m_listSend);
	ListEmpty(&m_listRecv);
}

int RTEventLooper::PostData(const char*pData, int nSize)
{
    {
        OSMutexLocker locker(&mMutexRecv);
        char* ptr = (char*)malloc(sizeof(char)*(nSize+1));
        memcpy(ptr, pData, nSize);
        ptr[nSize] = '\0';
        {
            ListAppend(&m_listRecv, ptr, nSize);
        }
    }

	this->Signal(kReadEvent);
	return nSize;
}

int RTEventLooper::SendData(const void*pData, int nSize)
{
    {
        OSMutexLocker locker(&mMutexSend);
        void* ptr = (void*)malloc(sizeof(char)*(nSize));
        memcpy(ptr, pData, nSize);
        {
            ListAppend(&m_listSend, ptr, nSize);
        }
    }

	this->Signal(kWriteEvent);
	return nSize;
}

int RTEventLooper::PushData(const char*pData, int nSize)
{
    {
        OSMutexLocker locker(&mMutexPush);
        char* ptr = (char*)malloc(sizeof(char)*(nSize+1));
        memcpy(ptr, pData, nSize);
        ptr[nSize] = '\0';
        {
            ListAppend(&m_listPush, ptr, nSize);
        }
    }

    this->Signal(kPushEvent);
    return nSize;
}

void RTEventLooper::NotifyRedis()
{
     this->Signal(kRedisEvent);
}

SInt64 RTEventLooper::Run()
{
	EventFlags events = this->GetEvents();
	this->ForceSameThread();

	// Http session is short connection, need to kill session when occur TimeoutEvent.
	// So return -1.
    if (events&Task::kTimeoutEvent) {
        UpdateTimer();
		return 0;
    }
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

	while(1)
	{
		if(events&Task::kReadEvent)
		{
            ListElement *elem = NULL;
            if((elem = m_listRecv.first) != NULL)
            {
                OnPostEvent((const char*)elem->content, elem->size);
                {
                    OSMutexLocker locker(&mMutexRecv);
                    ListRemoveHead(&m_listRecv);
                }
                if(NULL != m_listRecv.first)
                    this->Signal(kReadEvent);
            }
			events -= Task::kReadEvent;
		}
		else if(events&Task::kWriteEvent)
		{
            ListElement *elem = NULL;
            if((elem = m_listSend.first) != NULL)
            {
                OnSendEvent(elem->content, elem->size);
                {
                    OSMutexLocker locker(&mMutexSend);
                    ListRemoveHead(&m_listSend);
                }
                if(NULL != m_listSend.first)
                    this->Signal(kWriteEvent);
            }
			events -= Task::kWriteEvent;
		}
        else if(events&Task::kRedisEvent)
        {
            OnRedisEvent("", 0);
            events -= Task::kRedisEvent;
        }
		else if(events&Task::kWakeupEvent)
		{
            OnWakeupEvent(NULL, 0);
			events -= Task::kWakeupEvent;
		}
		else if(events&Task::kPushEvent)
		{
            ListElement *elem = NULL;
            if((elem = m_listPush.first) != NULL)
            {
                OnPushEvent((const char*)elem->content, elem->size);
                {
                    OSMutexLocker locker(&mMutexPush);
                    ListRemoveHead(&m_listPush);
                }
                if(NULL != m_listPush.first)
                    this->Signal(kPushEvent);
            }
			events -= Task::kPushEvent;
		}
		else if(events&Task::kIdleEvent)
		{
			OnTickEvent(NULL, 0);
			events -= Task::kIdleEvent;
		}
		else if(events&Task::kErrorEvent)
		{
			OnErrorEvent(NULL, 0);
			events -= Task::kErrorEvent;
		}
		else
		{
			return fTickTime;
		}
	}
    return 0;
}

void RTEventLooper::AddObserver(RTObserverConnection* conn)
{
    m_OCMItPair = m_mapConnectObserver.insert(std::make_pair(this, conn));
    if (!m_OCMItPair.second) {
        m_mapConnectObserver.erase(this);
        m_mapConnectObserver.insert(std::make_pair(this, conn));
    }
}

void RTEventLooper::DelObserver(RTObserverConnection* conn)
{
    m_mapConnectObserver.erase(this);
}
