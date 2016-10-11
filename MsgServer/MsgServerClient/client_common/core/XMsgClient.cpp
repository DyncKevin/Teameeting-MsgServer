//
//  XMsgClient.cpp
//  dyncRTCMsgClient
//
//  Created by hp on 12/22/15.
//  Copyright (c) 2015 Dync. All rights reserved.
//

#include "core/XMsgClient.h"
#include <sys/time.h>


#define TIMEOUT_TS (60*1000)

XMsgClient::XMsgClient()
: XJSBuffer()
, m_pCallback(nullptr)
, m_pClientImpl(nullptr)
, m_pMsgProcesser(nullptr)
, m_lastUpdateTime(0)
, m_uid("")
, m_token("")
, m_nname("")
, m_version("")
, m_uuid("")
, m_server("180.150.179.131")
, m_port(6630)
, m_autoConnect(true)
, m_login(false)
, m_msState(MSNOT_CONNECTED)
, m_syncGroupDataMaxSeqn(0)
, m_syncDataMaxSeqn(0)
{
#if WEBRTC_ANDROID
    LOGI("XMsgClient XMsgClient ok!!\n");
#else
    LOG(INFO) << "XMsgClient XMsgClient ok!!";
#endif
}

XMsgClient::~XMsgClient()
{

}

int XMsgClient::Init(const std::string& uid, const std::string& token, const std::string& nname, int devType, int push, int module)
{
    if (!m_pMsgProcesser) {
        m_pMsgProcesser = new XMsgProcesser(*this);
    }
    if (!m_pMsgProcesser) {
        return -1;
    }

    if (!m_pClientImpl) {
        m_pClientImpl = new XTcpClientImpl(*this);
    }
    if (!m_pClientImpl) {
        if (m_pMsgProcesser) {
            delete m_pMsgProcesser;
            m_pMsgProcesser = NULL;
        }
        return -1;
    }
    m_uid = uid;
    m_token = token;
    m_nname = nname;
    m_version = MSG_VERSION;
    m_devType = devType;
    m_enablePush = push;
    m_module = (pms::EModuleType)module;

#if WEBRTC_ANDROID
    LOGI("XMsgClient Init ok!!\n");
#else
    LOG(INFO) << "XMsgClient Init ok!!";
#endif
    return 0;
}

int XMsgClient::Unin()
{
#if WEBRTC_ANDROID
    LOGI("XMsgClient Unin !!\n");
#else
    LOG(INFO) << "XMsgClient Unin !!";
#endif
    if (m_uWait4AckMsgMap.size()>0)
        m_uWait4AckMsgMap.clear();
    if (m_uSyncedMsgMap.size()>0)
        m_uSyncedMsgMap.clear();
    if (m_uUserSeqnMap.size()>0)
        m_uUserSeqnMap.clear();
    if (m_uRecvMsgList.size()>0)
        m_uRecvMsgList.clear();

    if (m_gWait4AckMsgMap.size()>0)
        m_gWait4AckMsgMap.clear();
    if (m_gSyncedMsgMap.size()>0)
        m_gSyncedMsgMap.clear();
    if (m_gUserSeqnMap.size()>0)
        m_gUserSeqnMap.clear();
    if (m_gRecvMsgList.size()>0)
        m_gRecvMsgList.clear();

    if (m_Wait4CheckSeqnKeyMap.size()>0)
        m_Wait4CheckSeqnKeyMap.clear();

    if (m_MaxSeqnMap.size()>0)
        m_MaxSeqnMap.clear();

    if (m_pClientImpl) {
        m_pClientImpl->Disconnect();
        delete m_pClientImpl;
        m_pClientImpl = nullptr;
    }
    if (m_pMsgProcesser) {
        delete m_pMsgProcesser;
        m_pMsgProcesser = nullptr;
    }
#if WEBRTC_ANDROID
    LOGI("XMsgClient Unin over!!\n");
#else
    LOG(INFO) << "XMsgClient Unin over!!";
#endif
    return 0;
}

int XMsgClient::RegisterMsgCb(XMsgCallback* cb)
{
    if (!cb) return -1;
    m_pCallback = cb;
    return 0;
}

int XMsgClient::UnRegisterMsgCb(XMsgCallback* cb)
{
    //if (!cb) return -1;
    m_pCallback = nullptr;
    return 0;
}

int XMsgClient::ConnToServer(const std::string& server, int port, bool bAutoConnect)
{
    if (server.length()>0) {
        m_server = server;
    }
    if (port>2048) {
        m_port = port;
    }
#if WEBRTC_ANDROID
    LOGI("XMsgClient::ConnToServer server:%s, port:%d\n", server.c_str(), port);
#else
    LOG(INFO) << "XMsgClient::ConnToServer server:" << server << ", port:" << port;
#endif
    m_autoConnect = bAutoConnect;
    m_pClientImpl->Connect(server, port, bAutoConnect);
    m_msState = MSCONNECTTING;
    if (m_pCallback)
    {
        m_pCallback->OnMsgServerConnecting();
    } else {
#if WEBRTC_ANDROID
        LOGI("XMsgClient::OnServerDisconnect m_pCallback is null\n");
#else
        LOG(INFO) << "XMsgClient::OnServerDisconnect m_pCallback is null";
#endif
    }
    return 0;
}

int XMsgClient::AddGroup(const std::string& groupid)
{
    FetchGroupSeqn(groupid);
    return 0;
}

int XMsgClient::RmvGroup(const std::string& groupid)
{
    if (m_pCallback)
    {
        MSCbData cbData;
        cbData.type = 1;
        cbData.data = "del ok";
        m_pCallback->OnCmdCallback(0, MSRMV_GROUP, groupid, cbData);
    }
    return 0;
}


