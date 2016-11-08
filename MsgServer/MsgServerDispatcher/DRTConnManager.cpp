//
//  RTConnectionManager.cpp
//  dyncRTMsgQueue
//
//  Created by hp on 12/3/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#include "DRTConnManager.h"
#include <algorithm>
#include <assert.h>
#include <json/json.h>
#include "OSMutex.h"
#include "DRTTransferSession.h"
#include "RTZKClient.hpp"
#include "ProtoCommon.h"

static OSMutex       s_mutex;
static OSMutex       s_mutexModule;
static OSMutex       s_mutexTypeModule;

static DRTConnManager::ModuleInfoMaps                 s_ModuleInfoMap(0);
static DRTConnManager::TypeModuleSessionInfoLists     s_TypeModuleSessionInfoList(0);
static DRTConnManager::UserSessionInfoLists           s_UserSessionInfoList(0);
static DRTConnManager::UserSessionInfoMaps            s_UserSessionInfoMap(0);


DRTConnManager::ModuleInfo* DRTConnManager::findConnectorInfo(const std::string& userid)
{
    return findModuleInfo(userid, pms::ETransferModule::MCONNECTOR);
}

DRTConnManager::ModuleInfo* DRTConnManager::findModuleInfo(const std::string& uid, pms::ETransferModule module)
{
    DRTConnManager::ModuleInfo *pInfo = NULL;
    {
        OSMutexLocker locker(&s_mutexModule);
        DRTConnManager::ModuleInfoMapsIt it = s_ModuleInfoMap.begin();
        for (; it!=s_ModuleInfoMap.end(); it++) {
            if (it->second && it->second->othModuleType == module) {
                pInfo = it->second;
                break;
            }
        }
    }
    return pInfo;
}

DRTConnManager::ModuleInfo* DRTConnManager::findModuleInfoBySid(const std::string& sid)
{

    DRTConnManager::ModuleInfo *pInfo = NULL;
    {
        OSMutexLocker locker(&s_mutexModule);
        DRTConnManager::ModuleInfoMapsIt it = s_ModuleInfoMap.find(sid);
        if (it!=s_ModuleInfoMap.end()) {
            pInfo = it->second;
        }
    }
    return pInfo;
}

DRTConnManager::ModuleInfo* DRTConnManager::findConnectorInfoById(const std::string& userid, const std::string& connector)
{

    if (userid.length()==0 || connector.length()==0) {
        LE("findConnectorInfoById userid or connector is 0\n");
        return NULL;
    }
    DRTConnManager::ModuleInfo* pInfo = NULL;
    std::string sessionid;
    {
        OSMutexLocker locker(&s_mutexTypeModule);
        TypeModuleSessionInfoLists::iterator it = s_TypeModuleSessionInfoList.begin();
        TypeModuleSessionInfo* t_pInfo = NULL;
        for (; it!=s_TypeModuleSessionInfoList.end(); it++) {
            t_pInfo = *it;
            if (t_pInfo && t_pInfo->moduleId.compare(connector) == 0) {
                if (t_pInfo->sessionIds.empty()) {
                    continue;
                }
                sessionid = *(t_pInfo->sessionIds.begin());
                break;
            }
        }
    }
    if (sessionid.length()>0) {
        pInfo = findModuleInfoBySid(sessionid);
    } else {
        LE("findConnectorInfoById sessionid is null\n");
    }
    return pInfo;
}

bool DRTConnManager::ConnectConnector()
{
    if (m_ipList.size() == 0) {
        return false;
    }
    std::list<std::string>::iterator it;
    for (it=m_ipList.begin(); it!=m_ipList.end(); it++) {
        std::string s = *it;
        char ip[16] = {0};
        unsigned int port = 0;
        sscanf(s.c_str(), "%s %u", ip, &port);
        LI("ip:%s, port:%u\n", ip, port);
        if (strlen(ip)>0 && port > 2048) {
            DoConnectConnector(ip, port);
        }
    }
    return true;
}

bool DRTConnManager::DoConnectConnector(const std::string ip, unsigned short port)
{
    DRTTransferSession* connectorSession = new DRTTransferSession();
    connectorSession->Init();
    // conn to connector
    while (!connectorSession->Connect(ip, port)) {
        LI("connecting to connector server %s:%u waiting...\n", ip.c_str(), port);
        usleep(100*1000);
    }
    LI("%s port:%u, socketFD:%d\n", __FUNCTION__, port, connectorSession->GetSocket()->GetSocketFD());
    connectorSession->EstablishConnection();
    return true;
}

