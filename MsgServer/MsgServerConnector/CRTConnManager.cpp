//
//  CRTConnManager.cpp
//  dyncRTConnector
//
//  Created by hp on 12/3/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#include "CRTConnManager.h"
#include <assert.h>
#include "OSMutex.h"
#include <algorithm>

static OSMutex          s_mutex;
static OSMutex          s_mutexModule;
static OSMutex          s_mutexConnection;
static OSMutex          s_mutexTypeModule;

static CRTConnManager::ModuleInfoMaps               s_ModuleInfoMap(0);
static CRTConnManager::ConnectionInfoMaps           s_ConnectionInfoMap(0);
static CRTConnManager::TypeModuleSessionInfoLists   s_TypeModuleSessionInfoList(0);
static CRTConnManager::UserSessionInfoLists         s_UserSessionInfoList(0);
static CRTConnManager::UserSessionInfoMaps          s_UserSessionInfoMap(0);


CRTConnManager::ModuleInfo* CRTConnManager::findModuleInfo(const std::string& userid, pms::ETransferModule module)
{
    CRTConnManager::ModuleInfo *pInfo = NULL;
    {
        OSMutexLocker locker(&s_mutexModule);
        CRTConnManager::ModuleInfoMapsIt it = s_ModuleInfoMap.begin();
        for (; it!=s_ModuleInfoMap.end(); it++) {
            if (it->second && it->second->othModuleType == module) {
                pInfo = it->second;
                break;
            }
        }
    }
    return pInfo;
}

CRTConnManager::ConnectionInfo* CRTConnManager::findConnectionInfoById(const std::string& uid)
{
    CRTConnManager::ConnectionInfo* pInfo = NULL;
    {
        OSMutexLocker locker(&s_mutexConnection);
        CRTConnManager::ConnectionInfoMapsIt it = s_ConnectionInfoMap.find(uid);
        if (it!=s_ConnectionInfoMap.end()) {
            pInfo = it->second;
        }
    }
    return pInfo;
}

bool CRTConnManager::AddModuleInfo(CRTConnManager::ModuleInfo* pmi, const std::string& sid)
{
    OSMutexLocker locker(&s_mutexModule);
    CRTConnManager::ModuleInfoMapsIt it = s_ModuleInfoMap.find(sid);
    if (it!=s_ModuleInfoMap.end()) {
        CRTConnManager::ModuleInfo *p = it->second;
        delete p;
        p = NULL;
        s_ModuleInfoMap.erase(sid);
    }
    s_ModuleInfoMap.insert(make_pair(sid, pmi));
    return true;
}

bool CRTConnManager::DelModuleInfo(const std::string& sid)
{
    OSMutexLocker locker(&s_mutexModule);
    CRTConnManager::ModuleInfoMapsIt it = s_ModuleInfoMap.find(sid);
    if (it!=s_ModuleInfoMap.end()) {
        CRTConnManager::ModuleInfo *p = it->second;
        delete p;
        p = NULL;
        s_ModuleInfoMap.erase(sid);
    }
    return true;
}

bool CRTConnManager::AddTypeModuleSession(pms::ETransferModule module, const std::string& mid, const std::string& sid)
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

bool CRTConnManager::DelTypeModuleSession(const std::string& sid)
{
    bool found = false;
    TypeModuleSessionInfo* pInfo = NULL;
    {
        OSMutexLocker locker(&s_mutexTypeModule);
        TypeModuleSessionInfoLists::iterator it = s_TypeModuleSessionInfoList.begin();
        for (; it!=s_TypeModuleSessionInfoList.end(); it++) {
            pInfo = *it;
            std::set<std::string>::iterator sit = std::find(pInfo->sessionIds.begin(), pInfo->sessionIds.end(), sid);
            if (sit!=pInfo->sessionIds.end()) {
                pInfo->sessionIds.erase(sit);
                found = true;
                break;
            }
        }
    }
    return found;
}

bool CRTConnManager::AddUser(pms::EConnType type, const std::string& uid, CRTConnManager::ConnectionInfo* pInfo)
{
    if (uid.length()==0 || !pInfo) return false;
    {
        OSMutexLocker locker(&s_mutexConnection);
        CRTConnManager::ConnectionInfoMapsIt it = s_ConnectionInfoMap.find(uid);
        if (it!=s_ConnectionInfoMap.end()) {
            ConnectionLostNotify(uid, it->second->_token);
            delete it->second;
            it->second = NULL;
            s_ConnectionInfoMap.erase(uid);
        }
        s_ConnectionInfoMap.insert(make_pair(uid, pInfo));
    }
    return true;
}

bool CRTConnManager::DelUser(pms::EConnType type, const std::string& uid, std::string& token)
{
    if (uid.length()==0) return false;
    {
        OSMutexLocker locker(&s_mutexConnection);
        CRTConnManager::ConnectionInfoMapsIt it = s_ConnectionInfoMap.find(uid);
        if (it!=s_ConnectionInfoMap.end()) {
            token = it->second->_token;
            delete it->second;
            it->second = NULL;
            s_ConnectionInfoMap.erase(uid);
            return true;
        } else {
            return false;
        }
    }
}

void CRTConnManager::ConnectionLostNotify(const std::string& uid, const std::string& token)
{
    ModuleInfo* pmi = findModuleInfo(uid, pms::ETransferModule::MLIVE);
    if (pmi && pmi->pModule) {
        pmi->pModule->ConnectionLostNotify(uid, token);
    } else {
        LE("ConnectionLostNotify not find mlive module!!!\n");
        Assert(false);
    }
    pmi = findModuleInfo(uid, pms::ETransferModule::MMSGQUEUE);
    if (pmi && pmi->pModule) {
        LI("ConnectionLostNotify uid:%s\n", uid.c_str());
        pmi->pModule->ConnectionLostNotify(uid, token);
    } else {
        LE("ConnectionLostNotify not find msgqueue module!!!\n");
        Assert(false);
    }
}