int XMsgClient::SndMsg(std::string& outmsgid, const std::string& groupid, const std::string& grpname, const std::string& msg, int tag, int type, int module, int flag, int push)
{
    if (groupid.length()==0 || msg.length()==0 || msg.length()>2048 || grpname.length()>128) {
        return -2;
    }
    std::string outstr;
    if (m_pMsgProcesser) {
        std::vector<std::string> uvec;
        m_pMsgProcesser->EncodeSndMsg(outstr, outmsgid, m_uid, m_token\
                , m_nname, groupid, grpname, uvec, msg\
                , tag, type, module, flag, push);
    } else {
        return -1;
    }
    if (outstr.length()==0) {
        return -1;
    }

    UAddWait4AckMsg(outmsgid, outstr);
    return SendEncodeMsg(outstr);
}

int XMsgClient::SndMsgTo(std::string& outmsgid, const std::string& groupid, const std::string& grpname, const std::string& msg, int tag, int type, int module, int flag, int push, const std::vector<std::string>& uvec)
{
    if (groupid.length()==0 || msg.length()==0 || msg.length()>2048 || grpname.length()>128) {
        return -2;
    }
    if (uvec.size()==0) {
        return -3;
    }
    std::string outstr;
    if (m_pMsgProcesser) {
        m_pMsgProcesser->EncodeSndMsg(outstr, outmsgid, m_uid, m_token\
                                      , m_nname, groupid, grpname, uvec, msg\
                                      , tag, type, module, flag, push);
    } else {
        return -1;
    }
    if (outstr.length()==0) {
        return -1;
    }

    GAddWait4AckMsg(outmsgid, outstr);
    return SendEncodeMsg(outstr);
}

int XMsgClient::FetchSeqn()
{
    std::string outstr;
    if (m_pMsgProcesser) {
        m_pMsgProcesser->EncodeSyncSeqn(outstr, m_uid, m_token, -1, m_module, pms::EStorageTag::TFETCHSEQN, pms::EMsgFlag::FSINGLE, pms::EMsgRole::RSENDER);
    } else {
        return -1;
    }
    if (outstr.length()==0) {
        return -1;
    }
    return SendEncodeMsg(outstr);
}

int XMsgClient::SyncSeqn(int64 seqn, int role)
{
    std::string outstr;
    if (m_pMsgProcesser) {
        m_pMsgProcesser->EncodeSyncSeqn(outstr, m_uid, m_token, seqn, m_module, pms::EStorageTag::TSEQN, pms::EMsgFlag::FSINGLE, role);
    } else {
        return -1;
    }
    if (outstr.length()==0) {
        return -1;
    }
    return SendEncodeMsg(outstr);
}

int XMsgClient::SyncData(int64 seqn, int64 sdmaxseqn)
{
    std::string outstr;
    if (m_pMsgProcesser) {
        m_pMsgProcesser->EncodeSyncData(outstr, m_uid, m_token, seqn, sdmaxseqn, m_module, pms::EStorageTag::TDATA, pms::EMsgFlag::FSINGLE);
    } else {
        return -1;
    }
    if (outstr.length()==0) {
        return -1;
    }
    int ret = SendEncodeMsg(outstr);
    if (ret>0) {
        rtc::CritScope cs(&m_csWait4CheckSeqnKey);
        char seqnKey[256] = {0};
        sprintf(seqnKey, "%s:%lld", m_uid.c_str(), seqn);
        if (m_Wait4CheckSeqnKeyMap.find(seqnKey)==m_Wait4CheckSeqnKeyMap.end()) {
            m_Wait4CheckSeqnKeyMap.insert(make_pair(seqnKey, 1));
        } else {
            m_Wait4CheckSeqnKeyMap[seqnKey]++;
        }
        //LOG(INFO) << "XMsgClient::SyncData m_Wait4CheckSeqnKeyMap userid:" << m_uid << " seqnKey:" << seqnKey << " is :" << m_Wait4CheckSeqnKeyMap[seqnKey];
    }
    return ret;
}

int XMsgClient::FetchGroupSeqn(const std::string& groupid)
{
    std::string outstr;
    if (m_pMsgProcesser) {
        m_pMsgProcesser->EncodeSyncGroupSeqn(outstr, m_uid, groupid, m_token, -1, m_module, pms::EStorageTag::TFETCHSEQN, pms::EMsgFlag::FGROUP, pms::EMsgRole::RSENDER);
    } else {
        return -1;
    }
    if (outstr.length()==0) {
        return -1;
    }
    return SendEncodeMsg(outstr);
}

int XMsgClient::SyncGroupSeqn(const std::string& groupid, int64 seqn, int role)
{
    std::string outstr;
    if (m_pMsgProcesser) {
            m_pMsgProcesser->EncodeSyncGroupSeqn(outstr, m_uid, groupid, m_token, seqn, m_module, pms::EStorageTag::TSEQN, pms::EMsgFlag::FGROUP, role);
    } else {
        return -1;
    }
    if (outstr.length()==0) {
        return -1;
    }
    return SendEncodeMsg(outstr);
}

