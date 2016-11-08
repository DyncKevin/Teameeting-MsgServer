//
//  RTConnectionManager.cpp
//  dyncRTLogical
//
//  Created by hp on 12/3/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#include "LRTConnManager.h"
#include <algorithm>
#include <assert.h>
#include <json/json.h>
#include "OSMutex.h"
#include "LRTTransferSession.h"
#include "RTZKClient.hpp"
#include "ProtoCommon.h"

std::string     LRTConnManager::s_cohttpIp;
unsigned short  LRTConnManager::s_cohttpPort;
std::string     LRTConnManager::s_cohttpHost;

static OSMutex       s_mutex;
static OSMutex       s_mutexModule;
static OSMutex       s_mutexTypeModule;

static LRTConnManager::ModuleInfoMaps                 s_ModuleInfoMap(0);
static LRTConnManager::TypeModuleSessionInfoLists     s_TypeModuleSessionInfoList(0);


LRTConnManager::ModuleInfo* LRTConnManager::findConnectorInfo(const std::string& userid)
{
    return findModuleInfo(userid, pms::ETransferModule::MCONNECTOR);
}

LRTConnManager::ModuleInfo* LRTConnManager::findModuleInfo(const std::string& uid, pms::ETransferModule module)
{
    LRTConnManager::ModuleInfo *pInfo = NULL;
    {
        OSMutexLocker locker(&s_mutexModule);
        LRTConnManager::ModuleInfoMapsIt it = s_ModuleInfoMap.begin();
        for (; it!=s_ModuleInfoMap.end(); it++) {
            if (it->second && it->second->othModuleType == module) {
                pInfo = it->second;
                break;
            }
        }
    }
    return pInfo;
}

LRTConnManager::ModuleInfo* LRTConnManager::findModuleInfoBySid(const std::string& sid)
{

    LRTConnManager::ModuleInfo *pInfo = NULL;
    {
        OSMutexLocker locker(&s_mutexModule);
        LRTConnManager::ModuleInfoMapsIt it = s_ModuleInfoMap.find(sid);
        if (it!=s_ModuleInfoMap.end()) {
            pInfo = it->second;
        }
    }
    return pInfo;
}

