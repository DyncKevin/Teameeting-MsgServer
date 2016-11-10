//
//  MRTConnManager.h
//  MsgServerModule
//
//  Created by hp on 12/3/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#ifndef __MsgServerModule__MRTConnManager__
#define __MsgServerModule__MRTConnManager__

#include <stdio.h>
#include <iostream>
#include <unordered_map>
#include <utility>
#include <list>
#include <set>
#include "OSMutex.h"
#include "RTSingleton.h"
#include "RTType.h"
#include "RTEventTimer.h"

#include "ProtoCommon.h"

class MRTTransferSession;

class MRTConnManager : public RTSingleton< MRTConnManager >{
    friend class RTSingleton< MRTConnManager >;
public:
    typedef struct _ModuleInfo{
        int             flag;
        pms::ETransferModule  othModuleType;
        std::string     othModuleId;
        MRTTransferSession*     pModule;
        _ModuleInfo() {
            flag = 0;
            // when meet compile error here, please comment this line or add TransferModule to replace MMODULE
            othModuleType = pms::ETransferModule::MMODULE;
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
            // when meet compile error here, please comment this line or add TransferModule to replace MMODULE
            moduleType = pms::ETransferModule::MMODULE;
            moduleId = "";
            sessionIds.clear();
        }
    }TypeModuleSessionInfo;

    typedef std::unordered_map< std::string, ModuleInfo* >      ModuleInfoMaps;
    typedef ModuleInfoMaps::iterator                            ModuleInfoMapsIt;
    //<user_id, UserModuleTypeInfo>
    typedef std::list<TypeModuleSessionInfo*>                   TypeModuleSessionInfoLists;
    typedef std::unordered_multimap<std::string, std::string>   UserConnectorMaps;
    typedef UserConnectorMaps::iterator                         UserConnectorMapsIt;
    typedef std::list< MRTTransferSession* >                    ConnectingSessList;

    ///////////////////////////////////////////////////////////////////////////////////

    ModuleInfo*       findConnectorInfo(const std::string& userid);
    ModuleInfo*       findModuleInfo(const std::string& userid, pms::ETransferModule module);
    ModuleInfo*       findModuleInfoBySid(const std::string& sid);
    ModuleInfo*       findConnectorInfoById(const std::string& userid, const std::string& connector);

    bool AddModuleInfo(ModuleInfo* pmi, const std::string& sid);
    bool DelModuleInfo(const std::string& sid, EventData& data);
    bool AddTypeModuleSession(pms::ETransferModule module, const std::string& mid, const std::string& sid);
    bool DelTypeModuleSession(const std::string& sid);

    void TransferSessionLostNotify(const std::string& sid);

    ///////////////////////////////////////////////////////////////////////////////////

    bool ConnectSequence();
    bool ConnectStorage();

    void RefreshConnection();

    void OnTLogin(const std::string& uid, const std::string& token, const std::string& connector);
    void OnTLogout(const std::string& uid, const std::string& token, const std::string& connector);

    bool SignalKill();
    bool ClearAll();

    void SetModuleId(const std::string& mid) { m_logicalId = mid; }
    std::string& ModuleId() { return m_logicalId; }
    std::list<std::string>* GetSequenceAddrList() { return &m_sequenceAddrList; }
    std::list<std::string>* GetStorageAddrList() { return &m_storageAddrList; }

protected:
    MRTConnManager() { }
    ~MRTConnManager() { }

private:
    bool DoConnectSequence(const std::string ip, unsigned short port);
    bool DoConnectStorage(const std::string ip, unsigned short port);

private:
    std::list<std::string>    m_sequenceAddrList;
    std::list<std::string>    m_storageAddrList;
    std::string               m_logicalId;
    OSMutex                   m_mutexMembers;
    UserConnectorMaps         m_userConnectors;
};

#endif /* defined(__MsgServerModule__MRTConnManager__) */
