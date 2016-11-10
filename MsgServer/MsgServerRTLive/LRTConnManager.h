//
//  LRTConnManager.h
//  MsgServerRTLive
//
//  Created by hp on 12/3/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#ifndef __MsgServerRTLive__LRTConnManager__
#define __MsgServerRTLive__LRTConnManager__

#include <stdio.h>
#include <iostream>
#include <unordered_map>
#include <utility>
#include <list>
#include <set>
#include "OSMutex.h"
#include "RTSingleton.h"
#include "RTTcp.h"
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
            othModuleType = pms::ETransferModule::MLIVE;
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
            moduleType = pms::ETransferModule::MLIVE;
            moduleId = "";
            sessionIds.clear();
        }
    }TypeModuleSessionInfo;

    //store the user id and connection
    typedef struct _ConnectionInfo{
        std::string     _connId;
        std::string     _connAddr;
        std::string     _connPort;
        std::string     _userId;
        std::string     _token;
        RTTcp*          _pConn;
        pms::EConnType  _connType;
        int             _flag;
        _ConnectionInfo():_connId("")
                        , _connAddr("")
                        , _connPort("")
                        , _userId("")
                        , _token("")
                        , _pConn(NULL)
                        , _connType(pms::EConnType::TTCP)
                        , _flag(0){
        }
    }ConnectionInfo;

    typedef std::unordered_map< std::string, ModuleInfo* >      ModuleInfoMaps;
    typedef ModuleInfoMaps::iterator                            ModuleInfoMapsIt;
    //<user_id, UserModuleTypeInfo>
    typedef std::list<TypeModuleSessionInfo*>                   TypeModuleSessionInfoLists;
    typedef std::unordered_multimap<std::string, std::string>   UserConnectorMaps;
    typedef UserConnectorMaps::iterator                         UserConnectorMapsIt;
    typedef std::list< LRTTransferSession* >                    ConnectingSessList;
    //<user_id, connection_info>
    typedef std::unordered_map< std::string, ConnectionInfo* >  ConnectionInfoMaps;
    typedef ConnectionInfoMaps::iterator                        ConnectionInfoMapsIt;

    ///////////////////////////////////////////////////////////////////////////////////

    ModuleInfo* findConnModuleInfo(const std::string& userid);
    ModuleInfo* findModuleInfo(const std::string& userid, pms::ETransferModule module);
    ModuleInfo* findModuleInfoBySid(const std::string& sid);
    ModuleInfo* findModuleInfoByMid(const std::string& userid, const std::string& connector);
    ConnectionInfo* findConnectionInfoById(const std::string& uid);

    bool AddModuleInfo(ModuleInfo* pmi, const std::string& sid);
    bool DelModuleInfo(const std::string& sid, EventData& data);
    bool AddTypeModuleSession(pms::ETransferModule module, const std::string& mid, const std::string& sid);
    bool DelTypeModuleSession(const std::string& sid);

    bool AddUser(pms::EConnType type, const std::string& uid, ConnectionInfo* pInfo);
    // return true means delete one; false means not delete
    bool DelUser(pms::EConnType type, const std::string& uid, std::string& token);
    bool SendToGroupModule(const std::string& userid, const std::string& msg);
    bool SendToPushModule(const std::string& userid, const std::string& msg);

    void TransferSessionLostNotify(const std::string& sid);

    ///////////////////////////////////////////////////////////////////////////////////

    void InitManager();
    void UninManager();

    bool ConnectLogical();
    bool ConnectConnector();
    bool ConnectDispatcher();

    void RefreshConnection();
    void ReportError(pms::ETransferModule module, const std::string& uid, const std::string& err, int code);
    bool Transfer2Dispatcher(const std::string mid, const std::string uid, const std::string msg);

    bool PushNewMsg2Queue(const std::string& str);
    bool PushSeqnReq2Queue(const std::string& str);
    bool PushDataReq2Queue(const std::string& str);

    void OnTLogin(const std::string& uid, const std::string& token, const std::string& connector);
    void OnTLogout(const std::string& uid, const std::string& token, const std::string& connector);

    bool SignalKill();
    bool ClearAll();

    std::list<std::string>* GetLogicalAddrList() { return &m_logicalAddrList; }
    std::list<std::string>* GetConnectorAddrList() { return &m_connectorAddrList; }
    std::list<std::string>* GetDispatcherAddrList() { return &m_dispatcherAddrList; }

    void SetSvrLogical(bool ok) { m_isSvrLogicalOk = ok; }
    void SetSvrConnector(bool ok) { m_isSvrConnectorOk = ok; }
    void SetSvrDispatcher(bool ok) { m_isSvrDispatcherOk = ok; }
    bool IsSvrLogical() { return m_isSvrLogicalOk; }
    bool IsSvrConnector() { return m_isSvrConnectorOk; }
    bool IsSvrDispatcher() { return m_isSvrDispatcherOk; }

    void SetSvrSequence(bool ok) { m_isSvrSequenceOk = ok; }
    void SetSvrStorage(bool ok) { m_isSvrStorageOk = ok; }
    bool IsSvrSequence() { return m_isSvrSequenceOk; }
    bool IsSvrStorage() { return m_isSvrStorageOk; }

    void SetDispatcherSessId(const std::string& sid) { m_dispatcherSessId = sid; }
    void SetLogicalSessId(const std::string& sid) {m_logicalSessId = sid; }

    void SetRTLiveId(const std::string& mid) { m_rtliveId = mid; }
    std::string& RTLiveId() { return m_rtliveId; }

protected:
    LRTConnManager() { }
    ~LRTConnManager() { }

private:
    bool DoConnectLogical(const std::string ip, unsigned short port);
    bool DoConnectConnector(const std::string ip, unsigned short port);
    bool DoConnectDispatcher(const std::string ip, unsigned short port);

private:
    bool                      m_isSvrLogicalOk;
    bool                      m_isSvrConnectorOk;
    bool                      m_isSvrDispatcherOk;

    // this two for system type error
    bool                      m_isSvrSequenceOk;
    bool                      m_isSvrStorageOk;

    std::list<std::string>    m_logicalAddrList;
    std::list<std::string>    m_connectorAddrList;
    std::list<std::string>    m_dispatcherAddrList;

    std::string               m_rtliveId;
    OSMutex                   m_mutexMembers;
    UserConnectorMaps         m_userConnectors;

    std::string               m_dispatcherSessId;
    std::string               m_logicalSessId;

};

#endif /* defined(__MsgServerRTLive__LRTConnManager__) */
