//
//  CRTTransferSession.cpp
//  dyncRTConnector
//
//  Created by hp on 11/26/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#include "CRTTransferSession.h"
#include "CRTConnManager.h"
#include "RTUtils.hpp"
#include "StatusCode.h"

#include "MsgServer/proto/common_msg.pb.h"
#include "MsgServer/proto/sys_msg_type.pb.h"
#include "MsgServer/proto/storage_msg_type.pb.h"
#include "MsgServer/proto/sys_msg.pb.h"
#include "MsgServer/proto/storage_msg.pb.h"
#include "MsgServer/proto/entity_msg.pb.h"
#include "MsgServer/proto/entity_msg_type.pb.h"


CRTTransferSession::CRTTransferSession()
: RTJSBuffer()
, RTTransfer()
, m_transferSessId("")
, m_IsValid(true)
{
    AddObserver(this);
    GenericSessionId(m_transferSessId);
}

CRTTransferSession::~CRTTransferSession()
{
    m_IsValid = false;
    while(m_RecvMsgBuf.size()>0)
    {
         m_RecvMsgBuf.pop();
    }
    DelObserver(this);
    Unit();
}

void CRTTransferSession::Init()
{
    TCPSocket* socket = this->GetSocket();

    socket->Open();

    socket->InitNonBlocking(socket->GetSocketFD());
    socket->NoDelay();
    socket->KeepAlive();
    socket->SetSocketBufSize(MAX_SOCKET_BUF_32);
    socket->SetSocketRcvBufSize(MAX_SOCKET_BUF_64);


    socket->SetTask(this);
    this->SetTimer(120*1000);
}

void CRTTransferSession::Unit()
{
    Disconn();
}

bool CRTTransferSession::Connect(const std::string addr, int port)
{
    if (addr.empty() || port < 2048) {
        LE("invalid params addr:%s, port:%d\n", addr.c_str(), port);
        return false;
    }
    OS_Error err = GetSocket()->Connect(SocketUtils::ConvertStringToAddr(addr.c_str()), port);
    if (err == OS_NoErr || err == EISCONN) {
        return true;
    } else {
        LE("Socket Connect  ERR:%d\n", err);
        return false;
    }
}

void CRTTransferSession::Disconn()
{

}

void CRTTransferSession::EstablishConnection()
{
    pms::TransferMsg t_msg;
    pms::ConnMsg c_msg;

    c_msg.set_tr_module(pms::ETransferModule::MCONNECTOR);
    c_msg.set_conn_tag(pms::EConnTag::THI);

    t_msg.set_type(pms::ETransferType::TCONN);
    t_msg.set_flag(pms::ETransferFlag::FNEEDACK);
    t_msg.set_priority(pms::ETransferPriority::PHIGH);
    t_msg.set_content(c_msg.SerializeAsString());

    std::string s = t_msg.SerializeAsString();
    SendTransferData(s.c_str(), (int)s.length());
}

void CRTTransferSession::SendTransferData(const char* pData, int nLen)
{
    RTTcp::SendTransferData(pData, nLen);
    GetSocket()->RequestEvent(EV_RE);
}

void CRTTransferSession::ConnectionLostNotify(const std::string& uid, const std::string& token)
{
    pms::TransferMsg t_msg;
    pms::RelayMsg r_msg;
    pms::ToUser *pto = r_msg.mutable_touser();
    pto->add_users(uid);

    r_msg.set_tr_module(pms::ETransferModule::MCONNECTOR);
    r_msg.set_connector(CRTConnManager::Instance().ConnectorId());
    r_msg.set_content(token);

    t_msg.set_type(pms::ETransferType::TLOGOUT);
    t_msg.set_content(r_msg.SerializeAsString());

    const std::string s = t_msg.SerializeAsString();
    SendTransferData(s.c_str(), (int)s.length());
}

void CRTTransferSession::ConnectionConnNotify(const std::string& uid, const std::string& token)
{
    pms::TransferMsg t_msg;
    pms::RelayMsg r_msg;
    pms::ToUser *pto = r_msg.mutable_touser();
    pto->add_users(uid);

    r_msg.set_tr_module(pms::ETransferModule::MCONNECTOR);
    r_msg.set_connector(CRTConnManager::Instance().ConnectorId());
    r_msg.set_content(token);


    t_msg.set_type(pms::ETransferType::TLOGIN);
    t_msg.set_content(r_msg.SerializeAsString());

    const std::string s = t_msg.SerializeAsString();
    SendTransferData(s.c_str(), (int)s.length());
}

void CRTTransferSession::TransferMsg(pms::EServerCmd cmd, const std::string& msg)
{
    pms::TransferMsg t_msg;
    pms::RelayMsg r_msg;

    r_msg.set_svr_cmds(cmd);
    r_msg.set_tr_module(pms::ETransferModule::MCONNECTOR);
    r_msg.set_connector(CRTConnManager::Instance().ConnectorId());
    r_msg.set_content(msg);

    t_msg.set_type(pms::ETransferType::TTRANS);
    t_msg.set_content(r_msg.SerializeAsString());

    const std::string s = t_msg.SerializeAsString();
    SendTransferData(s.c_str(), (int)s.length());
}