void CRTConnManager::ConnectionConnNotify(const std::string& uid, const std::string& token)
{
    ModuleInfo* pmi = findModuleInfo(uid, pms::ETransferModule::MLIVE);
    if (pmi && pmi->pModule) {
        pmi->pModule->ConnectionConnNotify(uid, token);
    } else {
        LE("ConnectionConnNotify not find MLIVE module!!!\n");
        Assert(false);
    }
    pmi = findModuleInfo(uid, pms::ETransferModule::MMSGQUEUE);
    if (pmi && pmi->pModule) {
        LI("ConnectionConnNotify uid:%s\n", uid.c_str());
        pmi->pModule->ConnectionConnNotify(uid, token);
    } else {
        LE("ConnectionConnNotify not find  msgqueue module!!!\n");
        Assert(false);
    }
}

void CRTConnManager::TransferSessionLostNotify(const std::string& sid)
{
    DelModuleInfo(sid);
    DelTypeModuleSession(sid);
}

void CRTConnManager::TransferMsg(pms::EServerCmd cmd, pms::EModuleType module, const std::string& uid, const std::string& msg)
{
    ModuleInfo* pmi = findModuleInfo(uid, (pms::ETransferModule)module);
    if (pmi && pmi->pModule) {
        pmi->pModule->TransferMsg(cmd, msg);
    } else {
        LE("TransferMsg module type:%d is NULL\n", (int)module);
        Assert(false);
        return;
    }
}

void CRTConnManager::TransferToPusher(pms::EServerCmd cmd, pms::EModuleType type, const std::string& uid, const std::string& msg)
{
    CRTConnManager::ModuleInfo* pmodule = CRTConnManager::Instance().findModuleInfo("", pms::ETransferModule::MPUSHER);
    if (pmodule && pmodule->pModule && pmodule->pModule->IsLiveSession()) {

        // msg is pms::MsgReq->pms::Setting
        pms::RelayMsg r_msg;
        pms::TransferMsg t_msg;

        r_msg.set_svr_cmds(cmd);
        r_msg.set_tr_module(pms::ETransferModule::MCONNECTOR);
        r_msg.set_connector(CRTConnManager::Instance().ConnectorId());
        r_msg.set_content(msg);

        if (cmd==pms::EServerCmd::CUPDATESETTING)
        {
            t_msg.set_type(pms::ETransferType::TPUSH);
        } else if (cmd==pms::EServerCmd::CLOGIN)
        {
            t_msg.set_type(pms::ETransferType::TLOGIN);
        } else {
            return;
        }
        t_msg.set_content(r_msg.SerializeAsString());

        // t_msg is pms::TransferMsg->pms::RelayMsg->pms::MsgReq->pms::Setting
        std::string s = t_msg.SerializeAsString();
        pmodule->pModule->SendTransferData(s.c_str(), (int)s.length());
        //LI("CRTConnManager::TransferToPusher send push msg to pusher, module type:%d, module id:%s!!!\n\n", pmodule->othModuleType, pmodule->othModuleId.c_str());
    } else {
        //LE("CRTConnManager::TransferToPusher module pusher is not liveeeeeeeeeeee!!!\n");
    }
    return;
}

void CRTConnManager::Init(const std::string redisIp, int redisPort)
{
    m_xRedis.Init(redisIp, redisPort);
}

void CRTConnManager::Unin()
{
    m_xRedis.Unin();
}

bool CRTConnManager::GetEnablePush(const std::string& userid, pms::EModuleType type)
{
    std::string enablepush;
    if (m_xRedis.GetSettingPush(userid, type, "enablepush", enablepush))
    {
        LI("CRTConnManager::GetEnablePush enablepush:%s\n", enablepush.c_str());
        if (enablepush.compare("1")==0)
        {
            return true;
        } else {
            return false;
        }
    } else {
        return false;
    }
}

bool CRTConnManager::CouldPush(const std::string& userid, pms::EModuleType type)
{
    std::string enablepush;
    std::string mutenotify;
    if (m_xRedis.GetSettingPush(userid, type, "enablepush", enablepush, "mutenotify", mutenotify))
    {
        LI("CRTConnManager::GetEnablePush enablepush:%s, mutenotify:%s\n", enablepush.c_str(), mutenotify.c_str());
        if (enablepush.compare("2")!=0 && mutenotify.compare("1")!=0)
        {
            return true;
        } else {
            return false;
        }
    } else {
        return false;
    }
}


bool CRTConnManager::SignalKill()
{
    {
        OSMutexLocker mlocker(&s_mutexModule);
        for (auto & x : s_ModuleInfoMap) {
            x.second->pModule->Signal(Task::kKillEvent);
            usleep(100*1000);
        }
    }

    {
        OSMutexLocker clocker(&s_mutexConnection);
        for (auto & x : s_ConnectionInfoMap) {
            x.second->_pConn->Signal(Task::kKillEvent);
            usleep(100*1000);
        }
    }

    return true;
}

bool CRTConnManager::ClearAll()
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

    {
        OSMutexLocker clocker(&s_mutexConnection);
        for (auto & x : s_ConnectionInfoMap) {
            delete x.second;
            x.second = NULL;
            usleep(100*1000);
        }
        s_ConnectionInfoMap.clear();
    }
     return true;
}