LRTConnManager::ModuleInfo* LRTConnManager::findConnectorInfoById(const std::string& userid, const std::string& connector)
{

    if (userid.length()==0 || connector.length()==0) {
        LE("findConnectorInfoById userid or connector is 0\n");
        return NULL;
    }
    LRTConnManager::ModuleInfo* pInfo = NULL;
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

bool LRTConnManager::ConnectSequence()
{
    if (m_sequenceAddrList.size() == 0) {
        return false;
    }
    std::list<std::string>::iterator it;
    for (it=m_sequenceAddrList.begin(); it!=m_sequenceAddrList.end(); it++) {
        std::string s = *it;
        char ip[16] = {0};
        unsigned int port = 0;
        sscanf(s.c_str(), "%s %u", ip, &port);
        LI("ip:%s, port:%u\n", ip, port);
        if (strlen(ip)>0 && port > 2048) {
            DoConnectSequence(ip, port);
        }
    }
    return true;
}

bool LRTConnManager::ConnectStorage()
{
    if (m_storageAddrList.size() == 0) {
        return false;
    }
    std::list<std::string>::iterator it;
    for (it=m_storageAddrList.begin(); it!=m_storageAddrList.end(); it++) {
        std::string s = *it;
        char ip[16] = {0};
        unsigned int port = 0;
        sscanf(s.c_str(), "%s %u", ip, &port);
        LI("ip:%s, port:%u\n", ip, port);
        if (strlen(ip)>0 && port > 2048) {
            DoConnectStorage(ip, port);
        }
    }
    return true;
}

bool LRTConnManager::DoConnectSequence(const std::string ip, unsigned short port)
{
    m_sequenceWriteSession = new LRTTransferSession();
    m_sequenceWriteSession->Init();
    // conn to connector
    while (!m_sequenceWriteSession->Connect(ip, port)) {
        LI("connecting to sequence server write %s:%u waiting...\n", ip.c_str(), port);
        usleep(100*1000);
    }
    LI("%s port:%u, socketFD:%d\n", __FUNCTION__, port, m_sequenceWriteSession->GetSocket()->GetSocketFD());
    m_sequenceWriteSession->EstablishConnection();

    m_sequenceReadSession = new LRTTransferSession();
    m_sequenceReadSession->Init();
    // conn to connector
    while (!m_sequenceReadSession->Connect(ip, port)) {
        LI("connecting to sequence server read %s:%u waiting...\n", ip.c_str(), port);
        usleep(100*1000);
    }
    LI("%s port:%u, socketFD:%d\n", __FUNCTION__, port, m_sequenceReadSession->GetSocket()->GetSocketFD());
    m_sequenceReadSession->EstablishConnection();
    return true;
}

bool LRTConnManager::DoConnectStorage(const std::string ip, unsigned short port)
{
    m_storageWriteSession = new LRTTransferSession();
    m_storageWriteSession->Init();
    // conn to connector
    while (!m_storageWriteSession->Connect(ip, port)) {
        LI("connecting to storage server %s:%u waiting...\n", ip.c_str(), port);
        usleep(100*1000);
    }
    LI("%s Storage Write port:%u, socketFD:%d\n", __FUNCTION__, port, m_storageWriteSession->GetSocket()->GetSocketFD());
    m_storageWriteSession->EstablishConnection();

    m_storageReadSession = new LRTTransferSession();
    m_storageReadSession->Init();
    // conn to connector
    while (!m_storageReadSession->Connect(ip, port)) {
        LI("connecting to storage server %s:%u waiting...\n", ip.c_str(), port);
        usleep(100*1000);
    }
    LI("%s Storage Read port:%u, socketFD:%d\n", __FUNCTION__, port, m_storageReadSession->GetSocket()->GetSocketFD());
    m_storageReadSession->EstablishConnection();
    return true;
}

bool LRTConnManager::TryConnectSequence(const std::string ip, unsigned short port)
{
    LI("MRTConnManager::TryConneectSequence ip:%s, port:%u\n", ip.c_str(), port);
    //TODO:
    //check m_sequenceWriteSession
    m_sequenceWriteSession = new LRTTransferSession();
    m_sequenceWriteSession->Init();
    // conn to connector

    bool ok = false;
    int times = 0;
    do{
        ok = m_sequenceWriteSession->Connect(ip, port);
        LI("try %d times to connect sequence server write %s:%u, waiting...\n", times, ip.c_str(), port);
        usleep(1000*1000);
    }while(!ok && ++times < 5);

    if (ok) {
        m_sequenceWriteSession->EstablishConnection();
        return true;
    } else {
        m_connectingSessList.push_back(m_sequenceWriteSession);
        return false;
    }

    //check m_sequenceReadSession
    m_sequenceReadSession = new LRTTransferSession();
    m_sequenceReadSession->Init();
    // conn to connector

    do{
        ok = m_sequenceReadSession->Connect(ip, port);
        LI("try %d times to connect sequence server read %s:%u, waiting...\n", times, ip.c_str(), port);
        usleep(1000*1000);
    }while(!ok && ++times < 5);

    if (ok) {
        m_sequenceReadSession->EstablishConnection();
        return true;
    } else {
        m_connectingSessList.push_back(m_sequenceReadSession);
        return false;
    }
}

bool LRTConnManager::TryConnectStorage(const std::string ip, unsigned short port)
{
    LI("MRTConnManager::TryConneectStorage ip:%s, port:%u\n", ip.c_str(), port);
    //TODO:
    //check m_storageReadSession m_storageWriteSession
    m_storageWriteSession = new LRTTransferSession();
    m_storageWriteSession->Init();
    // conn to connector

    bool ok = false;
    int times = 0;
    do{
        ok = m_storageWriteSession->Connect(ip, port);
        LI("try %d times to connect storage server %s:%u, waiting...\n", times, ip.c_str(), port);
        usleep(1000*1000);
    }while(!ok && ++times < 5);

    if (ok) {
        m_storageWriteSession->EstablishConnection();
        return true;
    } else {
        m_connectingSessList.push_back(m_storageWriteSession);
        return false;
    }

    m_storageReadSession = new LRTTransferSession();
    m_storageReadSession->Init();
    // conn to connector

    do{
        ok = m_storageReadSession->Connect(ip, port);
        LI("try %d times to connect storage server %s:%u, waiting...\n", times, ip.c_str(), port);
        usleep(1000*1000);
    }while(!ok && ++times < 5);

    if (ok) {
        m_storageReadSession->EstablishConnection();
        return true;
    } else {
        m_connectingSessList.push_back(m_storageReadSession);
        return false;
    }
}

void LRTConnManager::PushSeqnReadMsg(const std::string& smsg)
{
    if (m_sequenceReadSession && m_sequenceReadSession->IsLiveSession())
        m_sequenceReadSession->SendTransferData(smsg);
}

void LRTConnManager::PushSeqnWriteMsg(const std::string& smsg)
{
    if (m_sequenceWriteSession && m_sequenceWriteSession->IsLiveSession())
        m_sequenceWriteSession->SendTransferData(smsg);
}

void LRTConnManager::PushStoreReadMsg(const std::string& srmsg)
{
    if (m_storageReadSession && m_storageReadSession->IsLiveSession())
        m_storageReadSession->SendTransferData(srmsg);
}

void LRTConnManager::PushStoreWriteMsg(const std::string& swmsg)
{
    if (m_storageWriteSession && m_storageWriteSession->IsLiveSession())
        m_storageWriteSession->SendTransferData(swmsg);
}

void LRTConnManager::RefreshConnection()
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

bool LRTConnManager::SignalKill()
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

bool LRTConnManager::ClearAll()
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
    m_sequenceAddrList.clear();
    m_storageAddrList.clear();
     return true;
}