int XMsgClient::SyncGroupData(const std::string& groupid, int64 seqn, int64 sdmaxseqn)
{
    std::string outstr;
    if (m_pMsgProcesser) {
        m_pMsgProcesser->EncodeSyncGroupData(outstr, m_uid, m_token, groupid, seqn, sdmaxseqn, m_module, pms::EStorageTag::TDATA, pms::EMsgFlag::FGROUP);
    } else {
        return -1;
    }
    if (outstr.length()==0) {
        return -1;
    }

    struct timeval tv;
    gettimeofday(&tv, NULL);
    LOG(INFO) << "XMsgClient::SyncGroupData was called,=========>>> uid:" << m_uid << ", curseqn:" << seqn << ", sdmaxseqn:" << sdmaxseqn << ", time:" << (long long)tv.tv_sec;
    int ret = SendEncodeMsg(outstr);
    if (ret>0) {
        rtc::CritScope cs(&m_csWait4CheckSeqnKey);
        char seqnKey[256] = {0};
        sprintf(seqnKey, "%s:%lld", groupid.c_str(), seqn);
        if (m_Wait4CheckSeqnKeyMap.find(seqnKey)==m_Wait4CheckSeqnKeyMap.end()) {
            m_Wait4CheckSeqnKeyMap.insert(make_pair(seqnKey, 1));
        } else {
            m_Wait4CheckSeqnKeyMap[seqnKey]++;
        }
        //LOG(INFO) << "XMsgClient::SyncGroupData m_Wait4CheckSeqnKeyMap uid:" << m_uid << " seqnKey:" << seqnKey << " is :" << m_Wait4CheckSeqnKeyMap[seqnKey];
    }
    return ret;
}

int XMsgClient::UpdateSetting(int64 setType, const std::string& jsonSetting)
{
    std::string outstr;
    if (m_pMsgProcesser) {
        m_pMsgProcesser->EncodeUpdateSetting(outstr, m_uid, setType, jsonSetting, m_module);
    } else {
        return -1;
    }
    if (outstr.length()==0) {
        return -1;
    }
    return SendEncodeMsg(outstr);
}

int XMsgClient::SyncOneData(int64 seqn)
{
    std::string outstr;
    if (m_pMsgProcesser) {
        m_pMsgProcesser->EncodeSyncOneData(outstr, m_uid, m_token, seqn, m_module, pms::EStorageTag::TDATA, pms::EMsgFlag::FSINGLE);
    } else {
        return -1;
    }
    if (outstr.length()==0) {
        return -1;
    }
    int ret = SendEncodeMsg(outstr);
    if (ret>0) {
        rtc::CritScope cs(&m_csWait4CheckSeqnKey);
        char seqnKey[256] = {0};
        sprintf(seqnKey, "%s:%lld", m_uid.c_str(), seqn);
        if (m_Wait4CheckSeqnKeyMap.find(seqnKey)==m_Wait4CheckSeqnKeyMap.end()) {
            m_Wait4CheckSeqnKeyMap.insert(make_pair(seqnKey, 1));
        } else {
            m_Wait4CheckSeqnKeyMap[seqnKey]++;
        }
        //LOG(INFO) << "XMsgClient::SyncOneData m_Wait4CheckSeqnKeyMap uid:" << m_uid << " seqnKey:" << seqnKey << " is :" << m_Wait4CheckSeqnKeyMap[seqnKey];
    }
    return ret;
}


int XMsgClient::SyncOneGroupData(const std::string& groupid, int64 seqn)
{
    std::string outstr;
    if (m_pMsgProcesser) {
        m_pMsgProcesser->EncodeSyncOneGroupData(outstr, m_uid, m_token, groupid, seqn, m_module, pms::EStorageTag::TDATA, pms::EMsgFlag::FGROUP);
    } else {
        return -1;
    }
    if (outstr.length()==0) {
        return -1;
    }

    struct timeval tv;
    gettimeofday(&tv, NULL);
    LOG(INFO) << "XMsgClient::SyncOneGroupData  =============================>>>uid:" << m_uid << " time:" << (long long)tv.tv_sec;

    int ret = SendEncodeMsg(outstr);

    if (ret>0) {
        rtc::CritScope cs(&m_csWait4CheckSeqnKey);
        char seqnKey[256] = {0};
        sprintf(seqnKey, "%s:%lld", groupid.c_str(), seqn);
        if (m_Wait4CheckSeqnKeyMap.find(seqnKey)==m_Wait4CheckSeqnKeyMap.end()) {
            m_Wait4CheckSeqnKeyMap.insert(make_pair(seqnKey, 1));
        } else {
            m_Wait4CheckSeqnKeyMap[seqnKey]++;
        }
        //LOG(INFO) << "XMsgClient::SyncOneGroupData m_Wait4CheckSeqnKeyMap uid:" << m_uid << " seqnKey:" << seqnKey << " is :" << m_Wait4CheckSeqnKeyMap[seqnKey];
    }
    return ret;
}


////////////////////////////////////////////
////////////////private/////////////////////
////////////////////////////////////////////

int XMsgClient::Login()
{
    std::string outstr;
    if (m_pMsgProcesser) {
        m_pMsgProcesser->EncodeLogin(outstr, m_uid, m_token, m_nname, m_uuid, m_devType, m_enablePush, m_module);
    } else {
        return -1;
    }
    if (outstr.length()==0) {
        return -1;
    }

    return SendEncodeMsg(outstr);
}

int XMsgClient::Logout()
{
    std::string outstr;
    if (m_pMsgProcesser) {
        m_pMsgProcesser->EncodeLogout(outstr, m_uid, m_token, m_module);
    } else {
        return -1;
    }
    if (outstr.length()==0) {
        return -1;
    }

    return SendEncodeMsg(outstr);
}

bool XMsgClient::RefreshTime()
{
    uint32_t now = rtc::Time();
    if (m_lastUpdateTime <= now) {
        m_lastUpdateTime = now  + TIMEOUT_TS;
        KeepAlive();
        return true;
    } else {
        return false;
    }
}

