//
//  LRTTransferSession.cpp
//  dyncRTLogical
//
//  Created by hp on 11/26/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#include <list>
#include <sys/time.h>
#include "LRTTransferSession.h"
#include "RTUtils.hpp"
#include "LRTConnManager.h"
#include "LRTLogicalManager.h"
#include "StatusCode.h"

#include "MsgServer/proto/storage_msg.pb.h"
#include "MsgServer/proto/sys_msg_type.pb.h"
#include "MsgServer/proto/sys_msg.pb.h"

#define TIMEOUT_TS (60*1000)

LRTTransferSession::LRTTransferSession(SessionType type)
: RTJSBuffer()
, RTLstorage()
, m_lastUpdateTime(0)
, m_moduleId("")
, m_transferSessId("")
, m_addr("")
, m_port(0)
, m_connectingStatus(0)
, m_tmpWMsgId(0)
, m_tmpRSeqnId(0)
, m_tmpRDataId(0)
, m_tmpRSeqn4DataId(0)
, m_tmpRPDataId(0)
, m_tmpRPSeqn4DataId(0)
, m_tmpRData2Id(0)
, m_tmpOData2Id(0)
, m_tmpOGData2Id(0)
, m_IsValid(true)
, m_sessionType(type)
{
    AddObserver(this);
    GenericSessionId(m_transferSessId);
}

LRTTransferSession::~LRTTransferSession()
{
    m_IsValid = false;
    while(m_RecvMsgBuf.size()>0)
    {
         m_RecvMsgBuf.pop();
    }
    DelObserver(this);
    Unit();
}

void LRTTransferSession::Init()
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
    for(int i=0;i<PACKED_MSG_ONCE_NUM;++i)
    {
        m_respReadMsg.add_msgs();
    }
    for(int i=0;i<PACKED_MSG_ONCE_NUM;++i)
    {
        m_respWriteMsg.add_msgs();
    }
    m_isRun = 1;
}

void LRTTransferSession::InitConf()
{
    for(int i=0;i<PACKED_MSG_ONCE_NUM;++i)
    {
        m_respReadMsg.add_msgs();
    }
    for(int i=0;i<PACKED_MSG_ONCE_NUM;++i)
    {
        m_respWriteMsg.add_msgs();
    }
    m_isRun = 1;
}

void LRTTransferSession::Unit()
{
    m_isRun = 0;
    Disconn();
    m_connectingStatus = 0;
}

bool LRTTransferSession::Connect(const std::string addr, int port)
{
    m_addr = addr;
    m_port = port;
    OS_Error err = GetSocket()->Connect(SocketUtils::ConvertStringToAddr(m_addr.c_str()), m_port);
    if (err == OS_NoErr || err == EISCONN) {
        m_connectingStatus = 1;
        return true;
    } else {
        LE("%s ERR:%d\n", __FUNCTION__, err);
        return false;
    }
}

bool LRTTransferSession::Connect()
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

void LRTTransferSession::Disconn()
{
    GetSocket()->Cleanup();
}

bool LRTTransferSession::RefreshTime()
{
    UInt64 now = OS::Milliseconds();
    if (m_lastUpdateTime <= now) {
        m_lastUpdateTime = now  + TIMEOUT_TS;
        RTTcpNoTimeout::UpdateTimer();
        return true;
    } else {
        return false;
    }
}

void LRTTransferSession::KeepAlive()
{
    pms::TransferMsg t_msg;
    pms::ConnMsg c_msg;

    c_msg.set_tr_module(pms::ETransferModule::MLOGICAL);
    c_msg.set_conn_tag(pms::EConnTag::TKEEPALIVE);

    t_msg.set_type(pms::ETransferType::TCONN);
    t_msg.set_flag(pms::ETransferFlag::FNOACK);
    t_msg.set_priority(pms::ETransferPriority::PNORMAL);
    t_msg.set_content(c_msg.SerializeAsString());

    this->SendTransferData(t_msg.SerializeAsString());
}

