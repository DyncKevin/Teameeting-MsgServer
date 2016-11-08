//
//  MRTConnManager.cpp
//  dyncRTMeeting
//
//  Created by hp on 12/3/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#include <assert.h>
#include <json/json.h>
#include "MRTConnManager.h"
#include "OSMutex.h"
#include "MRTRoomManager.h"
#include "MRTTransferSession.h"
#include "RTZKClient.hpp"

OSMutex       s_mutex;
OSMutex       s_mutexModule;
OSMutex       s_mutexTypeModule;

static MRTConnManager::ModuleInfoMaps                 s_ModuleInfoMap(0);
static MRTConnManager::TypeModuleSessionInfoLists     s_TypeModuleSessionInfoList(0);
static MRTConnManager::UserSessionInfoLists           s_UserSessionInfoList(0);
static MRTConnManager::UserSessionInfoMaps            s_UserSessionInfoMap(0);


MRTConnManager::ModuleInfo* MRTConnManager::findModuleInfo(const std::string& userid, pms::ETransferModule module)
{
    MRTConnManager::ModuleInfo* pInfo = NULL;
    {
        OSMutexLocker locker(&s_mutexModule);
        MRTConnManager::ModuleInfoMapsIt it = s_ModuleInfoMap.begin();
        for (; it!=s_ModuleInfoMap.end(); it++) {
            if ((it->second) && (it->second)->othModuleType == module) {
                pInfo = it->second;
                break;
            }
        }
    }
    return pInfo;
}

bool MRTConnManager::ConnectConnector()
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

bool MRTConnManager::DoConnectConnector(const std::string ip, unsigned short port)
{
    MRTTransferSession* connectorSession = new MRTTransferSession(pms::ETransferModule::MCONNECTOR);
    connectorSession->Init();
    // conn to connector

    int test = 0;
    while(!connectorSession->Connect(ip, port)) {
        LI("connecting to connector server %s:%u, waiting...\n", ip.c_str(), port);
        usleep(100*1000);
        if (test++ > 30) return false;
    }
    connectorSession->EstablishConnection();
    return true;
}

bool MRTConnManager::TryConnectConnector(const std::string ip, unsigned short port)
{
    LI("MRTConnManager::TryConneectConnector ip:%s, port:%u\n", ip.c_str(), port);
    MRTTransferSession* connectorSession = new MRTTransferSession(pms::ETransferModule::MCONNECTOR);
    connectorSession->Init();
    // conn to connector

    bool ok = false;
    int times = 0;
    do{
        ok = connectorSession->Connect(ip, port);
        LI("try %d times to connect connector server %s:%u, waiting...\n", times, ip.c_str(), port);
        usleep(1000*1000);
    }while(!ok && ++times < 5);

    if (ok) {
        connectorSession->EstablishConnection();
        return true;
    } else {
        m_connectingSessList.push_back(connectorSession);
        return false;
    }
}

void MRTConnManager::RefreshConnection()
{
    {
        OSMutexLocker locker(&s_mutexModule);
        for (auto & x : s_ModuleInfoMap) {
            if (x.second && x.second->pModule->RefreshTime()) {
                x.second->pModule->KeepAlive();
            }
        }
    }
}

bool MRTConnManager::SignalKill()
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

bool MRTConnManager::ClearAll()
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

bool MRTConnManager::AddModuleInfo(MRTConnManager::ModuleInfo* pmi, const std::string& sid)
{
    OSMutexLocker locker(&s_mutexModule);
    ModuleInfoMapsIt it = s_ModuleInfoMap.find(sid);
    if (it!=s_ModuleInfoMap.end()) {
        MRTConnManager::ModuleInfo *p = it->second;
        if (p && p->othModuleType == pms::ETransferModule::MMSGQUEUE) {
            MRTRoomManager::Instance().ClearMsgQueueSession(sid);
        }
        delete p;
        p = NULL;
        s_ModuleInfoMap.erase(sid);
    }
    s_ModuleInfoMap.insert(make_pair(sid, pmi));
    return true;
}

bool MRTConnManager::DelModuleInfo(const std::string& sid, EventData& data)
{
    OSMutexLocker locker(&s_mutexModule);
    ModuleInfoMapsIt it = s_ModuleInfoMap.find(sid);
    if (it!=s_ModuleInfoMap.end()) {
        MRTConnManager::ModuleInfo *p = it->second;
        if (p && p->othModuleType == pms::ETransferModule::MMSGQUEUE) {
            MRTRoomManager::Instance().ClearMsgQueueSession(sid);
        }
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

bool MRTConnManager::AddTypeModuleSession(pms::ETransferModule module, const std::string& mid, const std::string& sid)
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

bool MRTConnManager::DelTypeModuleSession(const std::string& sid)
{
    TypeModuleSessionInfo* pInfo = NULL;
    {
        OSMutexLocker locker(&s_mutexTypeModule);
        TypeModuleSessionInfoLists::iterator it = s_TypeModuleSessionInfoList.begin();
        for (; it!=s_TypeModuleSessionInfoList.end(); it++) {
            pInfo = *it;
            std::set<std::string>::iterator sit = std::find(pInfo->sessionIds.begin(), pInfo->sessionIds.end(), sid);
            if (sit!=pInfo->sessionIds.end()) {
                pInfo->sessionIds.erase(sit);
                break;
            }
        }
    }
    return true;
}

void MRTConnManager::TransferSessionLostNotify(const std::string& sid)
{
    EventData data;
    data.mtype = SESSEVENT::_sess_lost;
    DelModuleInfo(sid, data);
    DelTypeModuleSession(sid);
}
