#include "RTTcp.h"
#include "RTJSBuffer.h"
#include "atomic.h"

unsigned int RTTcp::sSessionIDCounter = kFirstTCPSessionID;
RTTcp::RTTcp()
: Task()
, fTimeoutTask(NULL, 120 * 1000)
, fSocket(NULL, Socket::kNonBlockingSocketType)
, fTickTime(0)
{
	ListZero(&m_listSend);

	fSessionID = (UInt32)atomic_add(&sSessionIDCounter, 1);

	fTimeoutTask.SetTask(this);
    fSocket.SetTask(this);
}

RTTcp::~RTTcp(void)
{
	ListEmpty(&m_listSend);
    GetSocket()->Cleanup();
}

int RTTcp::SendData(const char*pData, int nLen)
{
    if (nLen > 9999) {
        LE("RTTcp::SendData pData is over length:%d\n", nLen);
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

int RTTcp::SendTransferData(const char*pData, int nLen)
{
    if (nLen > 9999) {
        LE("RTTcp::SendTransferData pData is over length:%d\n", nLen);
    }
    {
        char* ptr = (char*)malloc(sizeof(char)*(nLen+4));
        char* pptr = ptr;
        *pptr = '$';
        (pptr)++;
        RTJSBuffer::writeShort(&pptr, nLen);
        memcpy((pptr), pData, nLen);
        ptr[nLen+3] = '\0';
        {
            OSMutexLocker locker(&mMutexSend);
            ListAppend(&m_listSend, ptr, nLen+3);
        }
        pptr = NULL;
    }

    this->Signal(kWriteEvent);
    return nLen;
}

SInt64 RTTcp::Run()
{
	EventFlags events = this->GetEvents();
	this->ForceSameThread();

	// Http session is short connection, need to kill session when occur TimeoutEvent.
	// So return -1.
	if(events&Task::kTimeoutEvent || events&Task::kKillEvent)
	{
        ObserverConnectionMapIt it = m_mapConnectObserver.find(this);
        if (it != m_mapConnectObserver.end()) {
            RTObserverConnection *conn = it->second;
            if (conn) {
                conn->ConnectionDisconnected();
            }
        }
        printf("RTTcp timeout or killed\n");
		return -1;
	}

	while(this->IsLiveSession())
	{
		if(events&Task::kReadEvent)
		{
			UInt32	readed = 0;
			char	fRequestBuffer[kRequestBufferSizeInBytes];
			while(1)
			{
				readed = 0;
				// We don't have any new data, get some from the socket...
				OS_Error sockErr = fSocket.Read(fRequestBuffer, kRequestBufferSizeInBytes - 1, &readed);
				if (sockErr == EAGAIN)
					break;
				if (sockErr != OS_NoErr)
				{
					Assert(!fSocket.IsConnected());
					break;
				}
				if(readed > 0)
				{
                    //(fRequestBuffer[0] != '$')
					OnRecvData(fRequestBuffer, readed);
				}
			}

			fSocket.RequestEvent(EV_RE);
			events -= Task::kReadEvent;
		}
		else if(events&Task::kWriteEvent)
		{
			ListElement *elem = NULL;
			if((elem = m_listSend.first) != NULL)
			{
				UInt32 theLengthSent = 0;
				OS_Error err = fSocket.Send((char*)elem->content, elem->size, &theLengthSent);
				if (err == EAGAIN)
				{
					fSocket.RequestEvent(EV_RE | EV_WR);
				}
				else
				{
                    {
                        OSMutexLocker locker(&mMutexSend);
                        ListRemoveHead(&m_listSend);
                    }
					if(NULL != m_listSend.first)
						this->Signal(kWriteEvent);
				}
			}
            //OnSendEvent("", 0);
			events -= Task::kWriteEvent;
		}
		else if(events&Task::kWakeupEvent)
		{
			OnWakeupEvent("", 0);
			events -= Task::kWakeupEvent;
		}
		else if(events&Task::kPushEvent)
		{
			OnPushEvent("", 0);
			events -= Task::kPushEvent;
		}
		else if(events&Task::kIdleEvent)
		{
			//OnTickEvent("", 0);
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
    ObserverConnectionMapIt it = m_mapConnectObserver.find(this);
    if (it != m_mapConnectObserver.end()) {
        RTObserverConnection *conn = it->second;
        if (conn) {
            conn->ConnectionDisconnected();
        }
    }
    printf("RTTcp session die...\n");
    return -1;
}

void RTTcp::AddObserver(RTObserverConnection* conn)
{
    m_OCMItPair = m_mapConnectObserver.insert(std::make_pair(this, conn));
    if (!m_OCMItPair.second) {
        m_mapConnectObserver.erase(this);
        m_mapConnectObserver.insert(std::make_pair(this, conn));
    }
}

void RTTcp::DelObserver(RTObserverConnection* conn)
{
    m_mapConnectObserver.erase(this);
}
