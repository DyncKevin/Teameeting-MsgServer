//
//  MRTTransferSession.cpp
//  dyncRTMeeting
//
//  Created by hp on 11/26/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#include "MRTTransferSession.h"
#include "MRTRoomManager.h"
#include "MRTConnManager.h"
#include "MRTHttpSvrConn.h"
#include "RTUtils.hpp"
#include "StatusCode.h"

#define TIMEOUT_TS (60*1000)

MRTTransferSession::MRTTransferSession(pms::ETransferModule module)
: RTTcp()
, RTJSBuffer()
, RTTransfer()
, m_lastUpdateTime(0)
, m_moduleId("")
, m_transferSessId("")
, m_module(module)
, m_addr("")
, m_port(0)
, m_connectingStatus(0)
, m_IsValid(true)
{
    AddObserver(this);
    GenericSessionId(m_transferSessId);
}

MRTTransferSession::~MRTTransferSession()
{
    m_IsValid = false;
    while(m_RecvMsgBuf.size()>0)
    {
         m_RecvMsgBuf.pop();
    }
    DelObserver(this);
    Unit();
}

void MRTTransferSession::Init()
{
    TCPSocket* socket = this->GetSocket();

    socket->Open();

    socket->InitNonBlocking(socket->GetSocketFD());
    socket->NoDelay();
    socket->KeepAlive();
    socket->SetSocketBufSize(MAX_SOCKET_BUF_32);
    socket->SetSocketRcvBufSize(MAX_SOCKET_BUF_64);


    this->SetTimer(120*1000);
}

void MRTTransferSession::Unit()
{
    Disconn();
    m_connectingStatus = 0;
}

bool MRTTransferSession::Connect(const std::string addr, int port)
{
    m_addr = addr;
    m_port = port;
    LI("MRTTransferSession::Connect(params) m_addr:%s, m_port:%d\n", m_addr.c_str(), m_port);
    OS_Error err = GetSocket()->Connect(SocketUtils::ConvertStringToAddr(m_addr.c_str()), m_port);
    if (err == OS_NoErr || err == EISCONN) {
        m_connectingStatus = 1;
        return true;
    } else {
        LE("%s ERR:%d\n", __FUNCTION__, err);
        return false;
    }
}

bool MRTTransferSession::Connect()
{
    if (m_addr.empty() || m_port < 2048) {
        LE("%s invalid params addr:%s, port:%d\n", __FUNCTION__, m_addr.c_str(), m_port);
        return false;
    }
    OS_Error err = GetSocket()->Connect(SocketUtils::ConvertStringToAddr(m_addr.c_str()), m_port);
    if (err == OS_NoErr || err == EISCONN) {
        m_connectingStatus = 1;
        return true;
    } else {
        LE("%s ERR:%d\n", __FUNCTION__, err);
        return false;
    }
}

void MRTTransferSession::Disconn()
{
    GetSocket()->Cleanup();
}

bool MRTTransferSession::RefreshTime()
{
    UInt64 now = OS::Milliseconds();
    if (m_lastUpdateTime <= now) {
        m_lastUpdateTime = now  + TIMEOUT_TS;
        RTTcp::UpdateTimer();
        return true;
    } else {
        return false;
    }
}

void MRTTransferSession::KeepAlive()
{
    pms::TransferMsg t_msg;
    pms::ConnMsg c_msg;

    c_msg.set_tr_module(pms::ETransferModule::MMEETING);
    c_msg.set_conn_tag(pms::EConnTag::TKEEPALIVE);

    t_msg.set_type(pms::ETransferType::TCONN);
    t_msg.set_flag(pms::ETransferFlag::FNOACK);
    t_msg.set_priority(pms::ETransferPriority::PNORMAL);
    t_msg.set_content(c_msg.SerializeAsString());

    std::string s = t_msg.SerializeAsString();
    SendTransferData(s.c_str(), (int)s.length());
}


void MRTTransferSession::TestConnection()
{

}

void MRTTransferSession::EstablishConnection()
{
    pms::TransferMsg t_msg;
    pms::ConnMsg c_msg;

    c_msg.set_tr_module(pms::ETransferModule::MMEETING);
    c_msg.set_conn_tag(pms::EConnTag::THI);

    t_msg.set_type(pms::ETransferType::TCONN);
    t_msg.set_flag(pms::ETransferFlag::FNEEDACK);
    t_msg.set_priority(pms::ETransferPriority::PHIGH);
    t_msg.set_content(c_msg.SerializeAsString());

    std::string s = t_msg.SerializeAsString();
    SendTransferData(s.c_str(), (int)s.length());
}

void MRTTransferSession::SendTransferData(const char* pData, int nLen)
{
    RTTcp::SendTransferData(pData, nLen);
    GetSocket()->RequestEvent(EV_RE);
}

// from RTTcp
void MRTTransferSession::OnRecvData(const char*pData, int nLen)
{
    if (!pData) {
        return;
    }
    RTJSBuffer::RecvData(pData, nLen);
}

void MRTTransferSession::OnRedisEvent(const char*pData, int nLen)
{
    int64_t c=0;
    if (m_RecvMsgBuf.size()>0)
    {
        std::string v = m_RecvMsgBuf.front();
        RTTransfer::DoProcessData(v.c_str(), v.length());
        m_RecvMsgBuf.pop();
    }

    if (m_IsValid && m_RecvMsgBuf.size()>0)
    {
        this->NotifyRedis();
    }
}

void MRTTransferSession::OnRecvMessage(const char*message, int nLen)
{
#if USE_QUEUE_TO_CACHE
    //write redis to store msg
    std::string s(message, nLen);
    m_RecvMsgBuf.push(s);
    if (m_IsValid)
        this->NotifyRedis();
#else
    RTTransfer::DoProcessData(message, nLen);
#endif
}