int XMsgClient::KeepAlive()
{
    //if login or not
    if (!m_login) {
        return -1;
    }
    std::string outstr;
    if (m_pMsgProcesser) {
        //outstr, userid
        m_pMsgProcesser->EncodeKeepAlive(outstr, m_uid, m_module);
    } else {
        return -1;
    }
    if (outstr.length()==0) {
        return -1;
    }
    LOG(INFO) << "XMsgClient::KeepAlive ===============================m_login:" << m_login << ", m_uid:" << m_uid << " SendEncodeMsg";
    return SendEncodeMsg(outstr);
}

int XMsgClient::SendEncodeMsg(std::string& msg)
{
    if (TcpState::CONNECTED!=m_pClientImpl->Status()) {
#ifdef WEBRTC_ANDROID
        LOGI("XMsgClient::SendEncodeMsg NOT_CONNECTED\n");
#else
        std::cout << "XMsgClient::SendEncodeMsg NOT_CONNECTED" << std::endl;
#endif
        return -1;
    }
    char* ptr = new char[msg.length()+4];//sprintf will add 1 in the end
    if (ptr) {
        char* pptr = ptr;
        *pptr = '$';
        pptr++;
        writeShort(&pptr, (unsigned short)msg.length());
        memcpy((pptr), msg.c_str(), msg.length());
        ptr[msg.length()+3] = '\0';
        if (m_pClientImpl) {
            int n = m_pClientImpl->SendMessageX(ptr, (int)(msg.length()+3));
            delete [] ptr;
            ptr = NULL;
            pptr = NULL;
            return n;
        } else {
#ifdef WEBRTC_ANDROID
            LOGI("XMsgClient::SendEncodeMsg m_pClientImpl is NULL\n");
#else
            std::cout << "XMsgClient::SendEncodeMsg m_pClientImpl is NULL" << std::endl;
#endif
            delete [] ptr;
            ptr = NULL;
            pptr = NULL;
            return -1;
        }
    } else {
#ifdef WEBRTC_ANDROID
        LOGI("XMsgClient::SendEncodeMsg ptr is NULL\n");
#else
        std::cout << "XMsgClient::SendEncodeMsg ptr is NULL" << std::endl;
#endif
        return -1;
    }
}

void XMsgClient::writeShort(char** pptr, unsigned short anInt)
{
    **pptr = (char)(anInt / 256);
    (*pptr)++;
    **pptr = (char)(anInt % 256);
    (*pptr)++;
}

unsigned short XMsgClient::readShort(char** pptr)
{
    char* ptr = *pptr;
    unsigned short len = 256 * ((unsigned char)(*ptr)) + (unsigned char)(*(ptr + 1));
    *pptr += 2;
    return len;
}

//////////////////////////////////////////////////
//////////////XTcpClientCallback//////////////////
//////////////////////////////////////////////////

void XMsgClient::OnServerConnected()
{
    //LOG(INFO) << __FUNCTION__ << " was called";
    Login();
}

void XMsgClient::OnServerDisconnect()
{
    //LOG(INFO) << __FUNCTION__ << " was called";
    m_login = false;
    m_msState = MSNOT_CONNECTED;
    if (m_pCallback)
    {
        m_pCallback->OnMsgServerDisconnect();
    } else {
#if WEBRTC_ANDROID
        LOGI("XMsgClient::OnServerDisconnect m_pCallback is null\n");
#else
        LOG(INFO) << "XMsgClient::OnServerDisconnect m_pCallback is null";
#endif
    }
}

void XMsgClient::OnServerConnectionFailure()
{
    //LOG(INFO) << __FUNCTION__ << " was called";
    m_login = false;
    m_msState = MSNOT_CONNECTED;
    if (m_pCallback)
    {
        m_pCallback->OnMsgServerConnectionFailure();
    } else {
#if WEBRTC_ANDROID
        LOGI("XMsgClient::OnServerConnectionFailure m_pCallback is null\n");
#else
        LOG(INFO) << "XMsgClient::OnServerConnectionFailure m_pCallback is null";
#endif
    }
}

void XMsgClient::OnTick()
{
    //LOG(INFO) << __FUNCTION__ << " was called, usrid:" << m_uid;
    if (m_msState == MSCONNECTED) {
        RefreshTime();
    }

}

void XMsgClient::OnMessageSent(int err)
{
    //LOG(INFO) << __FUNCTION__ << " was called err:" << err;
}

void XMsgClient::OnMessageRecv(const char*pData, int nLen)
{
    if (!pData) {
        return;
    }
    XJSBuffer::RecvData(pData, nLen);
}

void XMsgClient::OnClientSyncSeqn()
{
    //LOG(INFO) << __FUNCTION__ << " was called ----------";
    {
        UserSeqnMapIt  itCurSeqn = m_uUserSeqnMap.find(m_uid);
        if (itCurSeqn->second<0)
        {
#if WEBRTC_ANDROID
            LOGI("XMsgClient::OnClientSyncSeqn itCurSeqn is <0, so return\n");
#else
            LOG(INFO) << "XMsgClient::OnClientSyncSeqn itCurSeqn is <0, so return";
#endif
            return;
        }
        this->SyncSeqn(itCurSeqn->second, pms::EMsgRole::RSENDER);
    }
}

void XMsgClient::OnClientSyncGroupSeqn(const std::string& storeid)
{
    LOG(INFO) << __FUNCTION__ << " was called --------uid:" << m_uid << ", --storeid:" << storeid;
    {
        UserSeqnMapIt it = m_gUserSeqnMap.find(storeid);
        if (it != m_gUserSeqnMap.end())
        {
            SyncGroupSeqn(it->first, it->second, pms::EMsgRole::RSENDER);
        } else {
#if WEBRTC_ANDROID
            LOGI("XMsgClient::OnClientSyncGroupSeqn not find storeid:%s\n", storeid.c_str());
#else
            LOG(INFO) << "XMsgClient::OnClientSyncGroupSeqn not find storeid:" << storeid;
#endif
        }
    }
}