void LRTTransferSession::EstablishConnection()
{
    pms::TransferMsg t_msg;
    pms::ConnMsg c_msg;

    c_msg.set_tr_module(pms::ETransferModule::MLOGICAL);
    c_msg.set_conn_tag(pms::EConnTag::THI);

    t_msg.set_type(pms::ETransferType::TCONN);
    t_msg.set_flag(pms::ETransferFlag::FNEEDACK);
    t_msg.set_priority(pms::ETransferPriority::PHIGH);
    t_msg.set_content(c_msg.SerializeAsString());

    this->SendTransferData(t_msg.SerializeAsString());
}

void LRTTransferSession::SendTransferData(const char* pData, int nLen)
{
    RTTcpNoTimeout::SendTransferData(pData, nLen);
    GetSocket()->RequestEvent(EV_RE);
}

void LRTTransferSession::SendTransferData(const std::string& data)
{
    SendTransferData(data.c_str(), data.length());
}

void LRTTransferSession::PushReadMsg(pms::StorageMsg store)
{
    {
        OSMutexLocker locker(&m_mutexQRead);
        m_queueReadMsg.push(store);
    }
    this->Signal(Task::kPushEvent);
}

void LRTTransferSession::PushWriteMsg(pms::StorageMsg store)
{
    {
        OSMutexLocker locker(&m_mutexQWrite);
        m_queueWriteMsg.push(store);
    }
    this->Signal(Task::kIdleEvent);
}


// from RTTcpNoTimeout
void LRTTransferSession::OnRecvData(const char*pData, int nLen)
{
    if (!pData) {
        return;
    }
    RTJSBuffer::RecvData(pData, nLen);
}

void LRTTransferSession::OnRedisEvent(const char*pData, int nLen)
{
    int64_t c=0;
    if (m_RecvMsgBuf.size()>0)
    {
        std::string v = m_RecvMsgBuf.front();
        RTLstorage::DoProcessData(v.c_str(), v.length());
        m_RecvMsgBuf.pop();
    }

    if (m_IsValid && m_RecvMsgBuf.size()>0)
    {
        this->NotifyRedis();
    }
}

void LRTTransferSession::OnRecvMessage(const char*message, int nLen)
{
#if USE_QUEUE_TO_CACHE
    //write redis to store msg
    std::string s(message, nLen);
    m_RecvMsgBuf.push(s);
    if (m_IsValid)
        this->NotifyRedis();
#else
    RTLstorage::DoProcessData(message, nLen);
#endif
}

// for read
void LRTTransferSession::OnPushEvent(const char*pData, int nLen)
{
    if (!m_isRun) return;
    if(m_queueReadMsg.size()==0) return;
    bool hasData = false;
    for (int i=0;i<PACKED_MSG_ONCE_NUM;++i)
    {
        if(m_queueReadMsg.size()>0)
        {
            hasData = true;
            *(m_respReadMsg.mutable_msgs(i)) = m_queueReadMsg.front();
            {
                OSMutexLocker locker(&m_mutexQRead);
                m_queueReadMsg.pop();
            }
        } else {
             break;
        }
    }
    if (hasData)
    {
        {
            pms::TransferMsg tmsg;
            tmsg.set_type(pms::ETransferType::TDISPATCH);
            tmsg.set_flag(pms::ETransferFlag::FNOACK);
            tmsg.set_priority(pms::ETransferPriority::PNORMAL);
            tmsg.set_content(m_respReadMsg.SerializeAsString());
            this->SendTransferData(tmsg.SerializeAsString());
        }
        for (int i=0;i<PACKED_MSG_ONCE_NUM;++i)
        {
            m_respReadMsg.mutable_msgs(i)->Clear();
        }
    }
    if(m_queueReadMsg.size()>0)
    {
        this->Signal(Task::kPushEvent);
    }
}

