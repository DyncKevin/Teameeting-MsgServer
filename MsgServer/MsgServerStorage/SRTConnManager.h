//
//  SRTConnManager.h
//  MsgServerStorage
//
//  Created by hp on 12/3/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#ifndef __MsgServerStorage__SRTConnManager__
#define __MsgServerStorage__SRTConnManager__

#include <stdio.h>
#include <iostream>
#include <unordered_map>
#include <utility>
#include <list>
#include <set>
#include "OSMutex.h"
#include "RTSingleton.h"
#include "RTType.h"
#include "SRTConnDispatcher.h"
#include "RTEventTimer.h"

#include "ProtoCommon.h"

class SRTTransferSession;

class SRTConnManager : public RTSingleton< SRTConnManager >{
    friend class RTSingleton< SRTConnManager >;
public:
    typedef struct _ModuleInfo{
        int             flag;
        pms::ETransferModule  othModuleType;
        std::string     othModuleId;
        SRTTransferSession*     pModule;
        _ModuleInfo() {
            flag = 0;
            othModuleType = pms::ETransferModule::MSTORAGE;
            othModuleId = "";
            pModule = NULL;
        }
    }ModuleInfo;

    //store  module usage information
    //meet meet123456  session123456
    //msgqueue queue123456  session234567
    typedef struct _TypeModuleSessionInfo{
        pms::ETransferModule moduleType;
        std::string moduleId;
        std::set<std::string> sessionIds;
        _TypeModuleSessionInfo() {
            moduleType = pms::ETransferModule::MSTORAGE;
            moduleId = "";
            sessionIds.clear();
        }
    }TypeModuleSessionInfo;

    typedef std::unordered_map< std::string, ModuleInfo* >      ModuleInfoMaps;
    typedef ModuleInfoMaps::iterator                            ModuleInfoMapsIt;

    //<user_id, UserModuleTypeInfo>
    typedef std::list<TypeModuleSessionInfo*> TypeModuleSessionInfoLists;

    typedef std::unordered_multimap<std::string, std::string>        UserConnectorMaps;
    typedef UserConnectorMaps::iterator UserConnectorMapsIt;

    typedef std::list< SRTTransferSession* > ConnectingSessList;

    static std::string      s_cohttpIp;
    static unsigned short   s_cohttpPort;
    static std::string      s_cohttpHost;

    ModuleInfo*       findConnectorInfo(const std::string& userid);
    ModuleInfo*       findModuleInfo(const std::string& userid, pms::ETransferModule module);
    ModuleInfo*       findModuleInfoBySid(const std::string& sid);
    ModuleInfo*       findConnectorInfoById(const std::string& userid, const std::string& connector);

    bool AddModuleInfo(ModuleInfo* pmi, const std::string& sid);
    bool DelModuleInfo(const std::string& sid, EventData& data);
    bool AddTypeModuleSession(pms::ETransferModule module, const std::string& mid, const std::string& sid);
    bool DelTypeModuleSession(const std::string& sid);

    void TransferSessionLostNotify(const std::string& sid);

    bool    ConnectConnector();
    bool TryConnectConnector(const std::string ip, unsigned short port);
    std::list<std::string>* GetAddrsList() { return &m_ipList; }
    void    RefreshConnection();
    void    SendTransferData(const std::string mid, const std::string uid, const std::string msg);
    void SetStorageId(const std::string& mid) { m_sequenceId = mid; }
    std::string& StorageId() { return m_sequenceId; }
    bool    SignalKill();
    bool    ClearAll();

    void OnTLogin(const std::string& uid, const std::string& token, const std::string& connector);
    void OnTLogout(const std::string& uid, const std::string& token, const std::string& connector);

    void ProcessRecvEvent(const char*pData, int nLen);
    void ProcessTickEvent(const char*pData, int nLen);
    void PostDataStatic(const char* pData, int nLen);

    // for RTEventTimer
    static int DispTimerCallback(const char*pData, int nLen);
protected:
    SRTConnManager():m_pConnDispatcher(NULL) { }
    ~SRTConnManager() { }
private:
    bool DoConnectConnector(const std::string ip, unsigned short port);
    std::list<std::string>    m_ipList;
    std::string               m_sequenceId;
    OSMutex                   m_mutexMembers;
    SRTConnDispatcher*        m_pConnDispatcher;
    UserConnectorMaps         m_userConnectors;
    ConnectingSessList        m_connectingSessList;
};

#endif /* defined(__MsgServerStorage__SRTConnManager__) */