//////////////////////////////////////////////////
//////////////XMsgClientHelper////////////////////
//////////////////////////////////////////////////

void XMsgClient::OnHelpLogin(int code, const std::string& userid)
{
    if (code == 0) {
        m_login = true;
        m_msState = MSCONNECTED;
        if (m_pCallback)
        {
            m_pCallback->OnMsgServerConnected();
        } else {
#if WEBRTC_ANDROID
            LOGI("XMsgClient::OnHelpLogin m_pCallback is null\n");
#else
            LOG(INFO) << "XMsgClient::OnHelpLogin m_pCallback is null";
#endif
        }
    } else {
        Login();
    }
}

void XMsgClient::OnHelpLogout(int code, const std::string& userid)
{
    m_login = false;
    m_msState = MSNOT_CONNECTED;
    if (m_pCallback)
    {
        m_pCallback->OnMsgServerDisconnect();
    } else {
#if WEBRTC_ANDROID
        LOGI("XMsgClient::OnHelpLogout m_pCallback is null\n");
#else
        LOG(INFO) << "XMsgClient::OnHelpLogout m_pCallback is null";
#endif
    }
}

void XMsgClient::OnHelpSndMsg(int code, const std::string& cont)
{
    if (m_pCallback)
    {
        m_pCallback->OnSndMsg(code, cont);
    } else {
#if WEBRTC_ANDROID
        LOGI("XMsgClient::OnHelpSndMsg m_pCallback is null\n");
#else
        LOG(INFO) << "XMsgClient::OnHelpSndMsg m_pCallback is null";
#endif
    }
    return;
}

void XMsgClient::OnHelpKeepLive(int code, const std::string& cont)
{

}

void XMsgClient::OnHelpSyncSeqn(int code, const std::string& cont)
{
    pms::StorageMsg store;
    if (!store.ParseFromString(cont))
    {
#if WEBRTC_ANDROID
        LOGI("OnHelpSyncSeqn store.ParseFromString error\n");
#else
        LOG(INFO) << "OnHelpSyncSeqn store.ParseFromString error";
#endif
        return;
    }
    if (store.result()!=0)
    {
#if WEBRTC_ANDROID
        LOGI("OnHelpSyncSeqn store.result is not 0, get msg error\n");
#else
        LOG(INFO) << "OnHelpSyncSeqn store.result is not 0, get msg error";
#endif
        return;
    }

    // store or update storeid's max seqn
    if (store.maxseqn()>0) {
        UserSeqnMapIt uit =  m_MaxSeqnMap.find(store.storeid());
        if (uit == m_MaxSeqnMap.end()) {
            m_MaxSeqnMap.insert(make_pair(store.storeid(), store.maxseqn()));
        } else {
            ///////uit->second = store.maxseqn(); // this line is only for test and skip sync data
            if (uit->second < store.maxseqn()) {
                uit->second = store.maxseqn();
                // here means no matter TSEQN or TFETCHSEQN
                // both them need sync data,
                // so move the following code from case TSEQN to here
                if (store.mflag()==pms::EMsgFlag::FGROUP)
                {
                    UserSeqnMapIt it = m_gUserSeqnMap.find(store.storeid());
                    // if cur seqn is smaller then maxseqn just synced
                    // then sync data with the smaller seqn
                    if (it != m_gUserSeqnMap.end() && it->second < store.maxseqn())
                    {
                        if (m_gSyncedMsgMap.size()==0) {
                            LOG(INFO) << "XMsgClinet::OnHelpSyncSeqn ----------syncGroupData seqn:" << it->second << ", maxseqn:" << uit->second;
                            m_syncGroupDataMaxSeqn = uit->second;
                            SyncGroupData(store.storeid(), it->second, uit->second);
                        } else {
                            LOG(INFO) << "XMsgClinet::OnHelpSyncSeqn ++++++++++still has group synced data processed, m_gSyncedMsgMap.size:" << m_gSyncedMsgMap.size();
                        }
                    }
                } else if (store.mflag()==pms::EMsgFlag::FSINGLE)
                {
                    UserSeqnMapIt it = m_uUserSeqnMap.find(store.storeid());
                    // if cur seqn is smaller then maxseqn just synced
                    // then sync data with the smaller seqn
                    if (it != m_uUserSeqnMap.end() && it->second < store.maxseqn())
                    {
                        if (m_uSyncedMsgMap.size()==0) {
                            LOG(INFO) << "XMsgClinet::OnHelpSyncSeqn ----------syncData seqn:" << it->second << ", maxseqn:" << uit->second;
                            m_syncDataMaxSeqn = uit->second;
                            SyncData(it->second, uit->second);
                        } else {
                            LOG(INFO) << "XMsgClinet::OnHelpSyncSeqn ++++++++++still has user synced data processed, m_uSyncedMsgMap.size:" << m_uSyncedMsgMap.size();
                        }
                    }
                }
            }
        }
    }
    switch (store.mtag())
    {
        case pms::EStorageTag::TSEQN:
        {
            // nothing special need to do
            // all need to do is sync data
            // which move to above
        }
            break;

        case pms::EStorageTag::TFETCHSEQN:
        {
            if (store.mflag()==pms::EMsgFlag::FGROUP)
            {
                if (m_pCallback)
                {
                    if (store.maxseqn()>=0)
                    {
                        MSCbData cbData;
                        cbData.type = 0;
                        cbData.data = "add ok";
                        cbData.seqn = store.maxseqn();
                        cbData.result = store.result();
                        m_pCallback->OnCmdCallback(0, MSADD_GROUP, store.storeid(), cbData);
                        m_pCallback->OnCmdCallback(0, MSFETCH_SEQN, store.storeid(), cbData);
                    } else {
                        MSCbData cbData;
                        cbData.type = 0;
                        cbData.data = "add fail";
                        cbData.result = store.result();
                        m_pCallback->OnCmdCallback(-1, MSADD_GROUP, store.storeid(), cbData);
                        m_pCallback->OnCmdCallback(-1, MSFETCH_SEQN, store.storeid(), cbData);
                    }
                } else {
#if WEBRTC_ANDROID
                    LOGI("XMsgClient::OnHelpSyncSeqn fetchseqn group m_pCallback is null\n");
#else
                    LOG(INFO) << "XMsgClient::OnHelpSyncSeqn fetchseqn group m_pCallback is null";
#endif
                }
            } else if (store.mflag()==pms::EMsgFlag::FSINGLE)
            {
                if (m_pCallback)
                {
                    if (store.maxseqn()>=0)
                    {
                        MSCbData cbData;
                        cbData.type = 0;
                        cbData.data = "fetch ok";
                        cbData.seqn = store.maxseqn();
                        cbData.result = store.result();
                        m_pCallback->OnCmdCallback(0, MSFETCH_SEQN, "", cbData);
                    } else {
                        MSCbData cbData;
                        cbData.type = 0;
                        cbData.data = "fetch fail";
                        cbData.result = store.result();
                        m_pCallback->OnCmdCallback(-1, MSFETCH_SEQN, "", cbData);
                    }
                } else {
#if WEBRTC_ANDROID
                    LOGI("XMsgClient::OnHelpSyncSeqn fetchseqn single m_pCallback is null\n");
#else
                    LOG(INFO) << "XMsgClient::OnHelpSyncSeqn fetchseqn single m_pCallback is null";
#endif
                }
            }
        }
            break;
        default:
#if WEBRTC_ANDROID
            LOGI("XMsgClient::OnHelpSyncSeqn mtag :%d not handle\n", store.mtag());
#else
            LOG(INFO) << "XMsgClient::OnHelpSyncSeqn mtag:" << store.mtag() << " not handle";
#endif
            break;
    }
    return;
}

