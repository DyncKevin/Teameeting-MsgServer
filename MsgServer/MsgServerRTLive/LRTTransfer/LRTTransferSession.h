//
//  LRTTransferSession.h
//  MsgServerRTLive
//
//  Created by hp on 11/26/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#ifndef __MsgServerRTLive__LRTTransferSession__
#define __MsgServerRTLive__LRTTransferSession__

#include <stdio.h>
#include <iostream>
#include <queue>
#include "SocketUtils.h"
#include "TCPSocket.h"
#include "RTTcpNoTimeout.h"
#include "RTJSBuffer.h"
#include "RTTransfer.h"
#include "RTObserverConnection.h"
#include "LRTModuleConnTcp.h"

#define DEF_PROTO 1
#include "ProtoCommon.h"

class LRTTransferSession
    : public RTTcpNoTimeout
    , public LRTModuleConnTcp
    , public RTJSBuffer
    , public RTTransfer
    , public RTObserverConnection{
public:
    LRTTransferSession();
    virtual ~LRTTransferSession();
    void Init();
    void InitConf();
    void Unit();
    bool Connect(const std::string addr, int port);
    bool Connect();
    void Disconn();
    bool RefreshTime();
    void KeepAlive();

    void SendTransferData(const char* pData, int nLen);
    void SendTransferData(const std::string& data);

    void SetModuleId(std::string& moduleId) { m_moduleId = moduleId; }
    void TestConnection();

    std::string& GetTransferAddr() { return m_addr; }
    int GetTransferPort() { return m_port; }
    int GetConnectingStatus() { return m_connectingStatus; };
public:
    void EstablishConnection();

    void PushNewMsg2Queue(const std::string& str)
    {
        {
            OSMutexLocker locker(&m_mutexQueueNew);
            m_queueNewMsg.push(str);
        }
        this->Signal(Task::kPushEvent);
    }

    void PushSeqnReq2Queue(const std::string& str)
    {
        {
            OSMutexLocker locker(&m_mutexQueueSeqn);
            m_queueSeqnMsg.push(str);
        }
        this->Signal(Task::kWakeupEvent);
    }

    void PushDataReq2Queue(const std::string& str)
    {
        {
            OSMutexLocker locker(&m_mutexQueueData);
            m_queueDataMsg.push(str);
        }
        this->Signal(Task::kIdleEvent);
    }

// from RTTcpNoTimeout
public:
    virtual void OnRecvData(const char*pData, int nLen);
    virtual void OnSendEvent(const char*pData, int nLen) {}
    virtual void OnWakeupEvent(const char*pData, int nLen);
    virtual void OnPushEvent(const char*pData, int nLen);
    virtual void OnTickEvent(const char*pData, int nLen);
    virtual void OnRedisEvent(const char*pData, int nSize);

// from RTTransfer
public:
    virtual void OnTransfer(const std::string& str);
    virtual void OnMsgAck(pms::TransferMsg& tmsg);
    virtual void OnTypeConn(const std::string& str);
    virtual void OnTypeTrans(const std::string& str);
    virtual void OnTypeQueue(const std::string& str);
    virtual void OnTypeDispatch(const std::string& str);
    virtual void OnTypePush(const std::string& str);
    virtual void OnTypeTLogin(const std::string& str);
    virtual void OnTypeTLogout(const std::string& str);

// from LRTGrpConnTcp
public:
    virtual void OnSyncSeqn(pms::EServerCmd cmd, pms::EModuleType module, const std::string& msg);
    virtual void OnSyncData(pms::EServerCmd cmd, pms::EModuleType module, const std::string& msg);
    virtual void OnGroupNotify(pms::EServerCmd cmd, pms::EModuleType module, const std::string& msg);
    virtual void OnPGetData(pms::EServerCmd cmd, pms::EModuleType module, const std::string& msg);
    virtual void OnCreateGroupSeqn(pms::EServerCmd cmd, pms::EModuleType module, const std::string& msg);
    virtual void OnDeleteGroupSeqn(pms::EServerCmd cmd, pms::EModuleType module, const std::string& msg);

    virtual void OnResponse(const char*pData, int nLen);
protected:
   virtual void OnRecvMessage(const char*message, int nLen);
// from RTObserverConnection
public:
    virtual void ConnectionDisconnected();

private:
    UInt64          m_lastUpdateTime;
    std::string     m_transferSessId;
    std::string     m_moduleId;
    std::string     m_addr;
    int             m_port;
    int             m_connectingStatus;

    pms::PackedStoreMsg         m_packedNewMsg;
    std::queue<std::string>     m_queueNewMsg;
    OSMutex                     m_mutexQueueNew;

    pms::PackedStoreMsg         m_packedSeqnMsg;
    std::queue<std::string>     m_queueSeqnMsg;
    OSMutex                     m_mutexQueueSeqn;

    pms::PackedStoreMsg         m_packedDataMsg;
    std::queue<std::string>     m_queueDataMsg;
    OSMutex                     m_mutexQueueData;

    unsigned int                m_wNewMsgId;
    std::queue<std::string>             m_RecvMsgBuf;
    bool                                m_IsValid;
};

#endif /* defined(__MsgServerRTLive__LRTTransferSession__) */