bool LRTConnManager::AddModuleInfo(LRTConnManager::ModuleInfo* pmi, const std::string& sid)
{
    OSMutexLocker locker(&s_mutexModule);
    LRTConnManager::ModuleInfoMapsIt it = s_ModuleInfoMap.find(sid);
    if (it!=s_ModuleInfoMap.end()) {
        LRTConnManager::ModuleInfo *p = it->second;
        delete p;
        p = NULL;
        s_ModuleInfoMap.erase(sid);
    }
    s_ModuleInfoMap.insert(make_pair(sid, pmi));
    return true;
}

bool LRTConnManager::DelModuleInfo(const std::string& sid, EventData& data)
{
    OSMutexLocker locker(&s_mutexModule);
    LRTConnManager::ModuleInfoMapsIt it = s_ModuleInfoMap.find(sid);
    if (it!=s_ModuleInfoMap.end()) {
        LRTConnManager::ModuleInfo *p = it->second;
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

bool LRTConnManager::AddTypeModuleSession(pms::ETransferModule module, const std::string& mid, const std::string& sid)
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

bool LRTConnManager::DelTypeModuleSession(const std::string& sid)
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

void LRTConnManager::TransferSessionLostNotify(const std::string& sid)
{
    EventData data;
    data.mtype = SESSEVENT::_sess_lost;
    DelModuleInfo(sid, data);
    DelTypeModuleSession(sid);
}

void LRTConnManager::OnTLogin(const std::string& uid, const std::string& token, const std::string& connector)
{

}

void LRTConnManager::OnTLogout(const std::string& uid, const std::string& token, const std::string& connector)
{

}