void XMsgClient::DoHelpSyncData(const pms::StorageMsg& store)
{
    // this is max seqn
    UserSeqnMapIt uitmax = m_MaxSeqnMap.find(store.storeid());
    if (uitmax==m_MaxSeqnMap.end()) {
        return;
    }
    // this is cur seqn
    UserSeqnMapIt  itCurSeqn = m_uUserSeqnMap.find(store.storeid());
    if (itCurSeqn->second<0 || itCurSeqn->second > uitmax->second)
    {
        // cur seqn <0 || cur seqn > max seqn
        return;
    }
    if (store.sequence() <= itCurSeqn->second)
    {
        // if recved msg sequence <= curseqn, return
        return;
    }
    // sync data failed
    if (store.result()!=0) {
        // 1 check if cur seqn < max seqn
        // 2 check if m_Wait4CheckSeqnKeyMap is waiting for curseqn+1
        // if has data, check if 5 times enough
        // if not has, SyncOneData(curseqn +1)

        // because sync data failed, so sync curseqn+1 data
        char sk[256] = {0};
        sprintf(sk, "%s:%lld", store.storeid().c_str(), store.sequence());
        Wait4CheckSeqnKeyMapIt skit = m_Wait4CheckSeqnKeyMap.find(sk);
        if (skit==m_Wait4CheckSeqnKeyMap.end()) { // not find sk in map, so sync data
            // only when curseqn is smaller than the max seqn, then sync one group data once
            SyncOneData(store.sequence());
            return;
        } else { // find sk
            if (skit->second > SYNC_DATA_RETRY_TIMES) { // has try sync data 5 times
                // if curseqn < maxseqn, drop current seqn sync, itCurSeqn->second += 1;
                // sync the next one
                // if curseqn equal maxseqn, just drop current seqn sync
                // do not make itCurSeqn + 1
                if ((itCurSeqn->second < uitmax->second)
                    && (store.sequence() == itCurSeqn->second+1)) {
                    itCurSeqn->second += 1;//drop cur+1 msg
                }
                m_uSyncedMsgMap.erase(sk);
                {
                    rtc::CritScope cs(&m_csWait4CheckSeqnKey);
                    m_Wait4CheckSeqnKeyMap.erase(sk);
                }
                if (m_uSyncedMsgMap.size()==0)
                {
                    //if no data waiting for
                    //call syncseqn to get max seqn
                    //LOG(INFO) << "XMsgClinet::DoHelpSyncData ------->>call NotifySyncSeqn, itCurSeqn:" << itCurSeqn->second << ", m_syncDataMaxSeqn:" << m_syncDataMaxSeqn;
                    if (itCurSeqn->second == m_syncDataMaxSeqn)
                    {
                        if (m_pClientImpl)
                        {
                            m_pClientImpl->NotifySyncSeqn();
                        }
                    } else {

                    }
                }
                return;
            } else {
                // not reach 5 times, try to sync data again
                // only when curseqn is smaller than the max seqn, then sync data
                if (itCurSeqn->second < uitmax->second) {
                    SyncOneData(store.sequence());
                }
                return;
            }
        }
        return;
    }

    char seqnKey[256] = {0};
    sprintf(seqnKey, "%s:%lld", store.storeid().c_str(), store.sequence());
    UAddSyncedMsg(seqnKey, store);
    //LOG(INFO) << "XMsgClinet::DoHelpSyncData --->>m_uSyncedMsgMap.size:" << m_uSyncedMsgMap.size();

    UUpdateUserSeqn();

    for (RecvMsgListIt it = m_uRecvMsgList.begin();it!=m_uRecvMsgList.end();++it)
    {
        if (m_pCallback)
        {
            m_pCallback->OnRecvMsg(it->seqn, it->msgcont);
        } else {
#if WEBRTC_ANDROID
            LOGI("XMsgClient::DoHelpSyncData m_pCallback is null\n");
#else
            LOG(INFO) << "XMsgClient::DoHelpSyncData m_pCallback is null";
#endif
        }
    }
    m_uRecvMsgList.clear();
}

