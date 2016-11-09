//
//  LRTConnManager.h
//  MsgServerLogical
//
//  Created by hp on 12/3/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#ifndef __MsgServerLogical__LRTConnManager__
#define __MsgServerLogical__LRTConnManager__

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

class LRTTransferSession;

class LRTConnManager : public RTSingleton< LRTConnManager >{
    friend class RTSingleton< LRTConnManager >;
public:
    typedef struct _ModuleInfo{
        int             flag;
        pms::ETransferModule  othModuleType;
        std::string     othModuleId;
        LRTTransferSession*     pModule;
        _ModuleInfo() {
            flag = 0;
            othModuleType = pms::ETransferModule::MLOGICAL;
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
            moduleType = pms::ETransferModule::MLOGICAL;
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

    typedef std::list< LRTTransferSession* > ConnectingSessList;

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

    bool    ConnectSequence();
    bool    ConnectStorage();

    void PushSeqnReadMsg(const std::string& smsg);
    void PushSeqnWriteMsg(const std::string& smsg);
    void PushStoreReadMsg(const std::string& srmsg);
    void PushStoreWriteMsg(const std::string& swmsg);

    std::list<std::string>* GetSequenceAddrList() { return &m_sequenceAddrList; }
    std::list<std::string>* GetStorageAddrList() { return &m_storageAddrList; }
    void SetSvrSequence(bool ok) { m_isSvrSequenceOk = ok; }
    void SetSvrStorage(bool ok) { m_isSvrStorageOk = ok; }
    bool IsSvrSequence() { return m_isSvrSequenceOk; }
    bool IsSvrStorage() { return m_isSvrStorageOk; }

    void SetSequenceReadSessId(const std::string& sid) { m_sequenceReadSessId = sid; }
    void SetSequenceWriteSessId(const std::string& sid) { m_sequenceWriteSessId = sid; }
    void SetStorageReadSessId(const std::string& sid) { m_storageReadSessId = sid; }
    void SetStorageWriteSessId(const std::string& sid) { m_storageWriteSessId = sid; }

    void    RefreshConnection();

    void SetLogicalId(const std::string& mid) { m_logicalId = mid; }
    std::string& LogicalId() { return m_logicalId; }
    bool    SignalKill();
    bool    ClearAll();

    void OnTLogin(const std::string& uid, const std::string& token, const std::string& connector);
    void OnTLogout(const std::string& uid, const std::string& token, const std::string& connector);

protected:
    LRTConnManager() { }
    ~LRTConnManager() { }
private:
    bool DoConnectSequence(const std::string ip, unsigned short port);
    bool DoConnectStorage(const std::string ip, unsigned short port);
    bool                      m_isSvrSequenceOk;
    bool                      m_isSvrStorageOk;
    std::list<std::string>    m_sequenceAddrList;
    std::list<std::string>    m_storageAddrList;
    std::string               m_logicalId;
    OSMutex                   m_mutexMembers;
    UserConnectorMaps         m_userConnectors;

    std::string                 m_sequenceWriteSessId;
    std::string                 m_sequenceReadSessId;
    std::string                 m_storageWriteSessId;
    std::string                 m_storageReadSessId;
};

#endif /* defined(__MsgServerLogical__LRTConnManager__) */