// for write
void LRTTransferSession::OnTickEvent(const char*pData, int nLen)
{
    if (!m_isRun) return;
    if(m_queueWriteMsg.size()==0) return;
    bool hasData = false;
    for (int i=0;i<PACKED_MSG_ONCE_NUM;++i)
    {
        if(m_queueWriteMsg.size()>0)
        {
            hasData = true;
            *(m_respWriteMsg.mutable_msgs(i)) = m_queueWriteMsg.front();
            {
                OSMutexLocker locker(&m_mutexQWrite);
                m_queueWriteMsg.pop();
            }
        } else {
             break;
        }
    }
    if (hasData)
    {
        {
            pms::TransferMsg tmsg;
            tmsg.set_type(pms::ETransferType::TQUEUE);
            tmsg.set_flag(pms::ETransferFlag::FNOACK);
            tmsg.set_priority(pms::ETransferPriority::PNORMAL);
            tmsg.set_content(m_respWriteMsg.SerializeAsString());
            this->SendTransferData(tmsg.SerializeAsString());
        }
        for (int i=0;i<PACKED_MSG_ONCE_NUM;++i)
        {
            m_respWriteMsg.mutable_msgs(i)->Clear();
        }
    }
    if(m_queueWriteMsg.size()>0)
    {
        this->Signal(Task::kIdleEvent);
    }
}

// from RTTransfer

void LRTTransferSession::OnTransfer(const std::string& str)
{
    this->SendTransferData(str);
}

void LRTTransferSession::OnMsgAck(pms::TransferMsg& tmsg)
{
    pms::TransferMsg ack_msg;
    ack_msg.set_type(tmsg.type());
    ack_msg.set_flag(pms::ETransferFlag::FACK);
    ack_msg.set_priority(tmsg.priority());

    //OnTransfer(ack_msg.SerializeAsString());
}

void LRTTransferSession::OnTypeConn(const std::string& str)
{
    pms::ConnMsg c_msg;
    if (!c_msg.ParseFromString(str)) {
        LE("OnTypeConn c_msg ParseFromString error\n");
        return;
    }

    if ((c_msg.conn_tag() == pms::EConnTag::THI)) {
        // when other connect to ME:
        // send the transfersessionid and LogicalId to other
        pms::TransferMsg t_msg;

        c_msg.set_tr_module(pms::ETransferModule::MLOGICAL);
        c_msg.set_conn_tag(pms::EConnTag::THELLO);
        c_msg.set_transferid(m_transferSessId);
        //send self Logical id to other
        c_msg.set_moduleid(LRTConnManager::Instance().LogicalId());

        t_msg.set_type(pms::ETransferType::TCONN);
        //this is for transfer
        t_msg.set_flag(pms::ETransferFlag::FNEEDACK);
        t_msg.set_priority(pms::ETransferPriority::PHIGH);
        t_msg.set_content(c_msg.SerializeAsString());

        this->SendTransferData(t_msg.SerializeAsString());
    } else if ((c_msg.conn_tag() == pms::EConnTag::THELLO)) {
        // when ME connector to other:
        // store other's transfersessionid and other's moduleId
        if (c_msg.transferid().length()>0) {
            m_transferSessId = c_msg.transferid();
            {
                LRTConnManager::ModuleInfo* pmi = new LRTConnManager::ModuleInfo();
                if (pmi) {
                    pmi->flag = 1;
                    pmi->othModuleType = c_msg.tr_module();
                    pmi->othModuleId = m_transferSessId;
                    pmi->pModule = this;
                    switch(m_sessionType) {
                        case ESequenceRead:
                            LRTConnManager::Instance().SetSequenceReadSessId(m_transferSessId);
                            break;
                        case ESequenceWrite:
                            LRTConnManager::Instance().SetSequenceWriteSessId(m_transferSessId);
                            break;
                        case EStorageRead:
                            LRTConnManager::Instance().SetStorageReadSessId(m_transferSessId);
                            break;
                        case EStorageWrite:
                            LRTConnManager::Instance().SetStorageWriteSessId(m_transferSessId);
                            break;
                        case EOther:
                            break;
                        default:
                            break;
                    }
                    //bind session and transfer id
                    LRTConnManager::Instance().AddModuleInfo(pmi, m_transferSessId);
                    //store which moudle connect to this connector
                    //c_msg._moduleid:store other's module id
                    LI("store other connector moduleid:%s, transfersessionid:%s\n", c_msg.moduleid().c_str(), m_transferSessId.c_str());
                    LRTConnManager::Instance().AddTypeModuleSession(c_msg.tr_module(), c_msg.moduleid(), m_transferSessId);
                } else {
                    LE("new ModuleInfo error!!!\n");
                }
            }

            pms::TransferMsg t_msg;

            c_msg.set_tr_module(pms::ETransferModule::MLOGICAL);
            c_msg.set_conn_tag(pms::EConnTag::THELLOHI);
            c_msg.set_transferid(m_transferSessId);
            //send self Logical id to other
            c_msg.set_moduleid(LRTConnManager::Instance().LogicalId());

            t_msg.set_type(pms::ETransferType::TCONN);
            //this is for transfer
            t_msg.set_flag(pms::ETransferFlag::FNEEDACK);
            t_msg.set_priority(pms::ETransferPriority::PHIGH);
            t_msg.set_content(c_msg.SerializeAsString());

            this->SetTestName(m_transferSessId);
            this->SendTransferData(t_msg.SerializeAsString());
        } else {
            LE("Connection id:%s error!!!\n", c_msg.transferid().c_str());
        }
    } else if ((c_msg.conn_tag() == pms::EConnTag::THELLOHI)) {
        // when other connect to ME:
        if (m_transferSessId.compare(c_msg.transferid()) == 0) {
            LRTConnManager::ModuleInfo* pmi = new LRTConnManager::ModuleInfo();
            if (pmi) {
                pmi->flag = 1;
                pmi->othModuleType = c_msg.tr_module();
                pmi->othModuleId = m_transferSessId;
                pmi->pModule = this;
                //bind session and transfer id
                LRTConnManager::Instance().AddModuleInfo(pmi, m_transferSessId);
                //store which moudle connect to this connector
                //store other module id
                LI("store module type:%d, moduleid:%s, transfersessid:%s\n", (int)c_msg.tr_module(), c_msg.moduleid().c_str(), m_transferSessId.c_str());
                LRTConnManager::Instance().AddTypeModuleSession(c_msg.tr_module(), c_msg.moduleid(), m_transferSessId);
            } else {
                LE("new ModuleInfo error!!!!\n");
            }
            this->SetTestName(m_transferSessId);
        }
    }  else if (c_msg.conn_tag() == pms::EConnTag::TKEEPALIVE) {
        RTTcpNoTimeout::UpdateTimer();
    } else {
        LE("%s invalid msg tag\n", __FUNCTION__);
    }
}

