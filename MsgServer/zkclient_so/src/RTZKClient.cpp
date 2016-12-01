//
//  RTZKClient.cpp
//  MsgServer
//
//  Created by hp on 3/5/16.
//  Copyright © 2016 DYNC. All rights reserved.
//

#include "RTZKClient.hpp"
#include <iostream>
#include <vector>
#include "RTUtils.hpp"


RTZKClient::RTZKClient()
: m_conf_path("")
, m_client(NULL)
, m_server_node(NULL)
{
    m_listWs.clear();
    m_dataWs.clear();
    m_mapChildWs.clear();
    m_mapDataWs.clear();
}

RTZKClient::~RTZKClient()
{
    Unin();
}

int RTZKClient::InitOnly(const std::string& conf)
{
    m_conf_path = conf;
    return m_conf.init(m_conf_path);
}

int RTZKClient::InitZKClient(const std::string& conf)
{
    m_conf_path = conf;
    int res = m_conf.init(m_conf_path);
    if (res!=0) {
        printf("m_conf.init failed\n");
        return res;
    }

    m_client =  new gim::ZKClient();
    if (!m_client) return -1;
    res = m_client->init(m_conf.ZkUrl);
    if (-1==ParseAndCreateNode(m_conf.ModulePath)) return -1;

    res = InitStatusNode(m_conf);
    if (res == ZOK) {
        printf("InitStatusNode path:%s ip:%s ok!\n", m_conf.ModulePath.c_str(), m_conf.IP.c_str());
    } else if (res == ZNODEEXISTS) {
        printf("InitStatusNode node exists path:%s ip:%s ok!\n", m_conf.ModulePath.c_str(), m_conf.IP.c_str());
        return res;
    } else {
        printf("InitStatusNode error, path:%s ip:%s ok!\n", m_conf.ModulePath.c_str(), m_conf.IP.c_str());
        return res;
    }
    m_client->setLogFn(this, &RTZKClient::RTZKLogCallback);

    gim::ListWatcher< RTZKClient>* lw = NULL;
    gim::DataWatcher< RTZKClient>* dw = NULL;
    gim::s_vector c_v;

    //child watch
    m_client->getChildren(m_conf.ProjectPath, c_v);
    for(auto& x:c_v) {
        std::string s = gim::zkPath(m_conf.ProjectPath, x.c_str());
        lw = new gim::ListWatcher< RTZKClient >(this, &RTZKClient::ChildrenMapCallback, &RTZKClient::PathRemoveCallback);
        lw->init(m_client, s);
        lw->addWatch(gim::ZKWatcher::DELETE_EVENT);
        lw->addWatch(gim::ZKWatcher::CREATE_EVENT);
        m_listWs.push_back(lw);
    }

    //data watch
    dw = new gim::DataWatcher< RTZKClient >(this, &RTZKClient::RTZKCallback);
    dw->init(m_client, m_conf.ProjectPath);
    dw->addWatch(gim::ZKWatcher::CHANGE_EVENT);
    m_mapDataWs.insert(std::make_pair(m_conf.ProjectPath, dw));

    dw = new gim::DataWatcher< RTZKClient >(this, &RTZKClient::RTZKCallback);
    dw->init(m_client, m_conf.ModulePath);
    dw->addWatch(gim::ZKWatcher::CHANGE_EVENT);
    m_mapDataWs.insert(std::make_pair(m_conf.ProjectPath, dw));

    return 0;

}

int RTZKClient::InitStatusNode(gim::ServerConfig& conf){

    m_server_status.IP = conf.IP;
    m_server_status.Type = conf.Type;
    m_server_status.NodePath = conf.NodePath;
    m_server_status.ModulePath = conf.ModulePath;
    m_server_status.ProjectPath = conf.ProjectPath;
    m_server_status.autoSetIPs(true);
    m_server_node = new gim::ServerNode(m_client, conf.ModulePath, conf.IP);
    int ret = m_server_node->init(m_server_status);
    if(ret < 0){
        printf("m_server_node->init failed\n");
        return -12;
    }
    return 0;
}

int RTZKClient::Unin()
{
    for(auto& x : m_listWs){
        delete x;
        x = NULL;
    }
    m_listWs.clear();
    for(auto& x : m_dataWs){
        delete x;
        x = NULL;
    }
    m_dataWs.clear();
    for(auto& x : m_mapDataWs){
        delete x.second;
        x.second = NULL;
    }
    m_mapDataWs.clear();
    for(auto x : m_mapChildWs){
        delete x.second;
        x.second = NULL;
    }
    m_mapChildWs.clear();
    if (m_server_node) {
         delete m_server_node;
         m_server_node = NULL;
    }
    if (m_client) {
        delete m_client;
        m_client = NULL;
    }
     return 0;
}

void RTZKClient::SetNodeCallback(NodeAddCallback addCb, NodeDelCallback delCb)
{
    m_NodeAddCallback = addCb;
    m_NodeDelCallback = delCb;
}