void XMsgClient::OnHelpSyncData(int code, const std::string& cont)
{
    pms::StorageMsg store;
    if (!store.ParseFromString(cont))
    {
#if WEBRTC_ANDROID
        LOGI("OnHelpSyncData store.ParseFromString error\n");
#else
        LOG(INFO) << "OnHelpSyncData store.ParseFromString error";
#endif
        return;
    }

    DoHelpSyncData(store);
    return;
}

void XMsgClient::DoHelpSyncGroupData(const pms::StorageMsg& store)
{
    // this is max seqn
    UserSeqnMapIt uitmax = m_MaxSeqnMap.find(store.storeid());
    if (uitmax==m_MaxSeqnMap.end()) {
        return;
    }
    // this is cur seqn
    UserSeqnMapIt  itCurSeqn = m_gUserSeqnMap.find(store.storeid());
    if (itCurSeqn->second<0 || itCurSeqn->second > uitmax->second)
    {
        // cur seqn <0 || cur seqn > max seqn
        return;
    }
    if (store.sequence() <= itCurSeqn->second)
    {
        // if recved msg sequence <= curseqn, return
        return;
    }
    // sync data failed
    if (store.result()!=0) {
        // 1 check if cur seqn < max seqn
        // 2 check if m_Wait4CheckSeqnKeyMap is waiting for curseqn+1
        // if has data, check if 5 times enough
        // if not has, SyncOneGroupData(curseqn +1)


        // just test for a while

        LOG(INFO) << "XMsgClient::DoHelpSyncGroupData =======>>>get uid:" << m_uid << ", msg result error:" << store.result();
        return;


        // because sync data failed, so sync curseqn+1 data
        char sk[256] = {0};
        sprintf(sk, "%s:%lld", store.storeid().c_str(), store.sequence());
        Wait4CheckSeqnKeyMapIt skit = m_Wait4CheckSeqnKeyMap.find(sk);
        if (skit==m_Wait4CheckSeqnKeyMap.end()) { // not find sk in map, so sync data
            // only when curseqn is smaller than the max seqn, then sync one group data once
            SyncOneGroupData(store.storeid().c_str(), store.sequence());
            return;
        } else { // find sk
            LOG(INFO) << "XMsgClient::DoHelpSyncGroupData =======>>>SyncOneGroupData uid:" << m_uid << ", time:" << skit->second;
            if (skit->second > SYNC_DATA_RETRY_TIMES) { // has try sync data 5 times
                // if curseqn < maxseqn, drop current seqn sync, itCurSeqn->second += 1;
                // sync the next one
                // if curseqn equal maxseqn, just drop current seqn sync
                // do not make itCurSeqn + 1
                if ((itCurSeqn->second < uitmax->second)
                    && (store.sequence() == itCurSeqn->second+1)) {
                    itCurSeqn->second += 1;//drop cur+1 msg
                }
                m_gSyncedMsgMap.erase(sk);
                {
                    rtc::CritScope cs(&m_csWait4CheckSeqnKey);
                    m_Wait4CheckSeqnKeyMap.erase(sk);
                }
                if (m_gSyncedMsgMap.size()==0)
                {
                    //if no data waiting for
                    //call syncseqn to get max seqn

                    //LOG(INFO) << "XMsgClinet::DoHelpSyncGroupData ------->>call NotifySyncGroupSeqn, itCurSeqn:" << itCurSeqn->second << ", m_syncGroupDataMaxSeqn:" << m_syncGroupDataMaxSeqn;
                    if (itCurSeqn->second == m_syncGroupDataMaxSeqn)
                    {
                        if (m_pClientImpl)
                        {
                            m_pClientImpl->NotifySyncGroupSeqn(store.storeid());
                        }
                    }
                }
                return;
            } else {
                // not reach 5 times, try to sync data again
                // only when curseqn is smaller than the max seqn, then sync data
                if (itCurSeqn->second < uitmax->second) {
                    SyncOneGroupData(store.storeid().c_str(), store.sequence());
                }
                return;
            }
        }
        return;
    }

    char seqnKey[256] = {0};
    sprintf(seqnKey, "%s:%lld", store.storeid().c_str(), store.sequence());
    GAddSyncedMsg(seqnKey, store);
    //LOG(INFO) << "XMsgClinet::DoHelpSyncGroupData --------------------------->>m_gSyncedMsgMap.size:" << m_gSyncedMsgMap.size();
    GUpdateUserSeqn(store.storeid());
    //LOG(INFO) << "XMsgClient::DoHelpSyncGroupData seqnKey:" << seqnKey << ", ruserid:" << store.ruserid() << \
        ", sequence:" << store.sequence() << ", maxseqn:" << store.maxseqn() << ", sdmaxseqn:" << store.sdmaxseqn();

    for (RecvMsgListIt it = m_gRecvMsgList.begin();it!=m_gRecvMsgList.end();++it)
    {
        if (m_pCallback)
        {
            m_pCallback->OnRecvGroupMsg(it->seqn, it->seqnid, it->msgcont);
        } else {
#if WEBRTC_ANDROID
            LOGI("XMsgClient::DoHelpSyncGroupData m_pCallback is null\n");
#else
            LOG(INFO) << "XMsgClient::DoHelpSyncGroupData m_pCallback is null";
#endif
        }
    }
    m_gRecvMsgList.clear();
}