void LRTTransferSession::OnTypeTrans(const std::string& str)
{
    LI("%s was called\n", __FUNCTION__);
}

void LRTTransferSession::OnTypeQueue(const std::string& str)
{
    LI("%s was called\n", __FUNCTION__);
}

void LRTTransferSession::OnTypeDispatch(const std::string& str)
{
    LI("%s was called\n", __FUNCTION__);
}

void LRTTransferSession::OnTypePush(const std::string& str)
{
    LI("%s was called\n", __FUNCTION__);
}

void LRTTransferSession::OnTypeError(const std::string& str)
{
    LI("%s was called\n", __FUNCTION__);
}

void LRTTransferSession::OnTypeWriteRequest(const std::string& str)
{
    pms::RelayMsg rmsg;
    if (!rmsg.ParseFromString(str)) return;

    switch (rmsg.svr_cmds())
    {
        case pms::EServerCmd::CNEWMSG:
        case pms::EServerCmd::CCREATESEQN:
        case pms::EServerCmd::CDELETESEQN:
            {
                pms::PackedStoreMsg store;
                if (!store.ParseFromString(rmsg.content())) return;
                for(int i=0;i<store.msgs_size();++i)
                {
                    if (store.msgs(i).ruserid().length()==0)
                    {
                        break;
                    }
                    if (store.msgs(i).msgid().length()==0)
                    {
                        char msgid[32] = {0};
                        sprintf(msgid, "wm:%u", m_tmpWMsgId++);
                        store.mutable_msgs(i)->set_msgid(msgid);
                    }
                    if(store.mutable_msgs(i)->msgid().length()==0)
                    {
                         break;
                    }
                    LRTLogicalManager::Instance().InsertDataWrite(this, store.mutable_msgs(i));
                }
                pms::TransferMsg tmsg;
                tmsg.set_type(pms::ETransferType::TWRITE_REQUEST);
                tmsg.set_flag(pms::ETransferFlag::FNOACK);
                tmsg.set_priority(pms::ETransferPriority::PNORMAL);
                tmsg.set_content(store.SerializeAsString());
                // send to sequence
                if (!LRTConnManager::Instance().PushSeqnWriteMsg(tmsg.SerializeAsString()))
                {
                    LRTConnManager::Instance().ReportError(pms::ETransferModule::MSEQUENCE, "", ERR_STR_SVR_NOT_EXISTS, ERR_CODE_SVR_NOT_EXISTS);
                }
            }
            break;
        default:
            {
                LE("OnTypeWriteRequest not handle svr_cmd:%d\n", rmsg.svr_cmds());
            }
            break;
    }
}