void DRTConnManager::RefreshConnection()
{
    ModuleInfo* pmi = NULL;
    {
        OSMutexLocker locker(&s_mutexModule);
        ModuleInfoMapsIt it = s_ModuleInfoMap.begin();
        for (; it!=s_ModuleInfoMap.end(); it++) {
            pmi = it->second;
            if (pmi && pmi->othModuleType == pms::ETransferModule::MCONNECTOR) {
                if (pmi->pModule && pmi->pModule->RefreshTime()) {
                    pmi->pModule->KeepAlive();
                }
            }
        }
    }
}

bool DRTConnManager::SignalKill()
{
    {
        OSMutexLocker mlocker(&s_mutexModule);
        for (auto & x : s_ModuleInfoMap) {
            x.second->pModule->Signal(Task::kKillEvent);
            usleep(100*1000);
        }
    }

    return true;
}

bool DRTConnManager::ClearAll()
{
    {
        OSMutexLocker mlocker(&s_mutexModule);
        for (auto & x : s_ModuleInfoMap) {
            delete x.second;
            x.second = NULL;
            usleep(100*1000);
        }
        s_ModuleInfoMap.clear();
    }

    {
        OSMutexLocker tlocker(&s_mutexTypeModule);
        for (auto & x : s_TypeModuleSessionInfoList) {
            delete x;
            x = NULL;
        }
        s_TypeModuleSessionInfoList.clear();
    }
    m_ipList.clear();
     return true;
}

bool DRTConnManager::AddModuleInfo(DRTConnManager::ModuleInfo* pmi, const std::string& sid)
{
    OSMutexLocker locker(&s_mutexModule);
    DRTConnManager::ModuleInfoMapsIt it = s_ModuleInfoMap.find(sid);
    if (it!=s_ModuleInfoMap.end()) {
        DRTConnManager::ModuleInfo *p = it->second;
        delete p;
        p = NULL;
        s_ModuleInfoMap.erase(sid);
    }
    s_ModuleInfoMap.insert(make_pair(sid, pmi));
    return true;
}

bool DRTConnManager::DelModuleInfo(const std::string& sid, EventData& data)
{
    OSMutexLocker locker(&s_mutexModule);
    DRTConnManager::ModuleInfoMapsIt it = s_ModuleInfoMap.find(sid);
    if (it!=s_ModuleInfoMap.end()) {
        DRTConnManager::ModuleInfo *p = it->second;
        data.connect.module = (int)p->othModuleType;
        memset(data.connect.ip, 0x00, 17);
        memcpy(data.connect.ip, p->pModule->GetTransferAddr().c_str(), p->pModule->GetTransferAddr().length());
        data.connect.port = p->pModule->GetTransferPort();

        delete p;
        p = NULL;
        s_ModuleInfoMap.erase(sid);
    }
    return true;
}

bool DRTConnManager::AddTypeModuleSession(pms::ETransferModule module, const std::string& mid, const std::string& sid)
{
    TypeModuleSessionInfo* pInfo = NULL;
    bool found = false;
    {
        OSMutexLocker locker(&s_mutexTypeModule);
        TypeModuleSessionInfoLists::iterator it = s_TypeModuleSessionInfoList.begin();
        for (; it!=s_TypeModuleSessionInfoList.end(); it++) {
            if ((*it) && (*it)->moduleId.compare(mid) == 0) {
                pInfo = *it;
                found = true;
                break;
            }
        }
        if (found) {
            pInfo->sessionIds.insert(sid);
        } else {
            pInfo = new TypeModuleSessionInfo();
            pInfo->moduleType = module;
            pInfo->moduleId = mid;
            pInfo->sessionIds.insert(sid);
            s_TypeModuleSessionInfoList.push_front(pInfo);
        }
    }
    return true;
}

bool DRTConnManager::DelTypeModuleSession(const std::string& sid)
{
    TypeModuleSessionInfo* pInfo = NULL;
    bool found = false;
    {
        OSMutexLocker locker(&s_mutexTypeModule);
        TypeModuleSessionInfoLists::iterator it = s_TypeModuleSessionInfoList.begin();
        for (; it!=s_TypeModuleSessionInfoList.end(); it++) {
            pInfo = *it;
            std::set<std::string>::iterator sit = std::find(pInfo->sessionIds.begin(), pInfo->sessionIds.end(), sid);
            if (sit!=pInfo->sessionIds.end()) {
                pInfo->sessionIds.erase(sit);
                found = true;
            }
        }
    }
    return found;
}