void RTZKClient::SetDataWatchCallback(DataWatchCallback dataCb)
{
     m_DataWatchCallback = dataCb;
}

bool RTZKClient::CheckNodeExists(const std::string& nodePath)
{
    return ((m_mapChildWs.find(nodePath)==m_mapChildWs.end())?false:true);
}


void RTZKClient::RTZKLogCallback(void* ctx, const std::string& l)
{
    //printf("RTZKClient::RTZKLogCallback Log:%s\n", l.c_str());
}

int RTZKClient::RTZKDataCallback(void* ctx, int version, const std::string& data)
{
    //printf("RTZKClient::RTZDataCallback Data:%s\n", data.c_str());
    return 0;
}

////////////
int RTZKClient::RTZKCallback(int ver, const std::string& data)
{
    //printf("RTZKClient::RTZCallback data:%s\n", data.c_str());
    if (m_DataWatchCallback)
    {
        m_DataWatchCallback(data, "");
    }
    return 0;
}

int RTZKClient::ChildrenMapCallback(const gim::ChildrenMap& cmap)
{
    gim::ListWatcher< RTZKClient>* w = NULL;
    for (auto& x : cmap) {
        gim::ServerStatus ss;
        ss.parseFromString(x.second);
        if (m_mapChildWs.find(ss.NodePath)==m_mapChildWs.end()) {
            w = new gim::ListWatcher< RTZKClient >(this, &RTZKClient::ChildrenMapCallback, &RTZKClient::PathRemoveCallback);
            w->init(m_client, ss.NodePath);
            printf("RTZKClient::ChildredMapCallback node %s was added\n", ss.NodePath.c_str());
            if (m_NodeAddCallback!=nullptr)
            {
                 m_NodeAddCallback(ss.NodePath);
            }
            m_mapChildWs.insert(make_pair(ss.NodePath, w));

            gim::DataWatcher< RTZKClient>* dw = NULL;
            dw = new gim::DataWatcher< RTZKClient >(this, &RTZKClient::RTZKCallback);
            dw->init(m_client, ss.NodePath);
            dw->addWatch(gim::ZKWatcher::CHANGE_EVENT);
            m_mapDataWs.insert(std::make_pair(ss.NodePath, dw));
        }
    }
    return 0;
}

int RTZKClient::PathRemoveCallback(const std::string& path)
{
    ChildrenWatcherMapIt it = m_mapChildWs.find(path);
    if (it!=m_mapChildWs.end()) {
        printf("RTZKClient::PathRemoveCallback node %s was removed\n", path.c_str());
        if (m_NodeDelCallback!=nullptr)
        {
             m_NodeDelCallback(path);
        }
        delete it->second;
        it->second = NULL;
        m_mapChildWs.erase(it);

        DataWatcherMapIt dit = m_mapDataWs.find(path);
        if (dit != m_mapDataWs.end())
        {
            delete dit->second;
            dit->second = NULL;
            m_mapDataWs.erase(dit);
        }
    }
    return 0;
}

void RTZKClient::SetNodeData(const std::string& path, const std::string& data)
{
    if (m_client)
    {
        m_client->setNodeData(path, data);
    }
}

int RTZKClient::CreateEphemeralNode(const std::string& path, const std::string& data)
{
    if (m_client)
    {
        m_client->createEphemeralNode(path, data);
    }
}

int RTZKClient::CreatePersistentNode(const std::string& path, const std::string& data)
{
    if (m_client)
    {
        m_client->createPersistentNode(path, data);
    }
}

int RTZKClient::DeleteNode(const std::string& path)
{
    if (m_client)
    {
        m_client->deleteNode(path);
    }
}

int RTZKClient::ParseAndCreateNode(const std::string& zkUrl)
{
    std::vector<std::string> nodes;
    if (zkUrl.at(0)=='/')
    {
        Split("/", zkUrl.substr(1, zkUrl.length()), nodes);
        for(auto item : nodes)
        {
            printf("item :%s\n", item.c_str());
        }
        int countSlash = 0;
        int countNodes = nodes.size();
        while(countSlash<nodes.size())
        {
            countSlash++;
            std::string p("");
            for(int i=0;i<countSlash;i++)
            {
                p.append("/");
                p.append(nodes[i]);
            }
            if (ZkCheckNodeExists(p))
            {
                 continue;
            } else {
                if (countSlash>3)
                {
                    CreateEphemeralNode(p, nodes[countSlash-1]);
                } else {
                    CreatePersistentNode(p, nodes[countSlash-1]);
                }
            }
        }
        return 0;
    } else {
        printf("RTZKClient::ParseAndCreateNode zkUrl:%s, error\n", zkUrl.c_str());
         return -1;
    }
}

bool RTZKClient::ZkCheckNodeExists(const std::string& path)
{
     return m_client->checkNodeExists(path);
}