void LRTTransferSession::OnTypeWriteResponse(const std::string& str)
{

    pms::RelayMsg rmsg;
    if (!rmsg.ParseFromString(str)) return;

    // this is response from wirte seqn
    if (rmsg.svr_cmds()==pms::EServerCmd::CSEQN)
    {
        pms::PackedStoreMsg store, newmsg_store;
        if (!store.ParseFromString(rmsg.content())) return;
        for(int i=0;i<store.msgs_size();++i)
        {
            if (store.msgs(i).ruserid().length()==0)
            {
                break;
            }

            pms::StorageMsg* pstore = store.mutable_msgs(i);
            LRTLogicalManager::Instance().UpdateDataWrite(&pstore);
            if (pstore->tsvrcmd()==pms::EServerCmd::CNEWMSGSEQN)
            {
                newmsg_store.add_msgs()->MergeFrom(*pstore);
            } else if (pstore->tsvrcmd()==pms::EServerCmd::CCREATESEQN)
            {
                LRTLogicalManager::Instance().UpdateLocalSeqn(pstore);
                LRTLogicalManager::Instance().DeleteDataWrite(pstore);
            } else if (pstore->tsvrcmd()==pms::EServerCmd::CDELETESEQN)
            {
                // here should delete local seqn storage
                LRTLogicalManager::Instance().DeleteDataWrite(pstore);
            }
        }

        if (newmsg_store.msgs_size()>0)
        {
            pms::TransferMsg tmsg;
            tmsg.set_type(pms::ETransferType::TWRITE_REQUEST);
            tmsg.set_flag(pms::ETransferFlag::FNOACK);
            tmsg.set_priority(pms::ETransferPriority::PNORMAL);
            tmsg.set_content(newmsg_store.SerializeAsString());
            // send to storage
            if (!LRTConnManager::Instance().PushStoreWriteMsg(tmsg.SerializeAsString()))
            {
                LRTConnManager::Instance().ReportError(pms::ETransferModule::MSTORAGE, "", ERR_STR_SVR_NOT_EXISTS, ERR_CODE_SVR_NOT_EXISTS);
            }
        }
    // this means data write
    } else if (rmsg.svr_cmds()==pms::EServerCmd::CDATA)
    {
        pms::PackedStoreMsg store;
        if (!store.ParseFromString(rmsg.content())) return;
        for(int i=0;i<store.msgs_size();++i)
        {
            if (store.msgs(i).ruserid().length()==0)
            {
                break;
            }

            // after msg was generated, update server local seqn for user;
            // this sequene is from sequence()
            LRTLogicalManager::Instance().UpdateLocalSeqn(store.mutable_msgs(i));
            LRTLogicalManager::Instance().DeleteDataWrite(store.mutable_msgs(i));
        }
    } else {
        LE("OnTypeWriteResponse not handle svr_cmd:%d\n", rmsg.svr_cmds());
    }
}