void DRTConnManager::TransferSessionLostNotify(const std::string& sid)
{
    EventData data;
    data.mtype = SESSEVENT::_sess_lost;
    DelModuleInfo(sid, data);
    DelTypeModuleSession(sid);
}

void DRTConnManager::AddMemberToOnline(const std::string& uid)
{
    OSMutexLocker locker(&m_mutexMembers);
    m_onlineMembers.insert(uid);
}

bool DRTConnManager::IsMemberInOnline(const std::string& uid)
{
    bool found = false;
    {
        OSMutexLocker locker(&m_mutexMembers);
        found = !m_onlineMembers.empty() && m_onlineMembers.count(uid);
    }
    return found;
}

void DRTConnManager::DelMemberFmOnline(const std::string& uid)
{
    OSMutexLocker locker(&m_mutexMembers);
    m_onlineMembers.erase(uid);
}

void DRTConnManager::AddMemberToOffline(const std::string& uid)
{
    OSMutexLocker locker(&m_mutexMembers);
    m_offlineMembers.insert(uid);
}

bool DRTConnManager::IsMemberInOffline(const std::string& uid)
{
    bool found = false;
    {
        OSMutexLocker locker(&m_mutexMembers);
        found = !m_offlineMembers.empty() && m_offlineMembers.count(uid);
    }
    return found;
}

void DRTConnManager::DelMemberFmOffline(const std::string& uid)
{
    OSMutexLocker locker(&m_mutexMembers);
    m_offlineMembers.erase(uid);
}

void DRTConnManager::OnTLogin(const std::string& uid, const std::string& token, const std::string& connector)
{
    OSMutexLocker locker(&m_mutexMembers);
    m_onlineMembers.insert(uid);
    if (m_userConnectors.find(uid) == m_userConnectors.end()) {
        m_userConnectors.insert(make_pair(uid, connector));
    } else {
        m_userConnectors.erase(uid);
        m_userConnectors.insert(make_pair(uid, connector));
    }
}

void DRTConnManager::OnTLogout(const std::string& uid, const std::string& token, const std::string& connector)
{
    OSMutexLocker locker(&m_mutexMembers);
    m_onlineMembers.erase(uid);
    m_userConnectors.erase(uid);
}

void DRTConnManager::GetUserConnectorId(const std::string& uid, std::string& connector)
{
    connector = m_userConnectors.find(uid)->second;
}

bool DRTConnManager::ConnectHttpSvrConn(const std::string& addr, const unsigned short port, const std::string& host)
{
    if (!m_pHttpSvrConn) {
        LI("DRTConnManager::ConnectHttpSvrConn ok\n");
        m_pHttpSvrConn = new rtc::RefCountedObject<DRTHttpSvrConn>();
        m_pHttpSvrConn->SetHttpHost(addr, port, host);
    } else {
        LI("DRTConnManager::ConnectHttpSvrConn error\n");
        return false;
    }

    return true;
}

void DRTConnManager::PushMeetingMsg(const std::string& meetingid, const std::string& msgFromId, const std::string& meetingOnlineMembers, const std::string& pushMsg, const std::string& notification, const std::string& extra)
{
    if (m_pHttpSvrConn && meetingid.length()>0 && msgFromId.length()>0 && meetingOnlineMembers.length()>0 && pushMsg.length()>0 && notification.length()>0 && extra.length()>0) {
        m_pHttpSvrConn->HttpPushMeetingMsg(meetingid.c_str(), msgFromId.c_str(), meetingOnlineMembers.c_str(), pushMsg.c_str(), notification.c_str(), extra.c_str());
    } else {
        LE("DRTConnManager::PushMeetingMsg error\n");
    }
}
void DRTConnManager::PushCommonMsg(const std::string& sign, const std::string& targetid, const std::string& pushMsg, const std::string& notification, const std::string& extra)
{
    if (m_pHttpSvrConn && sign.length()>0 && targetid.length()>0 && pushMsg.length()>0 && notification.length()>0 && extra.length()>0) {
        m_pHttpSvrConn->HttpPushCommonMsg(sign.c_str(), targetid.c_str(), pushMsg.c_str(), notification.c_str(), extra.c_str());
    } else {
        LE("DRTConnManager::PushCommonMsg error\n");
    }
}