// from RTTcp
void CRTTransferSession::OnRecvData(const char*pData, int nLen)
{
    if (!pData) {
        return;
    }
    RTJSBuffer::RecvData(pData, nLen);
}

void CRTTransferSession::OnRedisEvent(const char*pData, int nLen)
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

void CRTTransferSession::OnRecvMessage(const char*message, int nLen)
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

void CRTTransferSession::OnTransfer(const std::string& str)
{
    RTTcp::SendTransferData(str.c_str(), (int)str.length());
}

void CRTTransferSession::OnMsgAck(pms::TransferMsg& tmsg)
{
    pms::TransferMsg ack_msg;
    ack_msg.set_type(tmsg.type());
    ack_msg.set_flag(pms::ETransferFlag::FACK);
    ack_msg.set_priority(tmsg.priority());

    const std::string s = ack_msg.SerializeAsString();
    OnTransfer(s);
}

void CRTTransferSession::OnTypeConn(const std::string& str)
{
    LI("%s was called\n", __FUNCTION__);
    pms::ConnMsg c_msg;
    if (!c_msg.ParseFromString(str)) {
        LE("OnTypeConn c_msg.ParseFromString error\n");
    }

    if ((c_msg.conn_tag() == pms::EConnTag::THI)) {
        // when other connect to ME:
        // generate TransferSessionId
        // send self ModuleId and TransferSessionId to other
        pms::TransferMsg t_msg;

        //this is for transfer
        t_msg.set_type(pms::ETransferType::TCONN);
        t_msg.set_flag(pms::ETransferFlag::FNEEDACK);
        t_msg.set_priority(pms::ETransferPriority::PHIGH);

        c_msg.set_tr_module(pms::ETransferModule::MCONNECTOR);
        c_msg.set_conn_tag(pms::EConnTag::THELLO);
        c_msg.set_transferid(m_transferSessId);
        //send self connector to other
        c_msg.set_moduleid(CRTConnManager::Instance().ConnectorId());

        t_msg.set_content(c_msg.SerializeAsString());
        std::string s = t_msg.SerializeAsString();
        SendTransferData(s.c_str(), (int)s.length());
        LI("%s was called, hi sessId:%s\n", __FUNCTION__, m_transferSessId.c_str());
    } else if ((c_msg.conn_tag() == pms::EConnTag::THELLOHI)) {
        // when other connect to ME:
        //get other's TransferSessionId and ModuleId
        if (m_transferSessId.compare(c_msg.transferid())==0) {
            CRTConnManager::ModuleInfo* pmi = new CRTConnManager::ModuleInfo();
            if (pmi) {
                pmi->flag = 1;
                //store other's module type
                pmi->othModuleType = c_msg.tr_module();
                pmi->othModuleId = m_transferSessId;
                pmi->pModule = this;
                //bind session and transfer id
                CRTConnManager::Instance().AddModuleInfo(pmi, m_transferSessId);
                //store which moudle connect to this connector
                //c_msg._moduleid: store other's module id
                CRTConnManager::Instance().AddTypeModuleSession(c_msg.tr_module(), c_msg.moduleid(), m_transferSessId);

                LI("%s was called, hellohi sessId:%s\n", __FUNCTION__, m_transferSessId.c_str());
            } else {
                LE("new ModuleInfo error!!!\n");
            }
            this->SetTestName(m_transferSessId);
        }
    } else if (c_msg.conn_tag() == pms::EConnTag::TKEEPALIVE) {
        this->RTTcp::UpdateTimer();
    } else {
        LE("%s invalid msg tag\n", __FUNCTION__);
    }
}

void CRTTransferSession::OnTypeTrans(const std::string& str)
{
    LI("%s was called\n", __FUNCTION__);
}

void CRTTransferSession::OnTypeQueue(const std::string& str)
{
    LI("%s was called\n", __FUNCTION__);
}

void CRTTransferSession::OnTypeDispatch(const std::string& str)
{
    pms::RelayMsg dmsg;
    if (!dmsg.ParseFromString(str)) {
        LE("OnTypeDispatch dmsg.ParseFromString error\n");
        return;
    }
    pms::ToUser to = dmsg.touser();

    {
        for(int i = 0;i < to.users_size(); ++i) {
            m_dispatchConnection.DispatchMsg(to.users(i), dmsg);
        }
    }
}

void CRTTransferSession::OnTypePush(const std::string& str)
{
    LI("%s was called\n", __FUNCTION__);
}

void CRTTransferSession::OnTypeTLogin(const std::string& str)
{
    LI("%s was called\n", __FUNCTION__);
}

void CRTTransferSession::OnTypeTLogout(const std::string& str)
{
    LI("%s was called\n", __FUNCTION__);
}

void CRTTransferSession::OnTypeError(const std::string& str)
{
    LI("%s was called\n", __FUNCTION__);
}

void CRTTransferSession::ConnectionDisconnected()
{
    if (m_transferSessId.length()>0) {
        LI("ConnectionDisconnected m_transferSessId:%s\n", m_transferSessId.c_str());
        m_IsValid = false;
        CRTConnManager::Instance().TransferSessionLostNotify(m_transferSessId);
    }
}

////////////////////////////////////////////////////////
////////////////////private/////////////////////////////
////////////////////////////////////////////////////////