void XMsgClient::OnHelpSyncGroupData(int code, const std::string& cont)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);

    LOG(INFO) << "OnHelpSyncGroupData userid:" << m_uid << ", time:" << tv.tv_sec << ", cont.length:" << cont.length();
    pms::StorageMsg store;
    if (!store.ParseFromString(cont))
    {
#if WEBRTC_ANDROID
        LOGI("OnHelpSyncGroupData store.ParseFromString error\n");
#else
        LOG(INFO) << "OnHelpSyncGroupData store.ParseFromString error";
#endif
        return;
    }

    if (store.conttype()==pms::EStoreContType::TPACKEDSTORE)
    {
        pms::PackedStoreMsg packed;
        if (packed.ParseFromString(store.content()))
        {
            for(int i=0;i<packed.msgs_size();++i)
            {
                DoHelpSyncGroupData(packed.msgs(i));
            }
        } else {
#if WEBRTC_ANDROID
        LOGI("OnHelpSyncGroupData packed.ParseFromString failed\n");
#else
        LOG(INFO) << "OnHelpSyncGroupData packed.ParseFromString failed";
#endif
        }
    } else {
#if WEBRTC_ANDROID
        LOGI("OnHelpSyncGroupData store.conttype is not handled:%d\n", store.conttype());
#else
        LOG(INFO) << "OnHelpSyncGroupData store.conttype is not handled:" << store.conttype();
#endif
    }

    return;
}

void XMsgClient::OnHelpGroupNotify(int code, const std::string& cont)
{
    pms::StorageMsg store;
    if (!store.ParseFromString(cont))
    {
#if WEBRTC_ANDROID
        LOGI("OnHelpGroupNotify store.ParseFromString error\n");
#else
        LOG(INFO) << "OnHelpGroupNotify store.ParseFromString error";
#endif
        return;
    }
    if (store.result()!=0)
    {
#if WEBRTC_ANDROID
        LOGI("OnHelpGroupNotify store.result is not 0, get msg error\n");
#else
        LOG(INFO) << "OnHelpGroupNotify store.result is not 0, get msg error";
#endif
        return;
    }

    if (m_pCallback)
    {
        UserSeqnMapIt it = m_gUserSeqnMap.find(store.groupid());
        if (it != m_gUserSeqnMap.end())
        {
            //LOG(INFO) << "XMsgClient::OnHelpGroupNotify SyncGroupSeqn find userid:" << store.ruserid();
            SyncGroupSeqn(it->first, it->second, pms::EMsgRole::RSENDER);
        } else {
#if WEBRTC_ANDROID
            LOGI("XMsgClient::OnHelpGroupNotify not find store.groupid:%s\n", store.groupid().c_str());
#else
            LOG(INFO) << "XMsgClient::OnHelpGroupNotify not find store.groupid:" << store.groupid();
#endif
        }
    } else {
#if WEBRTC_ANDROID
        LOGI("XMsgClient::OnHelpGroupNotify m_pCallback is null\n");
#else
        LOG(INFO) << "XMsgClient::OnHelpGroupNotify m_pCallback is null";
#endif
    }
    return;
}

// this means you need to sync seqn, get new seqn
// you just get new seqn from server
void XMsgClient::OnHelpNotifySeqn(int code, const std::string& cont)
{
    if (code==0)
    {
        UserSeqnMapIt  itCurSeqn = m_uUserSeqnMap.find(m_uid);
        if (itCurSeqn->second<0)
        {
#if WEBRTC_ANDROID
            LOGI("XMsgClient::OnHelpNotifySeqn itCurSeqn is <0, so return\n");
#else
            LOG(INFO) << "XMsgClient::OnHelpNotifySeqn itCurSeqn is <0, so return";
#endif
            return;
        }
        this->SyncSeqn(itCurSeqn->second, pms::EMsgRole::RSENDER);
    }
    return;
}

// this may not be useful, because the server will not notify to sync data directly
// instead server will notify to sync seqn
void XMsgClient::OnHelpNotifyData(int code, const std::string& cont)
{

}

void XMsgClient::OnHelpOtherLogin(int code, const std::string& cont)
{
    pms::StorageMsg store;
    if (!store.ParseFromString(cont))
    {
#if WEBRTC_ANDROID
        LOGI("OnHelpOtherLogin store.ParseFromString error\n");
#else
        LOG(INFO) << "OnHelpOtherLogin store.ParseFromString error";
#endif
        return;
    }
    if (m_pCallback)
    {
#if WEBRTC_ANDROID
        LOGI("OnHelpOtherLogin OnNotifyOtherLogin was called code:%d\n", code);
#else
        LOG(INFO) << "OnHelpOtherLogin OnNotifyOtherLogin code:" << code;
#endif
        m_pCallback->OnNotifyOtherLogin(code);
    }
}


//////////////////////////////////////////////////
//////////////XJSBuffer///////////////////////////
//////////////////////////////////////////////////

void XMsgClient::OnRecvMessage(const char*message, int nLen)
{
    if (m_pMsgProcesser) {
        m_pMsgProcesser->DecodeRecvData((char *)message, nLen);
    }
}