void LRTTransferSession::OnTypeReadRequest(const std::string& str)
{
    pms::RelayMsg rmsg;
    if (!rmsg.ParseFromString(str)) return;

    if (rmsg.svr_cmds()==pms::EServerCmd::CSYNCSEQN)
    {
        pms::PackedStoreMsg store, s_store;
        if (!store.ParseFromString(rmsg.content())) return;
        for(int i=0;i<store.msgs_size();++i)
        {
            if (store.msgs(i).ruserid().length()==0)
            {
                break;
            }
            //LI("LRTTransferSession::OnTypeReadRequest was called, SYNCSEQN ruserid:%s\n", store.msgs(i).ruserid().c_str());

            // first get seqn from local server
            // if not find seqn in local server,
            // then send request to sequence server
            int64 seqn = -1;
            if (LRTLogicalManager::Instance().ReadLocalSeqn(store.mutable_msgs(i), &seqn))
            {
				//LI("LRTTransferSession::OnTypeReadRequest was called, SYNCSEQN ruserid:%s, read local seqn:%lld\n", store.msgs(i).ruserid().c_str(), seqn);
                // update max seqn
                if (seqn<0) continue;

                char msgid[32] = {0};
                sprintf(msgid, "rs:%u", m_tmpRSeqnId++);
                store.mutable_msgs(i)->set_msgid(msgid);
                store.mutable_msgs(i)->set_maxseqn(seqn);
                this->PushReadMsg(store.msgs(i));// has got max seqn, so send response directly
            } else {
                char msgid[32] = {0};
                sprintf(msgid, "rs:%u", m_tmpRSeqnId++);
                store.mutable_msgs(i)->set_msgid(msgid);
				//LI("LRTTransferSession::OnTypeReadRequest was called, SYNCSEQN ruserid:%s, read remote redis seqn, msgid:%s\n", store.msgs(i).ruserid().c_str(), msgid);
                LRTLogicalManager::Instance().InsertSeqnRead(this, store.mutable_msgs(i));
                s_store.add_msgs()->MergeFrom(store.msgs(i));
            }
        }
        if (s_store.msgs_size()>0)
        {
            pms::TransferMsg tmsg;
            tmsg.set_type(pms::ETransferType::TREAD_REQUEST);
            tmsg.set_flag(pms::ETransferFlag::FNOACK);
            tmsg.set_priority(pms::ETransferPriority::PNORMAL);
            tmsg.set_content(s_store.SerializeAsString());
            // send to sequnce
            if (!LRTConnManager::Instance().PushSeqnReadMsg(tmsg.SerializeAsString()))
            {
                LRTConnManager::Instance().ReportError(pms::ETransferModule::MSEQUENCE, "", ERR_STR_SVR_NOT_EXISTS, ERR_CODE_SVR_NOT_EXISTS);
            }
        }
    } else if (rmsg.svr_cmds()==pms::EServerCmd::CSYNCDATA) {
        // first get cur seqn
        pms::PackedStoreMsg store, d_store, s_store;
        if (!store.ParseFromString(rmsg.content())) return;
        for(int i=0;i<store.msgs_size();++i)
        {
            if (store.msgs(i).ruserid().length()==0)
            {
                break;
            }
            //LI("LRTTransferSession::OnTypeReadRequest was called, SYNCDATA ruserid:%s\n", store.msgs(i).ruserid().c_str());

            // first get seqn from local server
            // if not find seqn in local server,
            // then send request to sequence server
            // if get seqn from local, then send sync data request

            // sync the pointed sequence data
            if (store.msgs(i).rsvrcmd()==pms::EServerCmd::CPGETDATA)
            {
				char msgid[32] = {0};
				sprintf(msgid, "prd:%u", m_tmpRDataId++);
				store.mutable_msgs(i)->set_msgid(msgid);
                store.mutable_msgs(i)->set_sdmaxseqn(store.mutable_msgs(i)->sequence());
                d_store.add_msgs()->MergeFrom(store.msgs(i));
            } else if (store.msgs(i).rsvrcmd()==pms::EServerCmd::CSYNCONEDATA) {
				char msgid[32] = {0};
				sprintf(msgid, "ord:%u", m_tmpOData2Id++);
				store.mutable_msgs(i)->set_msgid(msgid);
                store.mutable_msgs(i)->set_sdmaxseqn(store.mutable_msgs(i)->sequence());
                d_store.add_msgs()->MergeFrom(store.msgs(i));
            } else if (store.msgs(i).rsvrcmd()==pms::EServerCmd::CSYNCONEGROUPDATA) {
				char msgid[32] = {0};
				sprintf(msgid, "ogrd:%u", m_tmpOGData2Id++);
				store.mutable_msgs(i)->set_msgid(msgid);
                store.mutable_msgs(i)->set_sdmaxseqn(store.mutable_msgs(i)->sequence());
                d_store.add_msgs()->MergeFrom(store.msgs(i));
            } else {
                // sync the client current seqn +1 data

                struct timeval tv;
                gettimeofday(&tv, NULL);

                LI("LRTTransferSession::OnTypeReadRequest ====>>>CSYNCDATA read local, store.msgs(i).ruserid:%s, store.msgs(i).sequence:%lld, sdmaxseqn:%lld, time:%lld\n"\
                        , store.msgs(i).ruserid().c_str()\
                        , store.msgs(i).sequence()\
                        , store.msgs(i).sdmaxseqn()\
                        , (long long)tv.tv_sec);
                char msgid[32] = {0};
                sprintf(msgid, "rd:%u", m_tmpRDataId++);
                store.mutable_msgs(i)->set_msgid(msgid);
                d_store.add_msgs()->MergeFrom(store.msgs(i));
            } // this is for others
        }
        if (d_store.msgs_size()>0)
        {
            pms::TransferMsg tmsg;
            tmsg.set_type(pms::ETransferType::TREAD_REQUEST);
            tmsg.set_flag(pms::ETransferFlag::FNOACK);
            tmsg.set_priority(pms::ETransferPriority::PNORMAL);
            tmsg.set_content(d_store.SerializeAsString());
            // send to storage
            if (!LRTConnManager::Instance().PushStoreReadMsg(tmsg.SerializeAsString()))
            {
                LRTConnManager::Instance().ReportError(pms::ETransferModule::MSTORAGE, "", ERR_STR_SVR_NOT_EXISTS, ERR_CODE_SVR_NOT_EXISTS);
            }
        }

        if (s_store.msgs_size()>0)
        {
            pms::TransferMsg tmsg;
            tmsg.set_type(pms::ETransferType::TREAD_REQUEST);
            tmsg.set_flag(pms::ETransferFlag::FNOACK);
            tmsg.set_priority(pms::ETransferPriority::PNORMAL);
            tmsg.set_content(s_store.SerializeAsString());
            // send to sequence
            if (!LRTConnManager::Instance().PushSeqnReadMsg(tmsg.SerializeAsString()))
            {
                LRTConnManager::Instance().ReportError(pms::ETransferModule::MSEQUENCE, "", ERR_STR_SVR_NOT_EXISTS, ERR_CODE_SVR_NOT_EXISTS);
            }
        }

    } else if (rmsg.svr_cmds()==pms::EServerCmd::CPGETDATA) {
        // this request is from pusher to get data, this implement should be same as CSYNCDATA
        // the only difference is here it get THE sequence-data
        pms::PackedStoreMsg store, d_store;
        if (!store.ParseFromString(rmsg.content())) return;
        for(int i=0;i<store.msgs_size();++i)
        {
            if (store.msgs(i).ruserid().length()==0)
            {
                break;
            }
            //LI("LRTTransferSession::OnTypeReadRequest was called, PGETDATA ruserid:%s\n", store.msgs(i).ruserid().c_str());
            // because pusher has give the pointed sequence, so sync data request directly
            // this should be ok
            {
				char msgid[32] = {0};
				sprintf(msgid, "prd:%u", m_tmpRDataId++);
				store.mutable_msgs(i)->set_msgid(msgid);

                store.mutable_msgs(i)->set_sdmaxseqn(store.mutable_msgs(i)->sequence());
                d_store.add_msgs()->MergeFrom(store.msgs(i));
            }
        }
        if (d_store.msgs_size()>0)
        {
            pms::TransferMsg tmsg;
            tmsg.set_type(pms::ETransferType::TREAD_REQUEST);
            tmsg.set_flag(pms::ETransferFlag::FNOACK);
            tmsg.set_priority(pms::ETransferPriority::PNORMAL);
            tmsg.set_content(d_store.SerializeAsString());
            // send to storage
            if (!LRTConnManager::Instance().PushStoreReadMsg(tmsg.SerializeAsString()))
            {
                LRTConnManager::Instance().ReportError(pms::ETransferModule::MSTORAGE, "", ERR_STR_SVR_NOT_EXISTS, ERR_CODE_SVR_NOT_EXISTS);
            }
        }
    } else {
         LE("OnTypeReadRequest not handle svr_cmd:%d\n", rmsg.svr_cmds());
    }
}