// from RTTransfer

void MRTTransferSession::OnTransfer(const std::string& str)
{
    RTTcp::SendTransferData(str.c_str(), (int)str.length());
}

void MRTTransferSession::OnMsgAck(pms::TransferMsg& tmsg)
{
    pms::TransferMsg ack_msg;
    ack_msg.set_type(tmsg.type());
    ack_msg.set_flag(pms::ETransferFlag::FACK);
    ack_msg.set_priority(tmsg.priority());

    OnTransfer(ack_msg.SerializeAsString());
}

void MRTTransferSession::OnTypeConn(const std::string& str)
{
    pms::ConnMsg c_msg;
    if (!c_msg.ParseFromString(str)) {
        LE("OnTypeConn c_msg.ParseFromString error\n");
    }
    LI("OnTypeConn connmsg--->:\n");
    //c_msg.PrintDebugString();

    if ((c_msg.conn_tag() == pms::EConnTag::THELLO)) {
        // when ME connector to other:
        if (c_msg.transferid().length()>0) {
            m_transferSessId = c_msg.transferid();
            {
                MRTConnManager::ModuleInfo* pmi = new MRTConnManager::ModuleInfo();
                if (pmi) {
                pmi->flag = 1;
                pmi->othModuleType = c_msg.tr_module();
                pmi->othModuleId = m_transferSessId;
                pmi->pModule = this;
                //bind session and transfer id
                MRTConnManager::Instance().AddModuleInfo(pmi, m_transferSessId);
                //store which moudle connect to this connector
                MRTConnManager::Instance().AddTypeModuleSession(c_msg.tr_module(), c_msg.moduleid(), m_transferSessId);
                LI("store other moduleid:%s, transfersessionid:%s\n", c_msg.moduleid().c_str(), m_transferSessId.c_str());
                } else {
                    LE("new ModuleInfo error\n");
                }
            }
            pms::TransferMsg t_msg;

            c_msg.set_tr_module(pms::ETransferModule::MMEETING);
            c_msg.set_conn_tag(pms::EConnTag::THELLOHI);
            c_msg.set_transferid(m_transferSessId);
            //send self module id to other
            c_msg.set_moduleid(MRTConnManager::Instance().MeetingId());

            t_msg.set_type(pms::ETransferType::TCONN);
            t_msg.set_flag(pms::ETransferFlag::FNEEDACK);
            t_msg.set_priority(pms::ETransferPriority::PHIGH);
            t_msg.set_content(c_msg.SerializeAsString());

            std::string s = t_msg.SerializeAsString();
            SendTransferData(s.c_str(), (int)s.length());

        } else {
            LE("Connection id:%s error!!!\n", c_msg.transferid().c_str());
        }
    }
}

void MRTTransferSession::OnTypeTrans(const std::string& str)
{
    pms::RelayMsg r_msg;
    if (!r_msg.ParseFromString(str)) {
        LE("OnTypeTrans r_msg.ParseFromString error\n");
    }

    pms::Entity e_msg;
    if (!e_msg.ParseFromString(r_msg.content())) {
        LE("OnTypeTrans e_msg.ParseFromString error\n");
    }
    switch (e_msg.msg_tag()) {
        case pms::EMsgTag::TENTER:
        case pms::EMsgTag::TLEAVE:
        {
            MRTRoomManager::Instance().HandleOptRoom(r_msg, e_msg);
        }
            break;
        case pms::EMsgTag::TCHAT:
        case pms::EMsgTag::TNOTIFY:
        {
            //handle msgs
            MRTRoomManager::Instance().HandleDcommRoom(r_msg, e_msg);
        }
            break;
        default:
        {
            //invalid params
            LE("OnTypeTrans invalid params\n");
        }
            break;
    }
}

void MRTTransferSession::OnTypeQueue(const std::string& str)
{
    LI("%s was called\n", __FUNCTION__);
}

void MRTTransferSession::OnTypeDispatch(const std::string& str)
{
    LI("%s was called\n", __FUNCTION__);
}

void MRTTransferSession::OnTypePush(const std::string& str)
{
    LI("%s was called\n", __FUNCTION__);
}

void MRTTransferSession::OnTypeTLogin(const std::string& str)
{
    pms::RelayMsg r_msg;
    if (!r_msg.ParseFromString(str)) {
        LE("OnTypeTLogin r_msg.ParseFromString error\n");
    }
}

void MRTTransferSession::OnTypeTLogout(const std::string& str)
{
    pms::RelayMsg r_msg;
    if (!r_msg.ParseFromString(str)) {
        LE("OnTypeTLogout r_msg.ParseFromString error\n");
    }
    Assert(r_msg.touser().users_size == 1);
    OnConnectionLostNotify(r_msg.touser().users(0), r_msg.content(), r_msg.connector());
    return;
}

/**
 *  User connection lost, clear user in the meeting
 */
void MRTTransferSession::OnConnectionLostNotify(const std::string& uid, const std::string& token, const std::string& connector)
{
    MRTRoomManager::Instance().ClearSessionLost(uid, token, connector);
}

/**
 * TransferSession connection lost, clear Transfer Session
 */
void MRTTransferSession::ConnectionDisconnected()
{
    if (m_transferSessId.length()>0) {
        m_connectingStatus = 0;
        m_IsValid = false;
        MRTConnManager::Instance().TransferSessionLostNotify(m_transferSessId);
    }
}

////////////////////////////////////////////////////////
////////////////////private/////////////////////////////
////////////////////////////////////////////////////////

