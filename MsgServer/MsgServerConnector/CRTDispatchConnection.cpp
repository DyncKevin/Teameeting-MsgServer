//
//  CRTDispatchConnection.cpp
//  dyncRTConnector
//
//  Created by hp on 12/8/15.
//  Copyright (c) 2015 hp. All rights reserved.
//

#include "CRTDispatchConnection.h"
#include "CRTConnManager.h"
#include "CRTConnection.h"
#include "CRTConnectionTcp.h"
#include "rtklog.h"

#include <sys/time.h>


void CRTDispatchConnection::DispatchMsg(const std::string& uid, pms::RelayMsg& r_msg)
{
    //find connector
    CRTConnManager::ConnectionInfo* pci = CRTConnManager::Instance().findConnectionInfoById(uid);
    if (!pci) {
        // not set push in this msg
        if (r_msg.handle_cmd().length()==0 \
                || r_msg.handle_cmd().compare("push")!=0 \
                || r_msg.handle_data().length()==0 \
                ||  r_msg.handle_data().compare("1")!=0)
        {
            //LE("CRTDispatchConnection::DispatchMsg this type of message is no need to push, so return\n");
            return;
        }

        // user set not accept push
        // user set mute notification
        if (!CRTConnManager::Instance().CouldPush(uid, r_msg.cont_module()))
        {
            //LE("CRTDispatchConnection::DispatchMsg user set do not accept push or mute notify, so return\n");
            return;
        }
        // get redis setting enablepush
        // find pusher module and sent to pusher
        CRTConnManager::ModuleInfo* pmodule = CRTConnManager::Instance().findModuleInfo("", pms::ETransferModule::MPUSHER);
        if (pmodule && pmodule->pModule && pmodule->pModule->IsLiveSession()) {

            pms::TransferMsg t_msg;

            //r_msg.set_svr_cmds(cmd);
            r_msg.set_tr_module(pms::ETransferModule::MCONNECTOR);
            r_msg.set_connector(CRTConnManager::Instance().ConnectorId());

            t_msg.set_type(pms::ETransferType::TQUEUE);
            t_msg.set_content(r_msg.SerializeAsString());

            std::string s = t_msg.SerializeAsString();
            pmodule->pModule->SendTransferData(s.c_str(), (int)s.length());
        } else {
            //LE("CRTDispatchConnection::DispatchMsg module pusher is not liveeeeeeeeeeee!!!\n");
        }
        return;
    } else { //!pci
        if (pci->_pConn && pci->_pConn->IsLiveSession()) {
            if (pci->_connType == pms::EConnType::THTTP) {
                CRTConnection *c = dynamic_cast<CRTConnection*>(pci->_pConn);
                if (c) {
                    c->SendDispatch(uid, r_msg.content());
                }
            } else if (pci->_connType == pms::EConnType::TTCP) {
                struct timeval tv;
                gettimeofday(&tv, NULL);

                LI("CRTDispatchConnection::DispatchMsg to user:%s, time:%lld, rmsg.content.length:%d\n", uid.c_str(), (long long)tv.tv_sec, r_msg.content().length());
                CRTConnectionTcp *ct = dynamic_cast<CRTConnectionTcp*>(pci->_pConn);
                if (ct) {
                    ct->SendDispatch(uid, r_msg.content());
                }
            }
        }
    }

}