// for sequence, all read request, update maxseqn
// all write request, udpate sequence
void LRTTransferSession::OnTypeReadResponse(const std::string& str)
{
    pms::RelayMsg rmsg;
    if (!rmsg.ParseFromString(str)) return;

    if (rmsg.svr_cmds()==pms::EServerCmd::CSEQN)
    {
        pms::PackedStoreMsg store, d_store;
        if (!store.ParseFromString(rmsg.content())) return;
        for(int i=0;i<store.msgs_size();++i)
        {
            if (store.msgs(i).ruserid().length()==0 || store.msgs(i).msgid().length()==0)
            {
                break;
            }

            if (store.msgs(i).tsvrcmd()==pms::EServerCmd::CSYNCSEQN)
            {
                // after get seqn from remote server, update server local seqn for user;
                // this sequence is from maxseqn
                LRTLogicalManager::Instance().UpdateLocalMaxSeqn(store.mutable_msgs(i));
                LRTLogicalManager::Instance().DeleteSeqnRead(store.mutable_msgs(i));
            } else if (store.msgs(i).tsvrcmd()==pms::EServerCmd::CSSEQN4DATA)
            {
                LRTTransferSession* pSess = NULL;
                LRTLogicalManager::Instance().GetSessFromId(store.msgs(i).ruserid(), store.msgs(i).msgid(), &pSess);
                LRTLogicalManager::Instance().UpdateLocalMaxSeqn(store.mutable_msgs(i));
                LRTLogicalManager::Instance().DeleteSeqnRead(store.mutable_msgs(i));
                assert(pSess);

				char msgid[32] = {0};
				sprintf(msgid, "drd:%u", m_tmpRData2Id++);
				store.mutable_msgs(i)->set_msgid(msgid);

                store.mutable_msgs(i)->set_sdmaxseqn(store.mutable_msgs(i)->sequence());// modify the sequence, which msg you sync
                d_store.add_msgs()->MergeFrom(store.msgs(i));
            }
        }
        if (d_store.msgs_size()>0)
        {
            pms::TransferMsg tmsg;
            tmsg.set_type(pms::ETransferType::TREAD_REQUEST);
            tmsg.set_flag(pms::ETransferFlag::FNOACK);
            tmsg.set_priority(pms::ETransferPriority::PNORMAL);
            tmsg.set_content(d_store.SerializeAsString());
            // send to storage
            if (!LRTConnManager::Instance().PushStoreReadMsg(tmsg.SerializeAsString()))
            {
                LRTConnManager::Instance().ReportError(pms::ETransferModule::MSTORAGE, "", ERR_STR_SVR_NOT_EXISTS, ERR_CODE_SVR_NOT_EXISTS);
            }
        }
    } else if (rmsg.svr_cmds()==pms::EServerCmd::CDATA)
    {
        pms::PackedStoreMsg store;
        if (!store.ParseFromString(rmsg.content())) return;
        for(int i=0;i<store.msgs_size();++i)
        {
            if (store.msgs(i).ruserid().length()==0)
            {
                break;
            }
            //find storage module type and relay msg to module
            LRTConnManager::ModuleInfo* pMd = LRTConnManager::Instance().findModuleInfo("", (pms::ETransferModule)store.mutable_msgs(i)->module());
            if (pMd)
            {
                //TODO:
                if (pMd->pModule && pMd->pModule->IsLiveSession())
                {
                    struct timeval tv;
                    gettimeofday(&tv, NULL);

                    LI("LRTTransferSession::OnTypeReadResponse was called, CDATA ruserid:%s, time:%lld\n"\
                            , store.msgs(i).ruserid().c_str()\
                            , (long long)tv.tv_sec);
                    //send back to server for data read, e.g. rtlive
                     pMd->pModule->PushReadMsg(store.msgs(i));
                }
            } else {
                LI("not find module\n");
            }
        }
    } else {
        LE("OnTypeReadResponse not handle svr_cmd:%d\n", rmsg.svr_cmds());
    }
}


void LRTTransferSession::ConnectionDisconnected()
{
    if (m_transferSessId.length()>0) {
        m_connectingStatus = 0;
        m_IsValid = false;
        LRTConnManager::Instance().TransferSessionLostNotify(m_transferSessId);
    }
}

////////////////////////////////////////////////////////
////////////////////private/////////////////////////////
////////////////////////////////////////////////////////

